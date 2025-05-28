#ifndef CLIENT_DIBUJADOR_H
#define CLIENT_DIBUJADOR_H

#include "../common_src/queue.h"
#include "../common_src/snapshot.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2pp/SDL2pp.hh>

#define ANCHO_MIN 960
#define ALTO_MIN 720

using namespace SDL2pp; 

class Dibujador{
private:
    const int client_id;
    Renderer& renderer;
    Texture fondo;
    Texture player;
    Texture player_legs;
    Texture sight;
    void dibujar_pies();
    void dibujar_sight();

public:
    Dibujador(const int id, Renderer& renderer);
    void renderizar(Snapshot& snapshot/*, bool& jugador_activo*/);
    Dibujador(const Dibujador&) = delete;
    Dibujador& operator=(const Dibujador&) = delete;
    Dibujador(Dibujador&&) = default;
    Dibujador& operator=(Dibujador&&) = default;
    ~Dibujador();
};

#endif