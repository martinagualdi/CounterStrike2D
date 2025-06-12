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


class GameLoop : public Thread {
  private:
    Queue<ComandoDTO> &queue_comandos;
    ListaQueues &queues_jugadores;
    std::vector<Jugador *> jugadores;
    int cant_max_jugadores;
    int cant_min_ct;
    int cant_max_ct;
    int cant_min_tt;
    int cant_max_tt;
    std::atomic<bool> activo;
    std::vector<Municion> balas_disparadas;
    std::atomic<bool> ultimo_unido_ct;
    Mapa mapa;
    int tiempo_max_ronda;
    int tiempo_max_comprar;
    int ronda_actual;
    int cant_rondas;
    int rondas_por_equipo;
    std::vector<Jugador *> equipo_ct;
    std::vector<Jugador *> equipo_tt;
    int rondas_ganadas_ct;
    int rondas_ganadas_tt;
    std::atomic<bool> bomba_plantada;


    bool jugador_colisiones_con_mapa(float nuevo_x, float nuevo_y);
    bool bala_golpea_jugador(const Municion &bala);
    void ejecutar_movimiento(Jugador *jugador);
    Jugador* findJugador(int id_jugador);
    bool jugar_ronda();
    enum Equipo se_termino_ronda();

  public:
    explicit GameLoop(Queue<ComandoDTO> &queue_comandos, ListaQueues &queues_jugadores, std::string yaml_partida);

    void agregar_jugador_a_partida(const int id);

    std::string mapa_en_estado_inicial() const { return mapa.mapa_en_estado_inicial(); }

    virtual void run() override;
};

#endif