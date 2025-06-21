#include "arma_de_fuego.h"

class Glock : public ArmaDeFuego {
    
public:
    
    Glock() : ArmaDeFuego(
        "Glock", 
        Configuracion::get<float>("precision_glock"), 
        1250.0f, 
        Configuracion::get<int>("danio_min_glock"), 
        Configuracion::get<int>("danio_max_glock"), 
        false,
        Configuracion::get<int>("balas_glock"),
        Configuracion::get<int>("balas_max_glock"),
        150) {}
    
    int accion(float distancia) override;

    bool puedeAccionar()  override;
    
    enum ArmaEnMano getCodigoArma() const override {
        return GLOCK; 
    }
    
};