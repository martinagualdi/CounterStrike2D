#include "armaDeFuego.h"

class Glock : public ArmaDeFuego {
    
public:
    // Valores Hardcodeados hasta tener YAML
    Glock() : ArmaDeFuego("Glock", 0.8f, 20.0f, 10, 40, false,50) {}
    int accion(float distancia) override {
        if (getMunicion() == 0) 
            return 0;
        municion_actual--;
        distancia += 0.5f; // Aumentamos la distancia para que el daño disminuya con la distancia
        return 10;
    }

   
    
};