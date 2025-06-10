#include "client_sonido.h"

#include <SDL2/SDL.h>

#define FRECUENCIA 44100
#define CANALES 2
#define TAM_BUFFER 2048

Sonido::Sonido() : 
    snapshot(),
    mixer(FRECUENCIA, MIX_DEFAULT_FORMAT, CANALES, TAM_BUFFER),
    disparo_arma(),
    pasos()
    {      
        disparo_arma.emplace_back("client_src/sfx/weapons/knife_slash.wav");
        disparo_arma.emplace_back("client_src/sfx/weapons/glock18.wav");
        disparo_arma.emplace_back("client_src/sfx/weapons/ak47.wav");
        disparo_arma.emplace_back("client_src/sfx/weapons/m3.wav");
        disparo_arma.emplace_back("client_src/sfx/weapons/awp.wav");
        pasos.emplace_back("client_src/sfx/player/pl_dirt1.wav");
        pasos.emplace_back("client_src/sfx/player/pl_dirt2.wav");
        pasos.emplace_back("client_src/sfx/player/pl_dirt3.wav");
        pasos.emplace_back("client_src/sfx/player/pl_dirt4.wav");
        mixer.AllocateChannels(32);
    }


void Sonido::reproducirPasos() {
    Uint32 ahora = SDL_GetTicks();

    for (const InfoJugador &jugador : snapshot.info_jugadores) {
        FootstepState &state = estadosPasos[jugador.id];
        if (!jugador.esta_moviendose) {
            state.paso_actual = 0;
            state.ultimo_tick = ahora;
            continue;
        }

        if (ahora - state.ultimo_tick >= delay_pasos) {
            mixer.PlayChannel(-1, pasos[state.paso_actual], 0);
            state.paso_actual = (state.paso_actual + 1) % pasos.size();
            state.ultimo_tick = ahora;
        }
    }
}
void Sonido::reproducirDisparos() {

    for(const InfoJugador& jugador : snapshot.info_jugadores){  
        if(!jugador.esta_disparando) continue;
        mixer.PlayChannel(-1, disparo_arma[jugador.arma_en_mano]);
    }

}

void Sonido::reproducirSonidos(Snapshot& snapshot){

    this->snapshot = snapshot;
    reproducirPasos();
    reproducirDisparos();
        

}

Sonido::~Sonido() {}
