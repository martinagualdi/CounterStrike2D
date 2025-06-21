#include "client.h"
#include "client_map.h"
#include <SDL2pp/SDL2pp.hh>
#include <SDL2/SDL.h>
#include <QApplication>

#include "lobby_window.h"
#include "client_sonido.h"
#include "client_dibujador.h"
#include "client_event_handler.h"

#define CLIENT_CONFIG_PATH "client_config.yaml"
#define CS2D_TITLE "Counter-Strike 2D"
using namespace SDL2pp;

#define DEFAULT_ANCHO 960
#define DEFAULT_ALTO 720

Client::Client(const char *hostname, const char *servname, const char* username) : protocolo(hostname, servname), 
    username(username), cliente_id(-1), clienteActivo(true), cola_enviador(), cola_recibidor(), hilo_enviador(protocolo, cola_enviador),
    hilo_recibidor(protocolo, cola_recibidor), puede_comprar(true) {}

void Client::iniciar() {
    cliente_id = protocolo.recibirID();
    int default_ancho = DEFAULT_ANCHO;
    int default_alto = DEFAULT_ALTO;
    LobbyWindow lobby(protocolo, username);
    int result = lobby.exec();
    if (result != QDialog::Accepted) return;
    std::string mapa_inicial = protocolo.recibir_mapa();
    InfoConfigClient infoConfig = protocolo.recibir_configuracion_inicial();
    hilo_enviador.start();
    hilo_recibidor.start();
    Configuracion::cargar_path(CLIENT_CONFIG_PATH);
    int ancho_ventana = Configuracion::get<int>("ancho_ventana");
    int alto_ventana = Configuracion::get<int>("alto_ventana");
    bool fullscreen = Configuracion::get<bool>("fullscreen");
    int fps = Configuracion::get<int>("fps");
    SDL sdl(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    SDLTTF ttf;
    SDL_ShowCursor(SDL_DISABLE);
    Window window(CS2D_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ancho_ventana, alto_ventana, SDL_WINDOW_SHOWN);
    Renderer renderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(fullscreen)  {
        window.SetFullscreen(SDL_WINDOW_FULLSCREEN_DESKTOP);
        Point dimensionVentana = window.GetSize();
        ancho_ventana = dimensionVentana.GetX();
        alto_ventana = dimensionVentana.GetY();
        default_ancho = dimensionVentana.GetX();
        default_alto = dimensionVentana.GetY();
    }else if(ancho_ventana != default_ancho && alto_ventana != default_alto) {
        renderer.SetLogicalSize(default_ancho, default_alto);
    }
    ClientMap mapa(mapa_inicial, renderer);
    EventHandler eventHandler(cola_enviador, cliente_id, ancho_ventana, alto_ventana);
    Dibujador dibujador(cliente_id, renderer, mapa.parsearMapa(),
        eventHandler, cola_recibidor, default_ancho, default_alto, ancho_ventana, alto_ventana, infoConfig);        Sonido sonido(cliente_id);
    int ms_per_frame = 1000 / fps;
    Snapshot snapshotActual;
    Snapshot snapshotFinal; // Guardamos el snapshot final
    bool partida_terminada = false;

    while(clienteActivo){
        auto t1 = std::chrono::steady_clock::now();
        std::vector<Snapshot> snapshots;
        if (!partida_terminada) {
            while (cola_recibidor.try_pop(snapshotActual)) {
                snapshots.push_back(snapshotActual);
                // Chequeá si terminó la partida en alguno de los nuevos snapshots
                if (snapshotActual.termino_partida) {
                    partida_terminada = true;
                    snapshotFinal = snapshotActual; // Guardá el snapshot de fin de partida
                    break;
                }
            }
        }

        if (!partida_terminada) {
            // Normal: reproducir sonidos y renderizar el último snapshot recibido
            for (auto& s : snapshots) {
                sonido.reproducirSonidos(s);
            }
            if (!snapshots.empty()) {
                Snapshot snapshot = snapshots.back();
                dibujador.renderizar(snapshot);
                puede_comprar = snapshot.getJugadorPorId(cliente_id)->puede_comprar_ya;
            }
        } else {
            dibujador.renderizar(snapshotFinal);
            
        }
        eventHandler.manejarEventos(clienteActivo, puede_comprar);
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
    cola_enviador.close();
    cola_recibidor.close();
    hilo_enviador.join();
    hilo_recibidor.join();
}
