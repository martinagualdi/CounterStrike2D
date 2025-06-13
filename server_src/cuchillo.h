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

    int accion(float distancia) override {
        if (distancia <= alcance) {
            std::random_device rd; std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(min_danio, max_danio);
            return dis(gen);
        }
        return 0;
    }
    int getBalas() override { return 1; } // El cuchillo no tiene balas, pero se usa para el HUD
    bool puedeAccionar()  override {
        auto ahora = std::chrono::steady_clock::now();
        auto tiempo_transcurrido = std::chrono::duration_cast<std::chrono::milliseconds>(ahora - ultima_accion);
        if (tiempo_transcurrido.count() >= cadencia_accion_ms ){
            ultima_accion = std::chrono::steady_clock::now();
            return true;
        }
        return false;
    }

    enum ArmaEnMano getCodigoArma() const override {
        return CUCHILLO; // Asumiendo que el cuchillo es un arma del jugador
    }
    
};