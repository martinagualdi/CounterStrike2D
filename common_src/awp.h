#include "arma.h"

class AWP : public Arma {
public:
    AWP() : Arma() {}
    int disparar(float /*distancia*/) override {
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
private:
    std::string nombre = "AWP";
    int municion_max = 50;
    int municion_actual = 0;
    float precision = 0.7f; 
    float alcance = 30.0f;  
    int min_danio = 50;
    int max_danio = 100;
    bool es_automatica = false;
};