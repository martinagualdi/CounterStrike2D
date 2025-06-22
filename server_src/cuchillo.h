#ifndef CUCHILLO_H
#define CUCHILLO_H

#include "arma.h"  

class Cuchillo : public Arma {
public:
    

    Cuchillo() : Arma(
        "Cuchillo", 
        Configuracion::get<float>("precision_cuchillo"), 
        65.0f, 
        Configuracion::get<int>("danio_min_cuchillo"), 
        Configuracion::get<int>("danio_max_cuchillo"), 
        false,
        500) {}

    int accion(float distancia) override;

    int getBalas() override { return 1; } 

    bool puedeAccionar()  override;

    enum ArmaEnMano getCodigoArma() const override {
        return CUCHILLO; 
    }
    
};

#endif