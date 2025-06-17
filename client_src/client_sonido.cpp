#include "client_sonido.h"

#include <SDL2/SDL.h>
#include <iostream>

#define FRECUENCIA 44100
#define CANALES 2
#define TAM_BUFFER 2048
#define CANAL_BOMBA 29
#define CANAL_LIBRE -1

Sonido::Sonido(const int client_id) : 
    client_id(client_id),
    snapshot(),
    mixer(FRECUENCIA, MIX_DEFAULT_FORMAT, CANALES, TAM_BUFFER),
    pickup("client_src/sfx/items/pickup.wav"),
    compra_balas("client_src/sfx/items/ammo.wav"),
    bomb_has_been_planted("client_src/sfx/radio/bombpl.ogg"),
    bomb_has_been_defused("client_src/sfx/radio/bombdef.ogg"),
    bomb_detonada("client_src/sfx/weapons/c4_explode.wav"),
    bomb_beep("client_src/sfx/weapons/c4.wav"),
    accionando_sobre_bomba("client_src/sfx/weapons/c4_disarm.wav"),
    equipo_win(),
    disparo_arma(),
    pasos(),
    equipo_ganador_anterior(NONE),
    paso_actual(0),
    ultimo_tick(0),
    last_beep_ticks(0),
    delay_pasos(250)
    {   
        equipo_win.emplace_back("client_src/sfx/radio/ctwin.ogg");
        equipo_win.emplace_back("client_src/sfx/radio/terwin.ogg"); 
        disparo_arma.emplace_back("client_src/sfx/weapons/ak47.wav");
        disparo_arma.emplace_back("client_src/sfx/weapons/m3.wav");
        disparo_arma.emplace_back("client_src/sfx/weapons/awp.wav");
        disparo_arma.emplace_back("client_src/sfx/weapons/knife_slash.wav");
        disparo_arma.emplace_back("client_src/sfx/weapons/glock18.wav");
        pasos.emplace_back("client_src/sfx/player/pl_dirt1.wav");
        pasos.emplace_back("client_src/sfx/player/pl_dirt2.wav");
        pasos.emplace_back("client_src/sfx/player/pl_dirt3.wav");
        pasos.emplace_back("client_src/sfx/player/pl_dirt4.wav");
        mixer.AllocateChannels(32);
    }

int Sonido::volumen_segun_distancia(float x_jugador, float y_jugador, float x_sonido, float y_sonido) {
    float dx = x_jugador - x_sonido;
    float dy = y_jugador - y_sonido;
    float distancia = sqrt(dx*dx + dy*dy);

    const float DISTANCIA_MAX = 900.0f;  // hasta donde se escucha
    const float DISTANCIA_MIN = 70.0f;   // desde donde suena al máximo

    if (distancia <= DISTANCIA_MIN) return MIX_MAX_VOLUME;
    if (distancia >= DISTANCIA_MAX) return 0;
    float factor = 1.0f - (distancia - DISTANCIA_MIN) / (DISTANCIA_MAX - DISTANCIA_MIN);
    return static_cast<int>(MIX_MAX_VOLUME * factor);
}


void Sonido::reproducirPasos() {
    const InfoJugador* principal = snapshot.getJugadorPorId(client_id);
    
    if(!principal) return;

    Uint32 ahora = SDL_GetTicks();

    for (const InfoJugador &jugador : snapshot.info_jugadores) {
        FootstepState &state = estadosPasos[jugador.id];
        if (!jugador.esta_moviendose) {
            state.paso_actual = 0;
            state.ultimo_tick = ahora;
            continue;
        }

        if (ahora - state.ultimo_tick >= delay_pasos) {
            int canal = mixer.PlayChannel(CANAL_LIBRE, pasos[state.paso_actual], 0);
            int volumen = volumen_segun_distancia(principal->pos_x, principal->pos_y, jugador.pos_x, jugador.pos_y);
            mixer.SetVolume(canal, volumen);
            state.paso_actual = (state.paso_actual + 1) % pasos.size();
            state.ultimo_tick = ahora;
        }
    }
}

