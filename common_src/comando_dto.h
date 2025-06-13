#ifndef COMANDO_DTO_H
#define COMANDO_DTO_H

#include <string>
#include <cstdint>
#include "enums_jugador.h"

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

#endif