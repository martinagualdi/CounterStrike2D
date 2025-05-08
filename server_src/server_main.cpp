#include "server.h"

const int CANTIDAD_ARGS = 2;
const int FIRST_ARG = 0;
const int SECOND_ARG = 1;
const int ERROR = 1;
const int SUCCESS = 0;

void verificar_argumentos(int argc, const char** argv) {
    if (argc != CANTIDAD_ARGS) {
        std::cout << "Uso: " << argv[FIRST_ARG] << " <puerto>" << std::endl;
        exit(ERROR);
    }
}

int main(int argc, const char** argv) {
    verificar_argumentos(argc, argv);

    Server server(std::stoi(argv[SECOND_ARG]));
    server.run();
    return SUCCESS;
}