#ifndef M3_H
#define M3_H

#include "arma_de_fuego.h"

class m3 : public ArmaDeFuego {

private:
    static constexpr float D = 100.0f; // Factor de distancia para el cálculo de daño

public:
    
    m3() : ArmaDeFuego(
        "M3", 
        Configuracion::get<float>("precision_m3"), 
        750.0f, 
        Configuracion::get<int>("danio_min_m3"), 
        Configuracion::get<int>("danio_max_m3"), 
        false,
        Configuracion::get<int>("balas_m3"),
        Configuracion::get<int>("balas_max_m3"),
        875) {}

    int accion(float distancia) override;

    bool puedeAccionar() override;

    enum ArmaEnMano getCodigoArma() const override {
        return M3;
    }
};

#endif