void Sonido::reproducirDisparos() {

    const InfoJugador* principal = snapshot.getJugadorPorId(client_id);
    
    if(!principal) return;

    for(const InfoJugador& jugador : snapshot.info_jugadores){  
        if(jugador.esta_disparando) {
            int i = static_cast<int>(jugador.arma_en_mano);
            if(jugador.arma_en_mano > BOMBA_TT) i--;
            int canal = jugador.id;
            mixer.PlayChannel(canal, disparo_arma[i]);
            int volumen = volumen_segun_distancia(principal->pos_x, principal->pos_y, jugador.pos_x, jugador.pos_y);
            mixer.SetVolume(canal, volumen);
        }
    }

}

void Sonido::reproducirArmaNueva() {

    const InfoJugador* principal = snapshot.getJugadorPorId(client_id);    
    if(!principal) return;

    for(const InfoJugador& jugador : snapshot.info_jugadores){
        if(jugador.acaba_de_comprar_arma) {
            int canal = mixer.PlayChannel(CANAL_LIBRE, pickup);
            int volumen = volumen_segun_distancia(principal->pos_x, principal->pos_y, jugador.pos_x, jugador.pos_y);
            mixer.SetVolume(canal, volumen);
        }

    }

}

void Sonido::reproducirEquipoGanador() {
    
    if(snapshot.equipo_ganador != NONE && equipo_ganador_anterior == NONE){
        if(snapshot.bomba_en_suelo.estado_bomba == DESACTIVADA) {
            mixer.PlayChannel(CANAL_LIBRE, bomb_has_been_defused);
        } 
        mixer.PlayChannel(CANAL_LIBRE, equipo_win[snapshot.equipo_ganador]);
        
    }   
    equipo_ganador_anterior = snapshot.equipo_ganador;
}

void Sonido::reproducirCompraBalas() {

    const InfoJugador* j = snapshot.getJugadorPorId(client_id);
    if(!j) return;

    if(j->acaba_de_comprar_balas){
        mixer.PlayChannel(CANAL_LIBRE, compra_balas);
    }

}

void Sonido::reproducirEstadoBomba() {

    const InfoJugador* j = snapshot.getJugadorPorId(client_id);
    if(!j) return;

    enum EstadoBombaRonda estado = snapshot.bomba_en_suelo.estado_bomba;

    if(estado == SIN_PLANTAR) return;

    if(estado == PLANTADA) reproducirBeep();

    if(snapshot.bomba_en_suelo.acaba_de_detonar){
        mixer.PlayChannel(CANAL_LIBRE, bomb_detonada);
    }

    if(snapshot.bomba_en_suelo.acaba_de_ser_plantada){
        mixer.PlayChannel(CANAL_LIBRE, bomb_has_been_planted);
    }

}

void Sonido::reproducirBeep() {

    const InfoJugador* principal = snapshot.getJugadorPorId(client_id);
    
    if(!principal) return;    

    Uint32 ahora = SDL_GetTicks();
    int tiempo_restante = snapshot.bomba_en_suelo.tiempo_para_detonar;
    int intervalo = 1000; 
    if (tiempo_restante < 3) intervalo = 150;
    else if (tiempo_restante < 7) intervalo = 350;
    else if (tiempo_restante < 15) intervalo = 500;

    if (ahora - last_beep_ticks >= (Uint32)intervalo) {
        int canal = mixer.PlayChannel(CANAL_BOMBA, bomb_beep);
        int volumen = volumen_segun_distancia(principal->pos_x, principal->pos_y, 
        snapshot.bomba_en_suelo.pos_x, snapshot.bomba_en_suelo.pos_y);
        mixer.SetVolume(canal, volumen);
        last_beep_ticks = ahora;
    }

}

void Sonido::reproducirSonidos(Snapshot& snapshot){

    this->snapshot = snapshot;
    reproducirPasos();
    reproducirDisparos();
    reproducirArmaNueva();
    reproducirCompraBalas();
    reproducirEquipoGanador();
    reproducirEstadoBomba();

}

Sonido::~Sonido() {}
