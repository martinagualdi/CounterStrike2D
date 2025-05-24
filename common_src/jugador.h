#ifndef JUGADOR_H
#define JUGADOR_H

#include "comando_dto.h"
#include "cuchillo.h"
#include "arma.h"
#include "glock.h"
enum Equipo {
    CT,
    TT
};

class Jugador {
  private:
    int id;
    float x;
    float y;
    float angulo;
    enum Movimiento movimiento_actual;
    int vida = 100;
    int dinero = 0;
    int municion_secundaria = 0;
    int municion_primaria = 0;
    enum Equipo equipo_actual;
    bool vivo = true;
    Cuchillo cuchillo;
    Arma* arma_actual = &cuchillo;
    Glock glock;
    Arma* arma_secundaria = &glock;
    Arma* arma_primaria = nullptr;


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

    int getVida() const {
        return vida;
    }
    void setArma(Arma* nuevaArma) {
        if (arma_primaria != nullptr) {
            delete arma_primaria;
        }
        arma_primaria = nuevaArma;
    }
    Arma* getArmaPrimaria() const {
        return arma_primaria;
    }
    Arma* getArmaSecundaria() const {
        return arma_secundaria;
    }

    int getDinero() const {
        return dinero;
    }
    void recibirRecompensa(int recompensa) {
        this->dinero = dinero+recompensa;
    }
    int getMunicionSecundaria() const {
        return municion_secundaria;
    }
    void recibirMunicionSecundaria(int cantidad) {
        this->municion_secundaria = municion_secundaria+cantidad;
    }
    int getMunicionPrimaria() const {
        return municion_primaria;
    }
    void recibirMunicionPrimaria(int cantidad) {
        this->municion_primaria = municion_primaria+cantidad;
    }
    

    Jugador(const Jugador &) = default;
    Jugador &operator=(const Jugador &) = default;
    Jugador(Jugador &&) = default;
    Jugador &operator=(Jugador &&) = default;
};

#endif