#ifndef AWP_H
#define AWP_H
#include "arma_de_fuego.h"

class Awp : public ArmaDeFuego {


public:
    
    Awp() : ArmaDeFuego(
        "AWP", 
        Configuracion::get<float>("precision_awp"), 
        3000.0f, 
        Configuracion::get<int>("danio_min_awp"), 
        Configuracion::get<int>("danio_max_awp"), 
        false,
        Configuracion::get<int>("balas_awp"),
        Configuracion::get<int>("balas_max_awp"),
        1470) {}

    int accion(float distancia) override;

    bool puedeAccionar() override;

    enum ArmaEnMano getCodigoArma() const override { return AWP; }
};

#endif