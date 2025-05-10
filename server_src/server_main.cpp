#include <cstring>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

#include "server.h"

#define MIN_ARGUMNENTOS_SERVIDOR 2
#define RET_ERROR 1
#define RET_EXITO 0

int main(int argc, char *argv[]) {

    if (argc != MIN_ARGUMNENTOS_SERVIDOR)
        return RET_ERROR;
    try {
        const char *port = argv[1];

        Server server(port);
        server.start();
    } catch (const std::exception &e) {
        std::cerr << "Error al ejecutar el servidor: " << e.what() << std::endl;
        return RET_ERROR;
    }
    return RET_EXITO;
}
