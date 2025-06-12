#include "armaDeFuego.h"


class m3 : public ArmaDeFuego {



public:
    // Valores Hardcodeados hasta tener YAML
    // m3() : ArmaDeFuego("M3", 0.7f, 180.0f, 5, 80, false,100,875) {}
    
    m3() : ArmaDeFuego(
        "M3", 
        Configuracion::get<float>("precision_m3"), 
        180.0f, 
        Configuracion::get<int>("danio_min_m3"), 
        Configuracion::get<int>("danio_max_m3"), 
        false,
        Configuracion::get<int>("balas_m3"),
        Configuracion::get<int>("balas_max_m3"),
        875) {}

    int accion(float distancia) override {
        if (municion_actual <= 0) return 0;
        if (distancia > alcance) return 0;
        std::random_device rd; std::mt19937 gen(rd());
        std::uniform_real_distribution<> hit(0.0, 1.0);
        if (hit(gen) <= precision) {
            int danio = static_cast<int>(max_danio * (alcance / (distancia * distancia)) * 0.5f);
            if (danio < min_danio) {
                danio = min_danio; 
            }
            return danio > max_danio ? max_danio : danio;
        }
        return min_danio;
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
};