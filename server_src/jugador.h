#ifndef JUGADOR_H
#define JUGADOR_H
#include <memory>
#include "cuchillo.h"
#include "arma.h"
#include "glock.h"
#include "ak47.h"
#include "m3.h"
#include "awp.h"
#include "bomba.h"
#include "armaDeFuego.h"
#include "../common_src/enums_jugador.h"
#include "../common_src/comando_dto.h"


class Jugador {
  private:
    int id;
    float x;
    float y;
    float spawn_x;
    float spawn_y;
    float angulo;
    int vida;
    int dinero;
    enum Equipo equipo_actual;
    enum SkinTipos skin_tipo;
    bool vivo;
    bool moviendose;
    bool disparando;
    bool tiene_bomba;
    bool plantando_bomba;
    bool desactivando_bomba;
    bool puede_comprar;
    bool acaba_de_comprar_arma;
    bool acaba_de_comprar_balas;
    enum Movimiento movimiento_actual = DETENER;
    std::unique_ptr<Bomba> bomba; // Bomba del jugador, si la tiene
    std::unique_ptr<ArmaDeFuego> arma_principal;
    std::unique_ptr<ArmaDeFuego> arma_secundaria;
    std::unique_ptr<Cuchillo> cuchillo;
    Arma* arma_en_mano;
    int eliminaciones_esta_ronda;
    int eliminaciones_totales;

  public:  

    explicit Jugador(int id) : 
      id(id), 
      x(0), 
      y(0), 
      angulo(0), 
      vida(Configuracion::get<int>("vida_inicial")), 
      dinero(Configuracion::get<int>("dinero_inicial")), 
      equipo_actual(), 
      skin_tipo(), 
      vivo(true), 
      moviendose(false), 
      disparando(false), 
      tiene_bomba(false),
      plantando_bomba(false),
      puede_comprar(true),
      acaba_de_comprar_arma(false),
      acaba_de_comprar_balas(false), 
      bomba(nullptr),
      arma_principal(nullptr), 
      arma_secundaria(new Glock()), 
      cuchillo(new Cuchillo()), 
      arma_en_mano(arma_secundaria.get()),
      eliminaciones_esta_ronda(0),
      eliminaciones_totales(0) {}

    // Getters, Setters y estados del jugador
    int getId() const { return id; }
    float getX() const { return x; }
    float getY() const { return y; }
    void setX(float posX) { this->x = posX; }
    void setY(float posY) { this->y = posY; }
    void setAngulo(float angulo){ this->angulo = angulo; }
    float getAngulo() const { return angulo; }
    enum Movimiento getMovimiento() const { return movimiento_actual; }
    void setMovimiento(enum Movimiento m){ this->movimiento_actual = m; }
    int get_vida() const { return vida;}
    int get_dinero() const { return dinero; }
    enum Equipo get_equipo() const { return equipo_actual; }
    void establecer_equipo(enum Equipo equipo) { this->equipo_actual = equipo;  }
    void establecer_skin(enum SkinTipos skin) { this->skin_tipo = skin; }
    enum SkinTipos get_skin_tipo() const { return skin_tipo; }
    void set_skin_tipo(enum SkinTipos skin) { this->skin_tipo = skin; }
    bool esta_vivo() { return vivo; }
    bool esta_moviendose() const { return moviendose == true; }
    bool esta_disparando() const {  return disparando; }
    void dejar_de_disparar() { disparando = false; }
    bool esta_plantando_bomba() const { return plantando_bomba; }
    bool esta_desactivando_bomba() const {return desactivando_bomba;}
    bool puede_disparar() const { return arma_en_mano->puedeAccionar(); }
    void empezar_a_plantar();
    void empezar_a_desactivar();
    void desactivar_bomba();
    void plantar_bomba(float x, float y); 
    void cancelar_plantado_bomba();
    void cancelar_desactivado_bomba();
    bool puede_comprar_ahora() { return puede_comprar; }
    void en_posicion_de_compra(bool puede_o_no) {puede_comprar = puede_o_no; }
    bool compro_arma_ahora() const { return acaba_de_comprar_arma; }
    bool compro_balas_ahora() const { return acaba_de_comprar_balas; }
    void asignar_bomba();
    int get_eliminaciones_esta_ronda() const { return eliminaciones_esta_ronda; }
    int get_eliminaciones_totales() const { return eliminaciones_totales; }

    // Logicas
    void disparar();

    void recibir_danio(int danio);

    std::string get_nombre_arma_en_mano();

    void cambiar_estado_moviendose();

    void cambiar_arma_en_mano();

    bool comprarArma(enum Compra arma);

    bool comprarBalas(enum Compra tipo_bala);

    enum ArmaEnMano get_codigo_arma_en_mano();

    Arma* get_arma_actual() const;

    void sumar_eliminacion();

    void reiniciar_compras();

    void finalizar_ronda();

    ArmaDeFuego* soltar_arma_pricipal();
    Bomba* soltar_bomba();


    ArmaDeFuego* levantar_arma(Arma* arma_del_suelo);
    Bomba* levantar_bomba(Arma* bomba_del_suelo);

    void definir_spawn(float x, float y);

    void reiniciar();
    
    ~Jugador() {}
    
    Jugador(const Jugador &) = default;
    Jugador &operator=(const Jugador &) = default;
    Jugador(Jugador &&) = default;
    Jugador &operator=(Jugador &&) = default;
};

#endif