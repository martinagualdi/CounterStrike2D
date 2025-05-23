#ifndef COMANDO_DTO_H
#define COMANDO_DTO_H

#include <string>

enum Tipo {MOVIMIENTO, ROTACION, DISPARO, DESCONECTAR};
enum Movimiento {ARRIBA, ABAJO, IZQUIERDA, DERECHA, DIAGONAL_SUP_IZQ, DIAGONAL_SUP_DER, DIAGONAL_INF_IZQ, DIAGONAL_INF_DER, DETENER};

struct ComandoDTO {
    enum Tipo tipo;
    enum Movimiento movimiento;
    uint8_t id_jugador;
    float angulo;
};

#endif