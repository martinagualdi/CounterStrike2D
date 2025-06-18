#include "arma_de_fuego.h"

class Glock : public ArmaDeFuego {
    
public:
    // Valores Hardcodeados hasta tener YAML
    // Glock() : ArmaDeFuego("Glock", 0.9f, 500.0f, 5, 30, false,50,150) {}
    
    Glock() : ArmaDeFuego(
        "Glock", 
        Configuracion::get<float>("precision_glock"), 
        500.0f, 
        Configuracion::get<int>("danio_min_glock"), 
        Configuracion::get<int>("danio_max_glock"), 
        false,
        Configuracion::get<int>("balas_glock"),
        Configuracion::get<int>("balas_max_glock"),
        150) {}
    
    int accion(float distancia) override;

    bool puedeAccionar()  override;
    
    enum ArmaEnMano getCodigoArma() const override {
        return GLOCK; // Asumiendo que la Glock es un arma del jugador
    }
    
};