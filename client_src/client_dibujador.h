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
    Texture balas;
    Texture ak47;
    Texture awp;
    Texture bomba;
    Texture cuchillo;
    Texture dropped_bomb;
    Texture glock;
    Texture m3;
    Texture player_legs;
    Texture simbolos_hud;
    Texture numeros_hud;
    Texture sight;
    std::vector<Texture> ct_players;
    std::vector<Texture> tt_players;
    SDL_Rect sprite_arma;
    SDL_Rect sprite_bala;
    std::vector<SDL_Rect> sprites_player;
    std::vector<SDL_Rect> sprites_player_legs;
    std::vector<SDL_Rect> sprites_simbolos_hud;
    std::vector<SDL_Rect> sprites_numeros_hud;
    std::vector<int> separar_digitos(int n);
    float convertir_angulo(float angulo);
    void convertir_coordenadas(float &x, float &y);
    void dibujar_salud(int salud);
    void dibujar_saldo(int saldo);
    void dibujar_balas_hud(int balas);
    void dibujar_tiempo();
    void dibujar_jugadores();
    void dibujar_fondo();
    void dibujar_balas();
    void dibujar_cuerpo(float x, float y, float angulo, enum SkinTipos skin);
    void dibujar_pies(float x, float y, float angulo);
    void dibujar_arma(float x, float y, float angulo);
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