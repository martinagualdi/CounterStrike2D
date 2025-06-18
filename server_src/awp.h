#include "arma_de_fuego.h"

class Awp : public ArmaDeFuego {


public:
    // Valores Hardcodeados hasta tener YAML
    // Awp() : ArmaDeFuego("AWP", 0.6f, 2500.0f, 100, 100, false,25,1470) {}
    
    Awp() : ArmaDeFuego(
        "AWP", 
        Configuracion::get<float>("precision_awp"), 
        2500.0f, 
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