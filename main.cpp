#include "game_app.h"
#include "prc_config.h"
#include "executionEnvironment.h"

int main(int argc, char* argv[]) {
    load_embedded_prc();

    std::string exe_dir = static_cast<Filename>(ExecutionEnvironment::get_binary_name()).get_dirname();
    exe_dir = Filename::from_os_specific(exe_dir).get_fullpath();

    ExecutionEnvironment::set_environment_variable("PANDA_ROOT", exe_dir);
    
#ifdef DEBUG_LOG
    std::cout << ".EXE path dir: " << exe_dir << std::endl;
#endif

    // load_prc_file_data("", ("model-path " + exe_dir + "/models").c_str());

    GameApp game;

    if (!game.init(argc, argv)) {
        std::cerr << "Failed to initialize game.\n";
        return EXIT_FAILURE;
    }

    game.run();
    game.shutdown();

    return EXIT_SUCCESS;
}