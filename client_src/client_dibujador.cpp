#include "client_dibujador.h"
#include <thread>
#include <iostream> 

Dibujador::Dibujador(const int id, Renderer& renderer) : 
    client_id(id),
    renderer(renderer),
    fondo(Texture(renderer, Surface(IMG_Load("client_src/gfx/backgrounds/dust.png")))),
    player(Texture(renderer, Surface(IMG_Load("client_src/gfx/player/ct1.bmp")))),
    player_legs(Texture(renderer, Surface(IMG_Load("client_src/gfx/player/legs.bmp")))),
    sight([&renderer]() {
          Surface s(IMG_Load("client_src/gfx/pointer.bmp"));
          s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 255, 0, 255));
          return Texture(renderer, s);
      }())
    {}

void Dibujador::dibujar_pies() {

}

void Dibujador::dibujar_sight() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    Rect sprite(0,0,46,46);
    Rect dst(mouseX-23, mouseY-23, 46, 46);
    renderer.Copy(sight, sprite, dst);
}

void Dibujador::renderizar(Snapshot& snapshot/*, bool &jugador_activo*/)
{
    renderer.Clear();
    renderer.Copy(fondo, NullOpt, Rect(0, 0, ANCHO_MIN, ALTO_MIN));
    for(const Jugador& jugador : snapshot.info_jugadores){
        
        float x_pixel = jugador.getX();
        float y_pixel = ALTO_MIN -jugador.getY();
        SDL_FRect dst {x_pixel-30, y_pixel-30, 60, 60};
        SDL_Rect sprite = {32, 32, 32, 32};
        SDL_FPoint center = {30, 30};

        float angulo_sdl = 360.0f - jugador.getAngulo() + 90;

        if(jugador.getId() == client_id){
            SDL_RenderCopyExF(renderer.Get(), player.Get(), &sprite, &dst, angulo_sdl, &center, SDL_FLIP_NONE);
            dibujar_sight();
        } else{
            SDL_RenderCopyExF(renderer.Get(), player.Get(), &sprite, &dst, angulo_sdl, &center, SDL_FLIP_NONE);
        }
    }
    renderer.Present();
}


Dibujador::~Dibujador()
{
}
