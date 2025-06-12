#include "jugador.h"

#include <iostream>


void Jugador::disparar() {
    disparando = true;
}

void Jugador::recibir_danio(int danio) { 
    std::cout << "[recibir_danio] Antes: vida=" << vida << ", danio=" << danio << std::endl;
    vida -= danio; 
    if (vida <= 0) {
        vida = 0;
        vivo = false;
        std::cout << "Jugador " << id << " ha muerto." << std::endl;
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
    std::string str_arma = (arma == C_AK47) ? "ak47" : (arma == C_M3) ? "m3" : "awp";
    std::string str_precio = "precio_" + str_arma;
    int precio = Configuracion::get<int>(str_precio);
    switch (arma) {
        case C_AK47:
            if (dinero >= precio) {
                if(!arma_principal || arma_principal->getNombre() != "AK-47"){
                    arma_principal = std::make_unique<Ak47>();
                    dinero -= precio;
                    arma_en_mano = arma_principal.get();
                    acaba_de_comprar_arma = true; 
                    return true;
                } 
            }
            break;
        case C_M3:
            if (dinero >= precio) {
                if(!arma_principal || arma_principal->getNombre() != "M3"){
                    arma_principal = std::make_unique<m3>();
                    dinero -= precio;
                    arma_en_mano = arma_principal.get();
                    acaba_de_comprar_arma = true;
                    return true;
                }                
            }
            break;
        case C_AWP:
            if (dinero >= precio) {
                if(!arma_principal || arma_principal->getNombre() != "AWP"){
                    arma_principal = std::make_unique<Awp>();
                    dinero -= precio;
                    arma_en_mano = arma_principal.get();
                    acaba_de_comprar_arma = true;
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
    std::string str_tipo = (tipo_bala == BALAS_PRIMARIA) ? "principal" : "secundaria";
    std::string str_precio = "precio_municion_" + str_tipo;
    int precio = Configuracion::get<int>(str_precio);
    switch (tipo_bala) {
        case BALAS_PRIMARIA:
            if (dinero >= precio && arma_principal){
                arma_principal->agregarMunicion(10);
                dinero -= precio;
                acaba_de_comprar_balas = true;
                return true;
            }
            break;
        case BALAS_SECUNDARIA:
            if (dinero >= precio){
                arma_secundaria->agregarMunicion(10);
                dinero -= precio;
                acaba_de_comprar_balas = true;
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

void Jugador::sumar_eliminacion() {
    eliminaciones_esta_ronda++;
    eliminaciones_totales++;
}

void Jugador::reiniciar_compras() {
    if (acaba_de_comprar_arma)
        acaba_de_comprar_arma = false;
    if (acaba_de_comprar_balas)
        acaba_de_comprar_balas = false;
}

void Jugador::finalizar_ronda() {
    dinero += Configuracion::get<int>("dinero_por_eliminacion") * eliminaciones_esta_ronda;
    eliminaciones_esta_ronda = 0;
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
    vivo = true;
    moviendose = false;
    movimiento_actual = DETENER;
}
