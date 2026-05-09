#pragma once
#include "common.h"
#include "texture.h"
#include "pnmImage.h"
#include "stringStream.h"
#include "icon.h"

#include <cstdint>
#include <cmath>

static inline uint16_t ico_u16(const uint8_t* p) {
    return static_cast<uint16_t>(p[0])
        | static_cast<uint16_t>(p[1] << 8);
}
static inline uint32_t ico_u32(const uint8_t* p) {
    return static_cast<uint32_t>(p[0])
        | static_cast<uint32_t>(p[1] << 8)
        | static_cast<uint32_t>(p[2] << 16)
        | static_cast<uint32_t>(p[3] << 24);
}
static inline int32_t ico_i32(const uint8_t* p) {
    return static_cast<int32_t>(ico_u32(p));
}

static inline bool is_png(const uint8_t* p) {
    return p[0] == 0x89 && p[1] == 0x50 &&
        p[2] == 0x4E && p[3] == 0x47;
}

static bool load_png_from_memory(const uint8_t* png_data, size_t png_size, PNMImage& out_img) {
    StringStream ss;
    ss.write(reinterpret_cast<const char*>(png_data),
        static_cast<std::streamsize>(png_size));
    ss.seekg(0);

    if (!out_img.read(ss)) {
        std::cerr << "[icon] PNMImage failed to decode embedded PNG\n";
        return false;
    }
    return true;
}

inline PT(Texture) parse_ico_to_texture() {
    const uint8_t* const data = reinterpret_cast<const uint8_t*>(icon_bytes);
    constexpr size_t     file_size = sizeof(icon_bytes);

    // ICO file header
    if (file_size < 6 || ico_u16(data + 2) != 1) {
        std::cerr << "[icon] Not a valid ICO file\n";
        return nullptr;
    }
    const int count = static_cast<int>(ico_u16(data + 4));
    std::cout << std::format("[icon] ICO: {} entries, file_size={}\n",
        count, file_size);

    // ICONDIRENTRY layout (16 bytes):
    //   +0  bWidth        +1  bHeight
    //   +2  bColorCount   +3  bReserved
    //   +4  wPlanes (2B)  +6  wBitCount (2B)
    //   +8  dwBytesInRes (4B)
    //   +12 dwImageOffset (4B)
    int      best_entry = -1;
    int      best_pixels = 0;
    uint32_t best_offset = 0;
    uint32_t best_size = 0;
    int      best_w = 0;
    int      best_h = 0;

    for (int i = 0; i < count; ++i) {
        const size_t   dir_off = 6 + static_cast<size_t>(i) * 16;
        if (dir_off + 16 > file_size) break;

        const uint8_t* e = data + dir_off;
        const int      w = e[0] == 0 ? 256 : static_cast<int>(e[0]);
        const int      h = e[1] == 0 ? 256 : static_cast<int>(e[1]);
        const uint32_t sz = ico_u32(e + 8);
        const uint32_t offset = ico_u32(e + 12);
        const bool     fits = (static_cast<uint64_t>(offset) + sz) <= file_size;

        std::cout << std::format("[icon] Entry {}: {:3d}x{:3d}  "
            "size={:<6d}  offset={:<6d}  {}\n",
            i, w, h, sz, offset,
            fits ? "OK" : "OUT OF BOUNDS");

        if (fits && w * h > best_pixels) {
            best_pixels = w * h;
            best_entry = i;
            best_offset = offset;
            best_size = sz;
            best_w = w;
            best_h = h;
        }
    }

    if (best_entry < 0) {
        std::cerr << "[icon] No valid ICO entries found\n";
        return nullptr;
    }
    std::cout << std::format("[icon] Using entry {} ({}x{})\n", best_entry, best_w, best_h);

    const uint8_t* img_data = data + best_offset;

    // Detect image format: PNG or BMP
    PNMImage pnm;

    if (best_size >= 8 && is_png(img_data)) {
        // PNG path (Vista+ ICO for 256x256)
        std::cout << "[icon] Format: PNG (Vista+ ICO)\n";

        if (!load_png_from_memory(img_data, best_size, pnm)) {
            return nullptr;
        }
        std::cout << std::format("[icon] PNG decoded: {}x{}\n",
            pnm.get_x_size(), pnm.get_y_size());

    }
    else {
        std::cout << "[icon] Format: BMP (classic ICO)\n";

        if (best_size < 40) {
            std::cerr << "[icon] BMP header too small\n";
            return nullptr;
        }

        const uint32_t hdr_size = ico_u32(img_data + 0);
        const int32_t  bmp_w = ico_i32(img_data + 4);
        const int32_t  bmp_h_raw = ico_i32(img_data + 8);
        const uint16_t bit_count = ico_u16(img_data + 14);

        // ICO stores biHeight as (real_height * 2) for the AND mask
        const int real_w = bmp_w > 0 ? bmp_w : best_w;
        const int real_h = bmp_h_raw != 0 ? std::abs(bmp_h_raw) / 2 : best_h;

        std::cout << std::format("[icon] BMP: hdr={}  {}x{}  bits={}\n",
            hdr_size, real_w, real_h, bit_count);

        if (bit_count != 32) {
            std::cerr << std::format("[icon] Expected 32-bit BGRA, got {}\n",
                bit_count);
            return nullptr;
        }

        const uint8_t* px = img_data + hdr_size;
        const size_t   px_bytes = static_cast<size_t>(real_w) * real_h * 4;

        if (px + px_bytes > data + file_size) {
            std::cerr << "[icon] BMP pixel data out of bounds\n";
            return nullptr;
        }

        // BGRA bottom-up -> RGBA top-down
        pnm = PNMImage(real_w, real_h, 4);
        for (int row = 0; row < real_h; ++row) {
            const int      src_row = (real_h - 1) - row;
            const uint8_t* src = px + static_cast<size_t>(src_row) * real_w * 4;
            for (int col = 0; col < real_w; ++col) {
                pnm.set_xel_a(col, row,
                    src[col * 4 + 2] / 255.0f,   // R
                    src[col * 4 + 1] / 255.0f,   // G
                    src[col * 4 + 0] / 255.0f,   // B
                    src[col * 4 + 3] / 255.0f);  // A
            }
        }
    }

    // Upload to Panda3D Texture
    PT(Texture) tex = new Texture("panda3d_icon");
    tex->load(pnm);
    tex->set_magfilter(SamplerState::FT_linear);
    tex->set_minfilter(SamplerState::FT_linear);
    tex->set_wrap_u(SamplerState::WM_clamp);
    tex->set_wrap_v(SamplerState::WM_clamp);

    std::cout << std::format("[icon] Texture ready: {}x{} RGBA\n", pnm.get_x_size(), pnm.get_y_size());
    return tex;
}