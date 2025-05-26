/*

#include "jugador.h"



int Jugador::getId() const {
    return id;
}

float Jugador::getX() const {
    return x;
}
float Jugador::getY() const {
    return y;
}

void Jugador::setX(float posX) {
    this->x = posX;
}
void Jugador::setY(float posY) {
    this->y = posY;
}

void Jugador::setAngulo(float angulo){
    this->angulo = angulo;
}

float Jugador::getAngulo() const {
    return angulo;
}

bool Jugador::estaMoviendo() const {
    return movimiento_actual != DETENER;
}

enum Movimiento Jugador::getMovimiento() const {
    return movimiento_actual;
}

void Jugador::setMovimiento(enum Movimiento m){
    this->movimiento_actual = m;
}

int Jugador::getVida() const {
    return vida;
}
void Jugador::setArmaPrimaria(ArmaDeFuego* nuevaArma) {
    if (arma_primaria != nullptr) {
        delete arma_primaria;
    }
    arma_primaria = nuevaArma;
}

void Jugador::cambiarASecundaria() {
    arma_actual = arma_secundaria;
}
void Jugador::cambiarAPrimaria() {
    arma_actual = arma_primaria;
}
void Jugador::cambiarACuchillo() {
    arma_actual = cuchillo;
}
ArmaDeFuego* Jugador::getArmaPrimaria() const {
    return arma_primaria;
}
ArmaDeFuego* Jugador::getArmaSecundaria() const {
    return arma_secundaria;
}

int Jugador::getDinero() const {
    return dinero;
}
void Jugador::recibirRecompensa(int recompensa) {
    this->dinero = dinero+recompensa;
}
int Jugador::getMunicionSecundaria() const {
    return arma_secundaria->getMunicion();
}
void Jugador::recibirMunicionSecundaria(int cantidad) {
    this->arma_secundaria->setMunicion(cantidad);
}
int Jugador::getMunicionPrimaria() const {
    return arma_primaria->getMunicion();
}
void Jugador::recibirMunicionPrimaria(int cantidad) {
    this->arma_primaria->setMunicion(cantidad);
}

void Jugador::recibirDanio(int danio) {
    if (danio < 0) return; // No se puede recibir daño negativo
    vida -= danio;
    if (vida <= 0) {
        vida = 0;
        vivo = false;
    }
}
bool Jugador::estaVivo() const {
    return vivo;
}
void Jugador::revivir() {
    vida = 100;
    vivo = true;
}
void Jugador::setEquipo(enum Equipo equipo) {
    this->equipo_actual = equipo;
}
enum Equipo Jugador::getEquipo() const {
    return equipo_actual;
}

int Jugador::disparar(float distancia) {
    return arma_actual->accion(distancia);
}

Jugador::~Jugador() {
    delete arma_primaria;
    delete arma_secundaria;
    delete cuchillo;
}
*/
