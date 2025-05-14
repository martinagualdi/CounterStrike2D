#include "client.h"
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>

#define CS2D_TITLE "Counter Strike 2D"
#define ANCHO_MIN 960
#define ALTO_MIN 720
#define FPS 30

using namespace SDL2pp;

Client::Client(const char *hostname, const char *servname, const char* username) : protocolo(hostname, servname), 
    username(username), cola_enviador(), cola_recibidor(), hilo_enviador(protocolo, cola_enviador),
    hilo_recibidor(protocolo, cola_recibidor) {}

void Client::iniciar() {

    hilo_enviador.start();
    hilo_recibidor.start();

    SDL sdl(SDL_INIT_VIDEO);
    Window window(CS2D_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ANCHO_MIN, ALTO_MIN, SDL_WINDOW_SHOWN);
    Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);

    while(true){
        auto t1 = std::chrono::steady_clock::now();
        unsigned long frame_count = 0;
        int ms_per_frame = 1000 / FPS;
       // controlador.manejar_eventos(this->jugador_activo);
       // if (dibujador) {
       //    dibujador->renderizar(renderer, this->jugador_activo);
       // }

        auto t2 = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        if (elapsed < ms_per_frame) {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms_per_frame - elapsed));
        }

        t1 += std::chrono::milliseconds(ms_per_frame);
        frame_count++;
    } 

}

Client::~Client(){
    hilo_enviador.stop();
    hilo_recibidor.stop();
    hilo_enviador.join();
    hilo_recibidor.join();
}
