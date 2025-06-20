#ifndef COMANDO_DTO_H
#define COMANDO_DTO_H

#include <string>
#include <cstdint>
#include "enums_jugador.h"
#include "../server_src/configuracion.h"

enum Tipo {
    MOVIMIENTO, 
    ROTACION,
    DISPARO,
    DESCONECTAR,
    CAMBIAR_ARMA,
    COMPRAR,
    SELECCIONAR_SKIN,
    LEVANTAR,
    DROPEAR,
    ACCION_SOBRE_BOMBA
};
enum Movimiento {
    ARRIBA,
    ABAJO,
    IZQUIERDA,
    DERECHA,
    DIAGONAL_SUP_IZQ,
    DIAGONAL_SUP_DER,
    DIAGONAL_INF_IZQ,
    DIAGONAL_INF_DER,
    DETENER
};

enum EstadoBomba{
    ACCIONANDO,
    DETENIDO,
};

enum Compra{
    C_AK47,
    C_M3,
    C_AWP,
    BALAS_PRIMARIA,
    BALAS_SECUNDARIA
};

struct ComandoDTO {
    enum Tipo tipo;
    enum Movimiento movimiento;
    enum Compra compra;
    enum SkinTipos skin;
    enum EstadoBomba estado_bomba;
    uint8_t id_jugador;
    float angulo;
};

struct InfoConfigClient {
    int precio_awp;
    int precio_ak47;
    int precio_m3;
    int opacidad;
    int angulo_vision;
    int radio_vision;

    explicit InfoConfigClient(bool es_server) {
        if (es_server) {
            precio_awp = Configuracion::get<int>("precio_awp");
            precio_ak47 = Configuracion::get<int>("precio_ak47");
            precio_m3 = Configuracion::get<int>("precio_m3");
            angulo_vision = Configuracion::get<int>("angulo_cono_vision");
            opacidad = Configuracion::get<int>("opacidad");
            radio_vision = Configuracion::get<int>("radio_vision");
        } else {
            precio_awp = 0;
            precio_ak47 = 0;
            precio_m3 = 0;
            opacidad = 0;
            angulo_vision = 0;
            radio_vision = 0;
        }
    }
};

#endif