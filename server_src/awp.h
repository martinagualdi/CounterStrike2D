#include "armaDeFuego.h"

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

    int accion(float distancia) override {
        if (municion_actual <= 0) return 0;
        if (distancia > alcance) return 0; 
        std::random_device rd; std::mt19937 gen(rd());
        std::uniform_real_distribution<> hit(0.0, 1.0);
        if (hit(gen) <= precision) {
            return max_danio;
        }
        return 0;
    }

    bool puedeAccionar()  override {
        auto ahora = std::chrono::steady_clock::now();
        auto tiempo_transcurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - ultima_accion);
        if (tiempo_transcurrido.count() >= cadencia_accion_ms && municion_actual > 0){
            municion_actual--;
            ultima_accion = std::chrono::steady_clock::now();
            return true;
        }
        return false;
    }

    enum ArmaEnMano getCodigoArma() const override {
        return AWP;
    }
};