#include "arma.h"  

class Cuchillo : public Arma {
public:
    // Valores Hardcodeados hasta tener YAML
    Cuchillo() : Arma("Cuchillo", 1.0f, 1.0f, 15, 35, false) {}
    int accion(float distancia) override {
        if (distancia <= alcance) {
            std::random_device rd; std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(min_danio, max_danio);
            return dis(gen);
        }
        return 0;
    }

    
    
};