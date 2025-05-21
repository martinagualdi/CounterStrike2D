#include "client.h"
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>

#define CS2D_TITLE "Counter Strike 2D"
#define ANCHO_MIN 960
#define ALTO_MIN 720
#define FPS 30

using namespace SDL2pp;

Client::Client(const char *hostname, const char *servname, const char* username) : protocolo(hostname, servname), 
    username(username), cliente_id(-1), clienteActivo(true), cola_enviador(), cola_recibidor(), hilo_enviador(protocolo, cola_enviador),
    hilo_recibidor(protocolo, cola_recibidor), eventHandler(cola_enviador) {}

void Client::iniciar() {
    cliente_id = protocolo.recibirID();
    std::cout << "cliente: " << cliente_id << std::endl;
    hilo_enviador.start();
    hilo_recibidor.start();
    
    SDL sdl(SDL_INIT_VIDEO);
    Window window(CS2D_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ANCHO_MIN, ALTO_MIN, SDL_WINDOW_SHOWN);
    Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);
    Dibujador dibujador(cliente_id, renderer, cola_recibidor);

    int ms_per_frame = 1000 / FPS;
    while(clienteActivo){
        auto t1 = std::chrono::steady_clock::now();
        eventHandler.manejarEventos(clienteActivo);
        dibujador.renderizar();
        auto t2 = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
        if (elapsed < ms_per_frame) {
            std::this_thread::sleep_for(std::chrono::milliseconds(ms_per_frame - elapsed));
        }
    }
    
}

Client::~Client(){
    hilo_enviador.stop();
    hilo_recibidor.stop();
    hilo_enviador.join();
    hilo_recibidor.join();
}
