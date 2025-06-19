#include "arma.h"  

class Cuchillo : public Arma {
public:
    // Valores Hardcodeados hasta tener YAML
    //Cuchillo() : Arma("Cuchillo", 1.0f, 1.0f, 15, 35, false,500) {}

    Cuchillo() : Arma(
        "Cuchillo", 
        Configuracion::get<float>("precision_cuchillo"), 
        30.0f, 
        Configuracion::get<int>("danio_min_cuchillo"), 
        Configuracion::get<int>("danio_max_cuchillo"), 
        false,
        500) {}

    int accion(float distancia) override;

    int getBalas() override { return 1; } // El cuchillo no tiene balas, pero se usa para el HUD

    bool puedeAccionar()  override;

    enum ArmaEnMano getCodigoArma() const override {
        return CUCHILLO; // Asumiendo que el cuchillo es un arma del jugador
    }
    
};