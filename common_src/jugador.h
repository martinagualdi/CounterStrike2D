#ifndef JUGADOR_H
#define JUGADOR_H

#include "comando_dto.h"

class Jugador {
  private:
    int id;
    float x;
    float y;
    float angulo;
    enum Movimiento movimiento_actual;

  public:
    explicit Jugador(int id) : id(id), x(10), y(10), angulo(0), movimiento_actual(DETENER) {};
    Jugador(int id, float x, float y, float angulo) : id(id), x(x), y(y), angulo(angulo) {}

    int getId() const {
        return id;
    }

    float getX() const {
        return x;
    }
    float getY() const {
        return y;
    }

    void setX(float posX) {
        this->x = posX;
    }
    void setY(float posY) {
        this->y = posY;
    }

    void setAngulo(float angulo){
        this->angulo = angulo;
    }

    float getAngulo() const {
        return angulo;
    }

    bool estaMoviendo() const {
        return movimiento_actual != DETENER;
    }

    enum Movimiento getMovimiento() const {
        return movimiento_actual;
    }

    void setMovimiento(enum Movimiento m){
        this->movimiento_actual = m;
    }

    Jugador(const Jugador &) = default;
    Jugador &operator=(const Jugador &) = default;
    Jugador(Jugador &&) = default;
    Jugador &operator=(Jugador &&) = default;
};

#endif