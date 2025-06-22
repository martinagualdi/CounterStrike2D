#include "jugador.h"

#include <iostream>
#include "bomba.h"


void Jugador::disparar() {
    disparando = true;
}

void Jugador::recibir_danio(int danio) { 
    vida -= danio; 
    if (vida <= 0) {
        vida = 0;
        vivo = false;
        muertes++;
    }
}

Arma* Jugador::get_arma_actual() const {
    return arma_en_mano;
}

void Jugador::asignar_bomba() {
    if (equipo_actual == CT){
        return; 
    }
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

void Jugador::cancelar_desactivado_bomba(){
    if (desactivando_bomba){
        desactivando_bomba=false;
    }
}

void Jugador::empezar_a_plantar() {
    if (tiene_bomba && !bomba->estaActivada()) {
        plantando_bomba = true;
    } else {
        std::cout << "No se puede empezar a plantar la bomba, ya está activada o no se tiene una." << std::endl;
    }
}

void Jugador::empezar_a_desactivar() {
    desactivando_bomba=true;
}

void Jugador::desactivar_bomba() {
    desactivando_bomba = false;
}

void Jugador::plantar_bomba(float x, float y) {
    if (tiene_bomba && !bomba->estaActivada()) {
        plantando_bomba = false;
        bomba->activar(x,y); 
        bomba->setPlantada(true);
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

ArmaDeFuego* Jugador::comprarArma(enum Compra arma) {
    std::string str_arma = (arma == C_AK47) ? "ak47" : (arma == C_M3) ? "m3" : "awp";
    std::string str_precio = "precio_" + str_arma;
    int precio = Configuracion::get<int>(str_precio);
    ArmaDeFuego* arma_a_soltar = nullptr;
    if(dinero < precio) {
        return arma_a_soltar; // No tiene suficiente dinero
    }
    switch (arma) {
        case C_AK47:
            if(!arma_principal || arma_principal->getNombre() != "AK-47"){
                arma_a_soltar = soltar_arma_pricipal();
                arma_principal = std::make_unique<Ak47>();
                dinero -= precio;
                arma_en_mano = arma_principal.get();
                acaba_de_comprar_arma = true; 
            } 
            break;
        case C_M3:
            if(!arma_principal || arma_principal->getNombre() != "M3"){
                arma_a_soltar = soltar_arma_pricipal();
                arma_principal = std::make_unique<m3>();
                dinero -= precio;
                arma_en_mano = arma_principal.get();
                acaba_de_comprar_arma = true;
            }                
            break;
        case C_AWP:
            if(!arma_principal || arma_principal->getNombre() != "AWP"){
                arma_a_soltar = soltar_arma_pricipal();
                arma_principal = std::make_unique<Awp>();
                dinero -= precio;
                arma_en_mano = arma_principal.get();
                acaba_de_comprar_arma = true;
            }
            break;
        default:
            break;
    }
    return arma_a_soltar;
}

bool Jugador::comprarBalas(enum Compra tipo_bala) {
    std::string str_tipo = (tipo_bala == BALAS_PRIMARIA) ? "principal" : "secundaria";
    std::string str_precio = "precio_municion_" + str_tipo;
    int precio = Configuracion::get<int>(str_precio);
    switch (tipo_bala) {
        case BALAS_PRIMARIA:
            if (dinero >= precio && arma_principal && arma_principal->getMunicion() < arma_principal->getMunicionMax()) {
                arma_principal->agregarMunicion(10);
                if (arma_principal->getMunicion() > arma_principal->getMunicionMax()) {
                    arma_principal->setMax();
                }
                dinero -= precio;
                acaba_de_comprar_balas = true;
                return true;
            }
            break;
        case BALAS_SECUNDARIA:
            if (dinero >= precio && arma_secundaria->getMunicion() < arma_secundaria->getMunicionMax()){
                arma_secundaria->agregarMunicion(10);
                if (arma_secundaria->getMunicion() > arma_secundaria->getMunicionMax()) {
                    arma_secundaria->setMax();
                }
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

void Jugador::quitar_bomba() {
    if (tiene_bomba) {
        bomba.reset();
        tiene_bomba = false;
        plantando_bomba = false;
        puede_plantar_bomba = false;
        desactivando_bomba = false;
        if (arma_principal) 
            arma_en_mano = arma_principal.get(); 
        else 
            arma_en_mano = arma_secundaria.get(); 
    }
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


Bomba* Jugador::soltar_bomba() {
    if (tiene_bomba) {
        arma_en_mano = cuchillo.get(); 
        Bomba* bomba_suelta = bomba.release();
        tiene_bomba = false; 
        return bomba_suelta;
    }
    return nullptr; 
}

Bomba* Jugador::levantar_bomba(Arma* bomba_del_suelo) {
    if (!tiene_bomba ) {
        Bomba* ver_bomba = dynamic_cast<Bomba*>(bomba_del_suelo);
        if (ver_bomba->estaPlantada()){return nullptr;} 
        bomba.reset(dynamic_cast<Bomba*>(bomba_del_suelo));
        tiene_bomba = true;
        plantando_bomba = false;
        arma_en_mano= bomba.get(); 
        return bomba.get();
    }
    return nullptr; // Ya tiene una bomba
}

ArmaDeFuego* Jugador::levantar_arma(Arma* arma_del_suelo) {
    ArmaDeFuego* arma_soltar = nullptr;
    if (arma_principal) 
        arma_soltar = arma_principal.release(); 
    arma_principal.reset(dynamic_cast<ArmaDeFuego*>(arma_del_suelo));
    arma_en_mano = arma_principal.get();
    acaba_de_comprar_arma = true; // Uso el mismo booleano para no sobrecargar la lógica
    return arma_soltar; 
}

void Jugador::reiniciar_arma() {
    arma_principal.reset();
    arma_en_mano = arma_secundaria.get();
}

void Jugador::reiniciar_dinero() {
    dinero = Configuracion::get<int>("dinero_inicial");
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

Jugador::~Jugador() {
    // Destructor para liberar recursos si es necesario
    arma_principal.reset();
    arma_secundaria.reset();
    cuchillo.reset();
    bomba.reset();
}