#ifndef JUGADOR_H
#define JUGADOR_H
#include <memory>
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
    int vida;
    int dinero;
    enum Equipo equipo_actual;
    bool vivo = true;
    //std::unique_ptr<Cuchillo> cuchillo;
    //Arma* arma_actual;
    //std::unique_ptr<ArmaDeFuego> arma_secundaria;
    //std::unique_ptr<ArmaDeFuego> arma_primaria;

  public:
    explicit Jugador(int id) : id(id), x(10), y(10), angulo(0), movimiento_actual(DETENER), vida(100), dinero(500),vivo(true)/*
    ,cuchillo(std::make_unique<Cuchillo>()), arma_actual(cuchillo.get()), arma_secundaria(std::make_unique<Glock>()), arma_primaria(nullptr)*/ {};

    Jugador(int id, float x, float y, float angulo) : id(id), x(x), y(y), angulo(angulo),movimiento_actual(DETENER), vida(100), dinero(500),vivo(true)/*
    ,cuchillo(std::make_unique<Cuchillo>()), arma_actual(cuchillo.get()), arma_secundaria(std::make_unique<Glock>()), arma_primaria(nullptr)*/{};

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
    /*
    void setArmaPrimaria(std::unique_ptr<ArmaDeFuego> nuevaArma) {
        arma_primaria = std::move(nuevaArma);
    }

    void cambiarASecundaria() {
        arma_actual = arma_secundaria.get();
    }
    void cambiarAPrimaria() {
        arma_actual = arma_primaria.get();
    }
    void cambiarACuchillo() {
        arma_actual = cuchillo.get();
    }
    ArmaDeFuego* getArmaPrimaria() const {
        return arma_primaria.get();
    }
    ArmaDeFuego* getArmaSecundaria() const {
        return arma_secundaria.get();
    }

    int getDinero() const {
        return dinero;
    }
    void recibirRecompensa(int recompensa) {
        this->dinero = dinero+recompensa;
    }
    int getMunicionSecundaria() const {
        return arma_secundaria->getMunicion();
    }
    void recibirMunicionSecundaria(int cantidad) {
        this->arma_secundaria->setMunicion(cantidad);
    }
    int getMunicionPrimaria() const {
        return arma_primaria->getMunicion();
    }
    void recibirMunicionPrimaria(int cantidad) {
        this->arma_primaria->setMunicion(cantidad);
    }

    void recibirDanio(int danio) {
        if (danio < 0) return; // No se puede recibir daño negativo
        vida -= danio;
        if (vida <= 0) {
            vida = 0;
            vivo = false;
        }
    }
    bool estaVivo() const {
        return vivo;
    }
    void revivir() {
        vida = 100;
        vivo = true;
    }
    void setEquipo(enum Equipo equipo) {
        this->equipo_actual = equipo;
    }
    enum Equipo getEquipo() const {
        return equipo_actual;
    }
    
    int disparar(float distancia) {
        return arma_actual->accion(distancia);
    }

    */
    
    ~Jugador() {
       
    }

    Jugador(const Jugador &) = default;
    Jugador &operator=(const Jugador &) = default;
    Jugador(Jugador &&) = default;
    Jugador &operator=(Jugador &&) = default;
};

#endif