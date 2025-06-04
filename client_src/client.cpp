#include "client.h"
#include "client_map.h"
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include <QApplication>

#include "lobby_window.h"

#define CS2D_TITLE "Counter Strike 2D"
#define ANCHO_MIN 960
#define ALTO_MIN 720
#define FPS 30

using namespace SDL2pp;

Client::Client(const char *hostname, const char *servname, const char* username) : protocolo(hostname, servname), 
    username(username), cliente_id(-1), clienteActivo(true), cola_enviador(), cola_recibidor(), hilo_enviador(protocolo, cola_enviador),
    hilo_recibidor(protocolo, cola_recibidor) {}

void Client::iniciar() {
    cliente_id = protocolo.recibirID();

    LobbyWindow lobby(protocolo, username);
    int result = lobby.exec();
    if (result == QDialog::Accepted) {
        hilo_enviador.start();
        hilo_recibidor.start();
        
        SDL sdl(SDL_INIT_VIDEO);
        SDL_ShowCursor(SDL_DISABLE);
        Window window(CS2D_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ANCHO_MIN, ALTO_MIN, SDL_WINDOW_SHOWN);
        Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);
        ClientMap mapa("m2.yaml", renderer);
        Dibujador dibujador(cliente_id, renderer, mapa.parsearMapa());
        EventHandler eventHandler(cola_enviador, cliente_id);

        int ms_per_frame = 1000 / FPS;
        while(clienteActivo){
            auto t1 = std::chrono::steady_clock::now();

            Snapshot snapshotActual, ultimo_snapshot_recibido;
            while (cola_recibidor.try_pop(snapshotActual)) {
                ultimo_snapshot_recibido = snapshotActual;
            }
            
            eventHandler.manejarEventos(clienteActivo);
            dibujador.renderizar(&ultimo_snapshot_recibido);
            auto t2 = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
            if (elapsed < ms_per_frame) {
                std::this_thread::sleep_for(std::chrono::milliseconds(ms_per_frame - elapsed));
            }
        }
    }
}

Client::~Client(){
    hilo_enviador.stop();
    hilo_recibidor.stop();
    cola_enviador.close();
    cola_recibidor.close();
    hilo_enviador.join();
    hilo_recibidor.join();
}
