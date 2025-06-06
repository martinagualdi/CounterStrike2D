#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>

typedef enum {FONDO, OBSTACULO, PISO, ARMA, INICIO_CT, INICIO_TT, ZONA_BOMBAS} TipoElementoMapa;

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
    std::string inicio_mapa_dto;

public:
    Mapa(std::string yamlPath);
    std::string mapa_en_estado_inicial() const { return inicio_mapa_dto; }

    bool jugador_colision_contra_pared(float pos_x, float pos_y);
    bool bala_colision_contra_pared(float pos_x, float pos_y);

};

#endif