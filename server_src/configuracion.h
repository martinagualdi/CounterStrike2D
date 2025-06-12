#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include <yaml-cpp/yaml.h>

class Configuracion{
private:
    static YAML::Node yaml;

public:
    static void cargar_path (const char *path_yaml);

    template <typename T>
    static T get(const std::string& campo);
};

#endif