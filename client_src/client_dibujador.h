#ifndef CLIENT_DIBUJADOR_H
#define CLIENT_DIBUJADOR_H

#include "../common_src/queue.h"
#include "../common_src/snapshot.h"
#include "client_parseador.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2pp/SDL2pp.hh>

#define ANCHO_MIN 960
#define ALTO_MIN 720
#define CANT_PLAYERS 4

using namespace SDL2pp; 

class Dibujador{
private:
    const int client_id;
    Renderer& renderer;
    ParseadorSpriteSheets parseador;
    Snapshot* snapshot;
    Texture fondo;
    Texture player_legs;
    Texture simbolos_hud;
    Texture numeros_hud;
    Texture sight;
    std::vector<Texture> ct_players;
    std::vector<Texture> tt_players;
    std::vector<SDL_Rect> sprites_player;
    std::vector<SDL_Rect> sprites_player_legs;
    std::vector<SDL_Rect> sprites_simbolos_hud;
    std::vector<SDL_Rect> sprites_numeros_hud;
    std::vector<int> separar_digitos(int n);
    void dibujar_salud();
    void dibujar_saldo();
    void dibujar_tiempo();
    void dibujar_balas_hud();
    void dibujar_jugadores();
    void dibujar_fondo();
    void dibujar_pies(float x, float y, float angulo);
    void dibujar_sight();
    void dibujar_hud();

public:
    Dibujador(const int id, Renderer& renderer);
    void renderizar(Snapshot* snapshot/*, bool& jugador_activo*/);
    Dibujador(const Dibujador&) = delete;
    Dibujador& operator=(const Dibujador&) = delete;
    Dibujador(Dibujador&&) = default;
    Dibujador& operator=(Dibujador&&) = default;
    ~Dibujador();
};

#endif