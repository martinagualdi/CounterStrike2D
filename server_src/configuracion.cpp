#include "configuracion.h"

YAML::Node Configuracion::yaml;

void Configuracion::cargar_path (const char *path_yaml) { yaml = YAML::LoadFile(path_yaml); }

template <typename T>
T Configuracion::get(const std::string& campo) { 
    try {
        return yaml[campo].as<T>(); 
    } catch (...) {
        throw std::runtime_error("Campo " + campo + " no encontrado en el archivo de configuracion.");
    }
}

template std::string Configuracion::get<std::string>(const std::string& campo);
template int Configuracion::get<int>(const std::string& campo);
template float Configuracion::get<float>(const std::string& campo);