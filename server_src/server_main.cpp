#include <cstring>
#include <exception>
#include <iostream>
#include <string>
#include <vector>
#include <yaml-cpp/yaml.h>

#include "server.h"
#include "../common_src/configuracion.h"

#define MIN_ARGUMNENTOS_SERVIDOR 2
#define RET_ERROR 1
#define RET_EXITO 0

int main(int argc, char *argv[]) {
    if (argc != MIN_ARGUMNENTOS_SERVIDOR)
        return RET_ERROR;

    try {
        const char *config_file = argv[1];

        /*YAML::Node config = YAML::LoadFile(config_file);
        if (!config["puerto"]) {
            std::cerr << "Error: el archivo de configuración no contiene 'puerto'\n";
            return RET_ERROR;
        }*/

        //std::string port = config["puerto"].as<std::string>();
        Configuracion::cargar_path(config_file);
        Server server((Configuracion::get<std::string>("puerto")).c_str());
        server.start();

    } catch (const std::exception &e) {
        std::cerr << "Error al ejecutar el servidor: " << e.what() << std::endl;
        return RET_ERROR;
    }

    return RET_EXITO;
}
