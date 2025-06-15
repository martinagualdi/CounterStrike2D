#include "client_sonido.h"

#include <SDL2/SDL.h>
#include <iostream>

#define FRECUENCIA 44100
#define CANALES 2
#define TAM_BUFFER 2048

Sonido::Sonido(const int client_id) : 
    client_id(client_id),
    snapshot(),
    mixer(FRECUENCIA, MIX_DEFAULT_FORMAT, CANALES, TAM_BUFFER),
    pickup("/var/CounterStrike2D/assets/sfx/items/pickup.wav"),
    compra_balas("/var/CounterStrike2D/assets/sfx/items/ammo.wav"),
    equipo_win(),
    disparo_arma(),
    pasos()
    {   
        equipo_win.emplace_back("/var/CounterStrike2D/assets/sfx/radio/ctwin.ogg");
        equipo_win.emplace_back("/var/CounterStrike2D/assets/sfx/radio/terwin.ogg"); 
        disparo_arma.emplace_back("/var/CounterStrike2D/assets/sfx/weapons/knife_slash.wav");
        disparo_arma.emplace_back("/var/CounterStrike2D/assets/sfx/weapons/glock18.wav");
        disparo_arma.emplace_back("/var/CounterStrike2D/assets/sfx/weapons/ak47.wav");
        disparo_arma.emplace_back("/var/CounterStrike2D/assets/sfx/weapons/m3.wav");
        disparo_arma.emplace_back("/var/CounterStrike2D/assets/sfx/weapons/awp.wav");
        pasos.emplace_back("/var/CounterStrike2D/assets/sfx/player/pl_dirt1.wav");
        pasos.emplace_back("/var/CounterStrike2D/assets/sfx/player/pl_dirt2.wav");
        pasos.emplace_back("/var/CounterStrike2D/assets/sfx/player/pl_dirt3.wav");
        pasos.emplace_back("/var/CounterStrike2D/assets/sfx/player/pl_dirt4.wav");
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
        if(jugador.esta_disparando)
            mixer.PlayChannel(-1, disparo_arma[jugador.arma_en_mano]);
    }

}

void Sonido::reproducirArmaNueva() {

    for(const InfoJugador& jugador : snapshot.info_jugadores){
        if(jugador.acaba_de_comprar_arma) 
            mixer.PlayChannel(-1, pickup);
    }

}

void Sonido::reproducirEquipoGanador() {
    
    if(snapshot.equipo_ganador != NONE){
        mixer.PlayChannel(-1, equipo_win[snapshot.equipo_ganador]);
    }

}

void Sonido::reproducirCompraBalas() {

    const InfoJugador* j = snapshot.getJugadorPorId(client_id);
    if(!j) return;

    if(j->acaba_de_comprar_balas){
        mixer.PlayChannel(-1, compra_balas);
    }

}

void Sonido::reproducirSonidos(Snapshot& snapshot){

    this->snapshot = snapshot;
    reproducirPasos();
    reproducirDisparos();
    reproducirArmaNueva();
    reproducirCompraBalas();
    //reproducirEquipoGanador();

}

Sonido::~Sonido() {}
