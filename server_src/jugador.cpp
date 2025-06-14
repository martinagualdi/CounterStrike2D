#include "jugador.h"

#include <iostream>
#include "bomba.h"


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

void Jugador::asignar_bomba() {
    bomba = std::make_unique<Bomba>();
    this->tiene_bomba = true;
    
    
}

void Jugador::cancelar_plantado_bomba() {
    if (tiene_bomba && plantando_bomba) {
        plantando_bomba = false;
        bomba->reiniciar();
    } else {
        std::cout << "No se puede cancelar el plantado de la bomba, no se está plantando." << std::endl;
    }
}

void Jugador::empezar_a_plantar() {
    if (tiene_bomba && !bomba->estaActivada()) {
        plantando_bomba = true;
        bomba->iniciarCuentaRegresiva();
    } else {
        std::cout << "No se puede empezar a plantar la bomba, ya está activada o no se tiene una." << std::endl;
    }
}

void Jugador::plantar_bomba(float x, float y) {
    if (tiene_bomba && !bomba->estaActivada()) {
        plantando_bomba = false;
        bomba->activar(x,y);
        tiene_bomba = false; // El jugador ya no tiene la bomba en mano
    } else {
        std::cout << "No se puede plantar la bomba, ya está activada o no se tiene una." << std::endl;
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
    } else if (arma_en_mano == cuchillo.get() && tiene_bomba) {
        arma_en_mano = bomba.get();
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

ArmaDeFuego* Jugador::soltar_arma_pricipal() {
    if (arma_principal) {
        arma_en_mano = arma_secundaria.get(); 
        ArmaDeFuego* arma = arma_principal.release();
        return arma;
    }
    return nullptr; 
}

ArmaDeFuego* Jugador::levantar_arma(ArmaDeFuego* arma_del_suelo) {
    ArmaDeFuego* arma_soltar = nullptr;
    if (arma_principal) 
        arma_soltar = arma_principal.release(); 
    arma_principal.reset(arma_del_suelo);
    arma_en_mano = arma_principal.get();
    acaba_de_comprar_arma = true; // Uso el mismo booleano para no sobrecargar la lógica
    return arma_soltar; 
}

void Jugador::reiniciar() {
    x = spawn_x;
    y = spawn_y;
    angulo = 0;
    vida = 100;
    vivo = true;
    moviendose = false;
    movimiento_actual = DETENER;
}
