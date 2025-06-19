#ifndef MAPA_H
#define MAPA_H

#include <vector>
#include <string>
#include <yaml-cpp/yaml.h>
#include "../common_src/enums_jugador.h"

typedef enum {FONDO, OBSTACULO, PISO, ARMA, INICIO_CT, INICIO_TT, ZONA_BOMBAS} TipoElementoMapa;

struct ArmaDefaultDelMapa {
    std::string nombre;
    int x;
    int y;
};

struct Area{
    int x;
    int y;
    int ancho;
    int alto;
};

struct ElementoDeMapa {
    Area area;
    TipoElementoMapa tipo;
};

struct ZonaBombas {
    Area zona_bombas_a;
    Area zona_bombas_b;
};

class Mapa {
private:
    int ancho_mapa;
    int alto_mapa;
    Area inicio_ct;
    Area inicio_tt;
    ZonaBombas zona_bombas;
    std::vector<ElementoDeMapa> elementos;
    std::string inicio_mapa_dto;
    int cantidad_zonas_bombas = 0;
    std::vector<ArmaDefaultDelMapa> armas_en_suelo;

    Area definir_inicio(const int x, const int y, const int ancho, const int alto);

public:
    Mapa(std::string yamlPath);
    std::string mapa_en_estado_inicial() const { return inicio_mapa_dto; }
    bool jugador_colision_contra_pared(float pos_x, float pos_y);
    bool bala_colision_contra_pared(float pos_x, float pos_y);
    int limite_alto() const { return alto_mapa; }
    int limite_ancho() const { return ancho_mapa; }
    std::vector<float> dar_posiciones_iniciales(enum Equipo equipo);
    bool en_posicion_de_compra(float pos_x, float pos_y, enum Equipo equipo);
    bool en_posicion_de_bomba_a(float pos_x, float pos_y);
    bool en_posicion_de_bomba_b(float pos_x, float pos_y);
    bool verificar_zona_bombas(float pos_x, float pos_y);
    int verificar_cantidad_zonas_bombas() const { return cantidad_zonas_bombas; }
    std::vector<ArmaDefaultDelMapa> get_armas_default() const { return armas_en_suelo; }
};

#endif