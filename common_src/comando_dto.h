#ifndef COMANDO_DTO_H
#define COMANDO_DTO_H

#include <string>

enum Tipo {MOVIMIENTO, ROTACION, DISPARO};
enum Movimiento{ARRIBA, ABAJO, IZQUIERDA, DERECHA};

struct ComandoDTO {
    enum Tipo tipo;
    enum Movimiento movimiento;
    uint8_t id_jugador;
    uint16_t mouseX;
    uint16_t mouseY;
};

#endif