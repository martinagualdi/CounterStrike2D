#ifndef CLIENT_SONIDO_H
#define CLIENT_SONIDO_H

#include <SDL2pp/SDL.hh>
#include <SDL2pp/Chunk.hh>
#include <SDL2pp/Mixer.hh>
#include <unordered_map>
#include <vector>
#include "../common_src/snapshot.h"

using namespace SDL2pp;

struct FootstepState {
    size_t paso_actual = 0;
    Uint32 ultimo_tick = 0;
};

class Sonido{
private:
    const int client_id;
    Snapshot snapshot;
    Mixer mixer;
    Chunk pickup;
    Chunk compra_balas;
    Chunk bomb_has_been_planted;
    Chunk bomb_has_been_defused;
    Chunk bomb_detonada;
    Chunk bomb_beep;
    Chunk accionando_sobre_bomba;
    std::vector<Chunk> equipo_win;
    std::vector<Chunk> disparo_arma;
    std::vector<Chunk> pasos;
    std::unordered_map<int, FootstepState> estadosPasos;
    enum Equipo equipo_ganador_anterior;
    size_t paso_actual;
    Uint32 ultimo_tick;
    Uint32 last_beep_ticks;
    const Uint32 delay_pasos;

    int volumen_segun_distancia(float x_jugador, float y_jugador, float x_sonido, float y_sonido);
    void reproducirPasos();
    void reproducirDisparos();
    void reproducirArmaNueva();
    void reproducirEquipoGanador();
    void reproducirCompraBalas();
    void reproducirEstadoBomba();
    void reproducirBeep();

public:
    explicit Sonido(int client_id);
    void reproducirSonidos(Snapshot& snapshot);
    Sonido(const Sonido&) = delete;
    Sonido& operator=(const Sonido&) = delete;
    Sonido(Sonido&&) = default;
    Sonido& operator=(Sonido&&) = default;
    ~Sonido();
};

#endif
