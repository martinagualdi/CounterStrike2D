#ifndef JUGADOR_H
#define JUGADOR_H
#include <memory>
#include "comando_dto.h"
#include "cuchillo.h"
#include "arma.h"
#include "glock.h"
enum Equipo {
    CT,
    TT
};

enum SkinTipos {
    /*Del 0 al 3 son CT, los otros TT*/
    SEAL_FORCE,
    GERMAN_GSG_9,
    UK_SAS,
    FRENCH_GIGN,
    PHEONIX, 
    L337_KREW, 
    ARTIC_AVENGER, 
    GUERRILLA
};

class Jugador {
  private:
    int id;
    float x;
    float y;
    float angulo;
    int vida;
    int dinero;
    enum Equipo equipo_actual;
    enum SkinTipos skin_tipo;
    bool vivo = true;
    bool moviendose = false;
    enum Movimiento movimiento_actual = DETENER;
    std::shared_ptr<Glock> arma_secundaria;




    //std::unique_ptr<Cuchillo> cuchillo;
    //Arma* arma_actual;
    //std::unique_ptr<ArmaDeFuego> arma_secundaria;
    //std::unique_ptr<ArmaDeFuego> arma_primaria;

  public:   /* AGREGAR LOGICA DE SEPARAR EN EQUIPOS */
    explicit Jugador(int id) : id(id), x(10), y(10), angulo(0), vida(100), dinero(500), equipo_actual(CT), skin_tipo(SEAL_FORCE), vivo(true), arma_secundaria(new Glock()) {};

    //Jugador(int id, float x, float y, float angulo) : id(id), x(x), y(y), angulo(angulo),movimiento_actual(DETENER), vida(100), dinero(500),vivo(true), arma_secundaria(new Glock()) {};

    Jugador(int id, float x, float y, float angulo, enum Equipo equipo, enum SkinTipos skin, int vida, int dinero, uint8_t arma_secundaria_id)
        : id(id), x(x), y(y), angulo(angulo), vida(vida), dinero(dinero), equipo_actual(equipo), skin_tipo(skin), vivo(true), 
        arma_secundaria(arma_secundaria_id == 0x01 ? new Glock() : nullptr) {};

    /*explicit Jugador(int id) : id(id), x(10), y(10), angulo(0), movimiento_actual(DETENER), vida(100), dinero(500),vivo(true)
    ,cuchillo(std::make_unique<Cuchillo>()), arma_actual(cuchillo.get()), arma_secundaria(std::make_unique<Glock>()), arma_primaria(nullptr){};

    Jugador(int id, float x, float y, float angulo) : id(id), x(x), y(y), angulo(angulo),movimiento_actual(DETENER), vida(100), dinero(500),vivo(true)
    ,cuchillo(std::make_unique<Cuchillo>()), arma_actual(cuchillo.get()), arma_secundaria(std::make_unique<Glock>()), arma_primaria(nullptr){};*/


    int getId() const {
        return id;
    }

    float getX() const {
        return x;
    }
    float getY() const {
        return y;
    }

    void setX(float posX) {
        this->x = posX;
    }
    void setY(float posY) {
        this->y = posY;
    }

    void setAngulo(float angulo){
        this->angulo = angulo;
    }

    float getAngulo() const {
        return angulo;
    }
    
    enum Movimiento getMovimiento() const {
        return movimiento_actual;
    }
    
    void setMovimiento(enum Movimiento m){
        this->movimiento_actual = m;
    }

    int get_vida() const {
        return vida;
    }

    int get_dinero() const {
        return dinero;
    }

    bool disparar() {
        /* HARCODEADO PAEA PROBAR DISPARO */
        if (arma_secundaria->accion(0.0f) > 0)
            return true;
        return false;
    }

    void recibir_danio(int danio) { 
        vida -= danio; 
        if (vida <= 0) {
            vida = 0;
            vivo = false;
        }
    }

    std::string get_nombre_arma_secundaria() const {
        return arma_secundaria->getNombre();
    }

    enum Equipo get_equipo() const {
        return equipo_actual;
    }

    void set_equipo(enum Equipo equipo) {
        this->equipo_actual = equipo;
    }

    enum SkinTipos get_skin_tipo() const {
        return skin_tipo;
    }

    void set_skin_tipo(enum SkinTipos skin) {
        this->skin_tipo = skin;
    }

    bool esta_vivo() const {
        return vivo;
    }

    bool esta_moviendose() const { return moviendose == true; }

    void cambiar_estado_moviendose() { moviendose = !moviendose; }

    /*
    void setArmaPrimaria(std::unique_ptr<ArmaDeFuego> nuevaArma) {
        arma_primaria = std::move(nuevaArma);
    }

    void cambiarASecundaria() {
        arma_actual = arma_secundaria.get();
    }
    void cambiarAPrimaria() {
        arma_actual = arma_primaria.get();
    }
    void cambiarACuchillo() {
        arma_actual = cuchillo.get();
    }
    ArmaDeFuego* getArmaPrimaria() const {
        return arma_primaria.get();
    }
    ArmaDeFuego* getArmaSecundaria() const {
        return arma_secundaria.get();
    }

    int getDinero() const {
        return dinero;
    }
    void recibirRecompensa(int recompensa) {
        this->dinero = dinero+recompensa;
    }
    int getMunicionSecundaria() const {
        return arma_secundaria->getMunicion();
    }
    void recibirMunicionSecundaria(int cantidad) {
        this->arma_secundaria->setMunicion(cantidad);
    }
    int getMunicionPrimaria() const {
        return arma_primaria->getMunicion();
    }
    void recibirMunicionPrimaria(int cantidad) {
        this->arma_primaria->setMunicion(cantidad);
    }

    void recibirDanio(int danio) {
        if (danio < 0) return; // No se puede recibir daño negativo
        vida -= danio;
        if (vida <= 0) {
            vida = 0;
            vivo = false;
        }
    }
    bool estaVivo() const {
        return vivo;
    }
    void revivir() {
        vida = 100;
        vivo = true;
    }
    void setEquipo(enum Equipo equipo) {
        this->equipo_actual = equipo;
    }
    enum Equipo getEquipo() const {
        return equipo_actual;
    }
    
    int disparar(float distancia) {
        return arma_actual->accion(distancia);
    }

    */
    
    ~Jugador() {
       
    }

    Jugador(const Jugador &) = default;
    Jugador &operator=(const Jugador &) = default;
    Jugador(Jugador &&) = default;
    Jugador &operator=(Jugador &&) = default;
};

#endif