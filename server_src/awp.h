#include "armaDeFuego.h"

class Awp : public ArmaDeFuego {


public:
    // Valores Hardcodeados hasta tener YAML
    Awp() : ArmaDeFuego("AWP", 0.7f, 35.0f, 50, 100, false,25) {}
    int accion(float /*distancia*/) override {
        if (municion_actual <= 0) return 0;
        municion_actual--;
        std::random_device rd; std::mt19937 gen(rd());
        std::uniform_real_distribution<> hit(0.0, 1.0);
        if (hit(gen) <= precision) {
            std::uniform_int_distribution<> dis(min_danio, max_danio);
            return dis(gen);
        }
        return 0;
    }

};