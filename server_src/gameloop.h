#ifndef GAMELOOP_H
#define GAMELOOP_H

#include "../common_src/comando_dto.h"
#include "../common_src/thread.h"
#include "../common_src/queue.h"
#include "../common_src/snapshot.h"
#include "jugador.h"
#include "lista_queues.h"
#include "municion.h"
#include "mapa.h"
#include "bomba.h"

class GameLoop : public Thread {
  private:
    Queue<ComandoDTO> &queue_comandos;
    ListaQueues &queues_jugadores;
    std::vector<Jugador *> jugadores;
    int cant_min_ct;
    int cant_min_tt;
    std::atomic<bool> activo;
    std::vector<Municion> balas_disparadas;
    std::atomic<bool> ultimo_unido_ct;
    Mapa mapa;
    int tiempo_max_ronda;
    int tiempo_max_comprar;
    std::atomic<bool> algun_jugador_puede_comprar;
    int ronda_actual;
    int cant_rondas;
    int rondas_por_equipo;
    std::vector<Jugador *> equipo_ct;
    std::vector<Jugador *> equipo_tt;
    int rondas_ganadas_ct;
    int rondas_ganadas_tt;
    std::atomic<bool> bomba_plantada;
    std::vector<ArmaEnSuelo> armas_en_suelo;
    BombaEnSuelo info_bomba;
    Bomba* bomba = nullptr;
    std::chrono::steady_clock::time_point tiempo_inicio_plantado;
    std::chrono::steady_clock::time_point tiempo_inicio_desactivado;
    Jugador* jugador_plantando = nullptr;
    Jugador* jugador_desactivando = nullptr;


    void volver_jugadores_a_spawn();
    void cargar_dinero_por_eliminaciones();
    bool jugador_colisiones_con_mapa(float nuevo_x, float nuevo_y);
    bool bala_golpea_jugador(const Municion &bala, bool esperando);
    void ejecutar_movimiento(Jugador *jugador);
    Jugador* findJugador(int id_jugador);
    bool jugar_ronda(bool esperando);
    enum Equipo se_termino_ronda(auto& t_inicio);
    bool esperando_jugadores();
    void chequear_estados_jugadores();
    void chequear_si_pueden_comprar(auto t_inicio);
    void chequear_bomba_plantada();
    void chequear_bomba_desactivada();
    void reiniciar_estado_bomba();
    void ejecucion_comandos_recibidos();
    void disparar_rafagas_restantes();
    void chequear_colisiones(bool esperando);
    void chequear_si_equipo_gano(enum Equipo& eq_ganador, bool& en_juego, auto& t_inicio);
    void chequear_si_completaron_equipos(bool& en_juego);
    void explosion();
    void realizar_cambio_equipo_si_es_necesario();
    void esperar_entre_rondas(int segundos, int t_restante, enum Equipo eq_ganador);
    void asignar_bomba_si_es_necesario(bool esperando);
    void colocar_armas_del_mapa();
    bool chequear_si_termino_partida();
    void reiniciar_armas_jugadores();
    void reiniciar_dinero_jugadores();

  public:
    explicit GameLoop(Queue<ComandoDTO> &queue_comandos, ListaQueues &queues_jugadores, bool activa, std::string yaml_partida);

    void agregar_jugador_a_partida(const int id, std::string& nombre);

    std::string mapa_en_estado_inicial() const { return mapa.mapa_en_estado_inicial(); }

    virtual void run() override;

    void eliminar_jugador_de_partida(int id_jugador);

    ~GameLoop() override;
};

#endif