#include "jugador.h"

bool Jugador::disparar() {
    return arma_en_mano->puedeAccionar();
}

void Jugador::recibir_danio(int danio) { 
    vida -= danio; 
    if (vida <= 0) {
        vida = 0;
        vivo = false;
    }
}

Arma* Jugador::get_arma_actual() const {
    return arma_en_mano;
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
            if (dinero >= 150) {
                if(!arma_principal){
                    arma_principal = std::make_unique<Ak47>();
                    dinero -= 150;
                    arma_en_mano = arma_principal.get();
                    return true;
                } else if(arma_principal->getNombre() != "AK-47"){
                    arma_principal = std::make_unique<Ak47>();
                    dinero -= 150;
                    arma_en_mano = arma_principal.get();
                    return true;
                }
            }
            break;
        case C_M3:
            if (dinero >= 100) {
                if(!arma_principal){
                    arma_principal = std::make_unique<m3>();
                    dinero -= 100;
                    arma_en_mano = arma_principal.get();
                    return true;
                } else if(arma_principal->getNombre() != "M3"){
                    arma_principal = std::make_unique<m3>();
                    dinero -= 100;
                    arma_en_mano = arma_principal.get();
                    return true;
                }                
            }
            break;
        case C_AWP:
            if (dinero >= 200) {
                if(!arma_principal){
                    arma_principal = std::make_unique<Awp>();
                    dinero -= 200;
                    arma_en_mano = arma_principal.get();
                    return true;
                }
                else if(arma_principal->getNombre() != "AWP"){
                    arma_principal = std::make_unique<Awp>();
                    dinero -= 200;
                    arma_en_mano = arma_principal.get();
                    return true;
                }
            }
            break;
        default:
            break;
    }
    return false;
}

bool Jugador::comprarBalas(enum Compra tipo_bala) {
    switch (tipo_bala) {
        case BALAS_PRIMARIA:
            if (dinero >= 5){
                arma_principal->agregarMunicion(10);
                dinero -= 5;
                return true;
            }
            break;
        case BALAS_SECUNDARIA:
            if (dinero >= 2){
                arma_secundaria->agregarMunicion(10);
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

void Jugador::definir_spawn(float x, float y) {
    this->x = x;
    this->y = y;
    this->spawn_x = x;
    this->spawn_y = y;
}

void Jugador::reiniciar() {
    /*HARDCODEADO, REDEFINIR CON YAML DE CONFIG*/
    x = spawn_x;
    y = spawn_y;
    angulo = 0;
    vida = 100;
    dinero = 500;
    vivo = true;
    moviendose = false;
    movimiento_actual = DETENER;
}
