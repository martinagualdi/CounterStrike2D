#include "client_sonido.h"
#include "../common_src/ruta_base.h"

#include <SDL2/SDL.h>
#include <iostream>

#define FRECUENCIA 44100
#define CANALES 2
#define TAM_BUFFER 2048

Sonido::Sonido(const int client_id) : 
    client_id(client_id),
    snapshot(),
    mixer(FRECUENCIA, MIX_DEFAULT_FORMAT, CANALES, TAM_BUFFER),
    pickup(RUTA_SONIDO("items/pickup.wav").c_str()),
    compra_balas(RUTA_SONIDO("items/ammo.wav").c_str()),
    equipo_win(),
    disparo_arma(),
    pasos()
{   
    equipo_win.emplace_back(RUTA_SONIDO("radio/ctwin.ogg").c_str());
    equipo_win.emplace_back(RUTA_SONIDO("radio/terwin.ogg").c_str()); 

    disparo_arma.emplace_back(RUTA_SONIDO("weapons/knife_slash.wav").c_str());
    disparo_arma.emplace_back(RUTA_SONIDO("weapons/glock18.wav").c_str());
    disparo_arma.emplace_back(RUTA_SONIDO("weapons/ak47.wav").c_str());
    disparo_arma.emplace_back(RUTA_SONIDO("weapons/m3.wav").c_str());
    disparo_arma.emplace_back(RUTA_SONIDO("weapons/awp.wav").c_str());

    pasos.emplace_back(RUTA_SONIDO("player/pl_dirt1.wav").c_str());
    pasos.emplace_back(RUTA_SONIDO("player/pl_dirt2.wav").c_str());
    pasos.emplace_back(RUTA_SONIDO("player/pl_dirt3.wav").c_str());
    pasos.emplace_back(RUTA_SONIDO("player/pl_dirt4.wav").c_str());

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
