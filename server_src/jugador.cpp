#include "jugador.h"

bool Jugador::disparar() {
    /* HARCODEADO PAEA PROBAR DISPARO */
    disparando = true;
    if (arma_en_mano->accion(0.0f) > 0)
        return true;
    return false;
}

void Jugador::recibir_danio(int danio) { 
    vida -= danio; 
    if (vida <= 0) {
        vida = 0;
        vivo = false;
    }
}

std::string Jugador::get_nombre_arma_en_mano() {
    return arma_en_mano->getNombre();
}

void Jugador::cambiar_estado_moviendose() { moviendose = !moviendose; }

void Jugador::cambiar_arma_en_mano() {
    if (arma_en_mano == arma_principal.get()) {
        arma_en_mano = arma_secundaria.get();
    } else if (arma_en_mano == arma_secundaria.get()) {
        arma_en_mano = cuchillo.get();
    } else {
        if(arma_principal != nullptr)
            arma_en_mano = arma_principal.get();
        else
            arma_en_mano = arma_secundaria.get();
    }
}

bool Jugador::comprarArma(enum Compra arma) {
    switch (arma) {
        case C_AK47:
            if (dinero >= 150 && arma_principal != std::make_unique<Ak47>()) {
                arma_principal = std::make_unique<Ak47>();
                dinero -= 150;
                arma_en_mano = arma_principal.get();
                return true;
            }
            break;
        case C_M3:
            if (dinero >= 100 && arma_principal != std::make_unique<m3>()) {
                arma_principal = std::make_unique<m3>();
                dinero -= 100;
                arma_en_mano = arma_principal.get();
                return true;
            }
            break;
        case C_AWP:
            if (dinero >= 200 && arma_principal != std::make_unique<Awp>()) {
                arma_principal = std::make_unique<Awp>();
                dinero -= 200;
                arma_en_mano = arma_principal.get();
                return true;
            }
            break;
        default:
            return false; // No se compró nada
    }
    return false;
}

bool Jugador::comprarBalas(enum Compra tipo_bala) {
    switch (tipo_bala) {
        case BALAS_PRIMARIA:
            if (dinero >= 5){
                arma_principal->setMunicion(10);
                dinero -= 5;
                return true;
            }
            break;
        case BALAS_SECUNDARIA:
            if (dinero >= 2){
                arma_secundaria->setMunicion(10);
                dinero -= 2;
                return true;
            }
            break;
        default:
            return false; // No se compró nada
    }
    return false;
}

enum ArmaEnMano Jugador::get_codigo_arma_en_mano() {
    std::string nombre_arma = arma_en_mano->getNombre();
    if (nombre_arma == "Cuchillo") {
        return CUCHILLO;
    } else if (nombre_arma == "Glock") {
        return GLOCK;
    } else if (nombre_arma == "AK-47") {
        return AK_47;
    } else if (nombre_arma == "M3") {
        return M3;
    } else if (nombre_arma == "AWP") {
        return AWP;
    } else {
        return BOMBA_TT;
    }
}

void Jugador::reiniciar() {
    /*HARDCODEADO, REDEFINIR CON YAML DE CONFIG*/
    x = 1000;
    y = 1000;
    angulo = 0;
    vida = 100;
    dinero = 500;
    vivo = true;
    moviendose = false;
    movimiento_actual = DETENER;
}
















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
