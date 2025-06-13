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
    std::vector<Chunk> equipo_win;
    std::vector<Chunk> disparo_arma;
    std::vector<Chunk> pasos;
    std::unordered_map<int, FootstepState> estadosPasos;
    size_t paso_actual = 0;
    Uint32 ultimo_tick = 0;
    const Uint32 delay_pasos = 250;

    void reproducirPasos();
    void reproducirDisparos();
    void reproducirArmaNueva();
    void reproducirEquipoGanador();
    void reproducirCompraBalas();

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
