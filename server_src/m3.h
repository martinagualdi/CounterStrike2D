#ifndef M3_H
#define M3_H

#include "arma_de_fuego.h"

class m3 : public ArmaDeFuego {

public:
    
    m3() : ArmaDeFuego(
        "M3", 
        Configuracion::get<float>("precision_m3"), 
        180.0f, 
        Configuracion::get<int>("danio_min_m3"), 
        Configuracion::get<int>("danio_max_m3"), 
        false,
        Configuracion::get<int>("balas_m3"),
        Configuracion::get<int>("balas_max_m3"),
        875) {}

    int accion(float distancia) override;

    bool puedeAccionar() override;

    enum ArmaEnMano getCodigoArma() const override {
        return M3; // Asumiendo que el M3 es un arma del jugador
    }
};

#endif