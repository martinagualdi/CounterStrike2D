#include "armaDeFuego.h"

class AK47 : public ArmaDeFuego {



public:
    // Valores Hardcodeados hasta tener YAML
    AK47(): ArmaDeFuego("AK-47", 0.7f, 20.0f, 15, 80, true,100)
    {}
    int accion(float distancia) override {
        if (municion_actual < 3) return 0;
        municion_actual -= 3;
        int total_danio = 0;
        for (int i = 0; i < 3; ++i) {
            float factor_precision = precision - (distancia / alcance) * 0.5f;
            if (factor_precision < 0.1f) factor_precision = 0.1f;
            std::random_device rd; std::mt19937 gen(rd());
            std::uniform_real_distribution<> hit(0.0, 1.0);
            if (hit(gen) <= factor_precision) {
                std::uniform_int_distribution<> dis(min_danio, max_danio);
                int danio = dis(gen);
                danio -= static_cast<int>(danio * (distancia / alcance) * 0.5f);
                total_danio += (danio > 0 ? danio : 1);
            }
        }
        return total_danio;
    }
};