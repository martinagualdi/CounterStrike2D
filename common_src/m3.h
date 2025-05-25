#include "armaDeFuego.h"


class M3 : public ArmaDeFuego {



public:
    // Valores Hardcodeados hasta tener YAML
    M3() : ArmaDeFuego("M3", 0.7f, 15.0f, 20, 70, false,100) {}
    int accion(float distancia) override {
        if (municion_actual <= 0) return 0;
        municion_actual--;
        float factor_precision = precision - (distancia / alcance) * 0.7f;
        if (factor_precision < 0.1f) factor_precision = 0.1f;
        std::random_device rd; std::mt19937 gen(rd());
        std::uniform_real_distribution<> hit(0.0, 1.0);
        int pellets = 6;
        int total_danio = 0;
        for (int i = 0; i < pellets; ++i) {
            if (hit(gen) <= factor_precision) {
                std::uniform_int_distribution<> dis(min_danio, max_danio);
                int danio = dis(gen) / pellets;
                // Daño disminuye mucho con la distancia
                danio -= static_cast<int>(danio * (distancia / alcance));
                total_danio += (danio > 0 ? danio : 1);
            }
        }
        return total_danio;
    }
};