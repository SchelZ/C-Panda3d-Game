#pragma once

#include <array>
#include <cstdint>
#include <cstddef>
#include <cstring>
#include <string>

// Platform-specific secure zero
#if defined(_WIN32)
#include <windows.h>
#endif

#ifdef STR_ENCRYPT

// ── Secure zero (cross-platform) ──────────────────────────────────
// Guarantees the compiler cannot optimize the zeroing loop away,
// even under LTO when the buffer is about to go out of scope.
inline void estr_secure_zero(void* p, std::size_t n) noexcept {
#if defined(_WIN32)
    SecureZeroMemory(p, n);
#elif defined(__STDC_LIB_EXT1__) && defined(__STDC_WANT_LIB_EXT1__)
    memset_s(p, n, 0, n);
#else
    // Generic fallback: volatile write + compiler barrier.
    // The volatile pointer prevents the writes from being treated
    // as dead stores, and the asm memory clobber prevents the
    // compiler from reordering or eliding them around scope exit.
    volatile unsigned char* vp = static_cast<volatile unsigned char*>(p);
    for (std::size_t i = 0; i < n; ++i) vp[i] = 0;
#if defined(__GNUC__) || defined(__clang__)
    asm volatile("" : : "r"(p) : "memory");
#endif
#endif
}

// ── Compile-time key generation ───────────────────────────────────
// Mixes seed, index, and the previous ciphertext byte so a known
// plaintext prefix does not leak the keystream for the remainder
// of the string. Adding (index + 1) avoids index==0 collapsing the
// mix to a pure function of the seed.
constexpr uint8_t enc_key_byte(uint32_t seed, uint32_t index, uint8_t prev) {
    uint32_t k = seed
        ^ ((index + 1u) * 2654435761u)
        ^ (static_cast<uint32_t>(prev) * 0x9E3779B1u);
    k ^= (k >> 16);
    k *= 0x45d9f3bu;
    k ^= (k >> 16);
    return static_cast<uint8_t>(k & 0xFF);
}

// ── EncryptedString<SEED, N> ──────────────────────────────────────
// Stores XOR ciphertext at compile time. Decrypts on demand into a
// stack buffer that is securely zeroed when it goes out of scope.
//
// USAGE:
//   std::string s = ESTR("foo");                    // implicit copy
//   legacy_api(ESTR("path").decrypt().c_str());     // one-shot C API
//   { auto d = ESTR("k").decrypt(); use(d); ... }   // repeated use,
//                                                   //   zeroed at }
//
// PITFALL: do NOT bind `const char*` to the implicit conversion of
// an EncryptedString — there is no such conversion, by design, because
// the underlying buffer would already be zeroed.
template<uint32_t SEED, std::size_t N>
struct EncryptedString {
    std::array<uint8_t, N> data{};

    // consteval (C++20) guarantees encryption happens at compile time.
    // Falls back to constexpr on older standards.
#if __cplusplus >= 202002L
    consteval EncryptedString(const char(&str)[N]) {
#else
    constexpr EncryptedString(const char(&str)[N]) {
#endif
        uint8_t prev = 0;
        for (std::size_t i = 0; i < N; ++i) {
            uint8_t k = enc_key_byte(SEED, static_cast<uint32_t>(i), prev);
            uint8_t c = static_cast<uint8_t>(str[i]) ^ k;
            data[i] = c;
            prev = c; // chain on ciphertext (CBC-style)
        }
    }

    // ── Decrypted ─────────────────────────────────────────────────
    // Temporary stack buffer — created on implicit conversion,
    // securely zeroed by destructor so plaintext doesn't linger.
    struct Decrypted {
        char buf[N]{};

        Decrypted(const Decrypted&) = delete;
        Decrypted& operator=(const Decrypted&) = delete;
        Decrypted(Decrypted&&) = default;
        Decrypted& operator=(Decrypted&&) = delete;

        explicit Decrypted(const std::array<uint8_t, N>& cipher) {
            uint8_t prev = 0;
            for (std::size_t i = 0; i < N; ++i) {
                uint8_t c = cipher[i];
                uint8_t k = enc_key_byte(SEED,
                    static_cast<uint32_t>(i),
                    prev);
                buf[i] = static_cast<char>(c ^ k);
                prev = c;
            }
        }

        ~Decrypted() {
            estr_secure_zero(buf, N);
        }

        operator const char* () const { return buf; }
        operator std::string() const {
            // Cut at first null so embedded-null inputs don't leak
            // trailing bytes into the string's length.
            return std::string(buf, ::strnlen(buf, N));
        }
        const char* c_str() const { return buf; }
    };

    // Implicit conversion — triggers decryption without .decrypt().
    //
    // Two safe conversion targets:
    //   • std::string  — copies plaintext into the string; the
    //                    decrypted temporary is securely zeroed.
    //   • Decrypted    — bind to a named local for repeated use:
    //                      auto d = ESTR("foo").decrypt();
    //                      use(d.c_str()); use(d.c_str());
    //                    The buffer is zeroed when `d` goes out of scope.
    //
    // There is intentionally NO `operator const char*()` because a
    // returned pointer would dangle into an already-destroyed buffer.
    // For one-shot C-API calls, use .decrypt():
    //      legacy_api(ESTR("path").decrypt().c_str());
    // The Decrypted temporary lives until the end of that full-expression.
    operator std::string() const {
        Decrypted d(data);
        return std::string(d.buf, ::strnlen(d.buf, N));
    }

    // Explicit decryption — caller controls lifetime.
    Decrypted decrypt() const { return Decrypted(data); }
    };

// ── Seed ──────────────────────────────────────────────────────────
#define ESTR_SEED(line, counter)       \
    static_cast<uint32_t>(             \
        ((line)    * 2246822519u) ^    \
        ((counter) * 3266489917u) ^    \
        0xDEADBEEFu                    \
    )

// ── ESTR() — encrypted path ───────────────────────────────────────
// Returns an EncryptedString. Use as `std::string s = ESTR("text");`
// or `ESTR("text").decrypt().c_str()` for C APIs.
#define ESTR(str) \
    (EncryptedString<ESTR_SEED(__LINE__, __COUNTER__), sizeof(str)>{str})

#else  // STR_ENCRYPT not defined ─────────────────────────────────

// ── ESTR() — passthrough path ─────────────────────────────────────
// Returns the string literal directly. Zero overhead, fully readable
// in the debugger and logs.
#define ESTR(str) (str)

#endif  // STR_ENCRYPT