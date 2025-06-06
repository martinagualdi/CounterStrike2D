#ifndef COMANDO_DTO_H
#define COMANDO_DTO_H

#include <string>
#include <cstdint>

enum Tipo {MOVIMIENTO, ROTACION, DISPARO, DESCONECTAR, CAMBIAR_ARMA, COMPRAR};
enum Movimiento {ARRIBA, ABAJO, IZQUIERDA, DERECHA, DIAGONAL_SUP_IZQ, DIAGONAL_SUP_DER, DIAGONAL_INF_IZQ, DIAGONAL_INF_DER, DETENER};
enum Compra{C_AK47, C_M3, C_AWP, BALAS_PRIMARIA, BALAS_SECUNDARIA, C_NULL};

struct ComandoDTO {
    enum Tipo tipo;
    enum Movimiento movimiento;
    enum Compra compra;
    uint8_t id_jugador;
    float angulo;
};

#endif