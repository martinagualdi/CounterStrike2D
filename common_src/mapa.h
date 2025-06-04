#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>

typedef enum {FONDO, OBSTACULO, PISO, ARMA, INICIO_CT, INICIO_TT} TipoElementoMapa;

struct ElementoDeMapa {
    int x;  
    int y;
    int ancho;
    int alto;
    TipoElementoMapa tipo;
};

class Mapa {
private:
    int ancho_mapa;
    int alto_mapa;
    std::vector<ElementoDeMapa> elementos;
public:
    Mapa(std::string yamlPath);

};

#endif