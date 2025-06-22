#include "client_sonido.h"
#include "../common_src/ruta_base.h"

#include <SDL2/SDL.h>
#include <iostream>

#define FRECUENCIA 44100
#define CANALES 2
#define TAM_BUFFER 2048
#define CANAL_BOMBA 29
#define DISTANCIA_MAX 900.0f
#define DISTANCIA_MIN 70.0f
#define CANTIDAD_SONIDOS_A_LA_VEZ 4
#define CANTIDAD_PASOS_A_LA_VEZ 6
#define CANAL_LIBRE -1

Sonido::Sonido(const int client_id) : 
    client_id(client_id),
    snapshot(),
    mixer(FRECUENCIA, MIX_DEFAULT_FORMAT, CANALES, TAM_BUFFER),
    pickup(RUTA_SONIDO("items/pickup.wav").c_str()),
    compra_balas(RUTA_SONIDO("items/ammo.wav")),
    bomb_has_been_planted(RUTA_SONIDO("radio/bombpl.ogg")),
    bomb_has_been_defused(RUTA_SONIDO("radio/bombdef.ogg")),
    bomb_detonada(RUTA_SONIDO("weapons/c4_explode.wav")),
    bomb_beep(RUTA_SONIDO("weapons/c4.wav")),
    accionando_sobre_bomba(RUTA_SONIDO("weapons/c4_disarm.wav")),
    equipo_win(),
    disparo_arma(),
    pasos(),
    equipo_ganador_anterior(NONE),
    paso_actual(0),
    ultimo_tick(0),
    last_beep_ticks(0),
    delay_pasos(250)
{   
    equipo_win.emplace_back(RUTA_SONIDO("radio/ctwin.ogg").c_str());
    equipo_win.emplace_back(RUTA_SONIDO("radio/terwin.ogg").c_str()); 

    disparo_arma.emplace_back(RUTA_SONIDO("weapons/ak47.wav").c_str());
    disparo_arma.emplace_back(RUTA_SONIDO("weapons/m3.wav").c_str());
    disparo_arma.emplace_back(RUTA_SONIDO("weapons/awp.wav").c_str());
    disparo_arma.emplace_back(RUTA_SONIDO("weapons/knife_slash.wav").c_str());
    disparo_arma.emplace_back(RUTA_SONIDO("weapons/glock18.wav").c_str());

    pasos.emplace_back(RUTA_SONIDO("player/pl_dirt1.wav").c_str());
    pasos.emplace_back(RUTA_SONIDO("player/pl_dirt2.wav").c_str());
    pasos.emplace_back(RUTA_SONIDO("player/pl_dirt3.wav").c_str());
    pasos.emplace_back(RUTA_SONIDO("player/pl_dirt4.wav").c_str());

    mixer.AllocateChannels(32);
}

int Sonido::volumen_segun_distancia(float x_jugador, float y_jugador, float x_sonido, float y_sonido) {
    float dx = x_jugador - x_sonido;
    float dy = y_jugador - y_sonido;
    float distancia = sqrt(dx*dx + dy*dy);

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
            int volumen = volumen_segun_distancia(
                principal->pos_x, principal->pos_y, 
                jugador.pos_x, jugador.pos_y);

            if (volumen < 5) {
                state.paso_actual = (state.paso_actual + 1) % pasos.size();
                state.ultimo_tick = ahora;
                continue;
            }

            Mix_Chunk* chunk = pasos[state.paso_actual].Get();
            int count = 0;
            int channels = Mix_AllocateChannels(-1);
            for (int c = 0; c < channels; ++c) {
                if (Mix_GetChunk(c) == chunk && Mix_Playing(c) && Mix_Volume(c, -1) > 0)
                    count++;
            }

            if (count < CANTIDAD_PASOS_A_LA_VEZ) {
                int canal = mixer.PlayChannel(CANAL_LIBRE, pasos[state.paso_actual], 0);
                if (canal != -1) mixer.SetVolume(canal, volumen);
            }

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
          
            int volumen = volumen_segun_distancia(principal->pos_x, principal->pos_y, jugador.pos_x, jugador.pos_y);
            if (volumen < 5) continue;

            Mix_Chunk* chunk = disparo_arma[i].Get();
            int count = 0;
            int channels = Mix_AllocateChannels(-1);
            for (int c = 0; c < channels; ++c) {
                // SOLO contás los sonidos activos que realmente se están escuchando (volumen no-cero)
                if (Mix_GetChunk(c) == chunk && Mix_Playing(c) && Mix_Volume(c, -1) > 0)
                    count++;
            }
            if (count < CANTIDAD_SONIDOS_A_LA_VEZ) {
                int canal = jugador.id;
                mixer.PlayChannel(canal, disparo_arma[i]);
                mixer.SetVolume(canal, volumen);
            }
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
        } else{
        mixer.PlayChannel(CANAL_LIBRE, equipo_win[snapshot.equipo_ganador]);
        }
        
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
    if (tiempo_restante < 1)        intervalo = 30; 
    else if (tiempo_restante < 2)   intervalo = 110;
    else if (tiempo_restante < 3)   intervalo = 140;
    else if (tiempo_restante < 4)   intervalo = 180;
    else if (tiempo_restante < 5)   intervalo = 220;
    else if (tiempo_restante < 7)   intervalo = 300;
    else if (tiempo_restante < 10)  intervalo = 400;
    else if (tiempo_restante < 15)  intervalo = 600;

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
