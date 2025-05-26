#include "armaDeFuego.h"

class Glock : public ArmaDeFuego {
    
public:
    // Valores Hardcodeados hasta tener YAML
    Glock() : ArmaDeFuego("Glock", 0.8f, 20.0f, 10, 40, false,50) {}
    int accion(float distancia) override {
        if (municion_actual <= 0) return 0;
        municion_actual--;
        float factor_precision = precision - (distancia / alcance) * 0.5f;
        if (factor_precision < 0.1f) factor_precision = 0.1f;
        std::random_device rd; std::mt19937 gen(rd());
        std::uniform_real_distribution<> hit(0.0, 1.0);
        if (hit(gen) <= factor_precision) {
            std::uniform_int_distribution<> dis(min_danio, max_danio);
            int danio = dis(gen);
            // Daño disminuye con la distancia
            danio -= static_cast<int>(danio * (distancia / alcance) * 0.5f);
            return danio > 0 ? danio : 1;
        }
        return 0;
    }

   
    
};