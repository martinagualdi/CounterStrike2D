#include "client_dibujador.h"
#include <thread>
#include <iostream> 

Dibujador::Dibujador(const int id, Renderer& renderer, Queue<Snapshot>& cola_snapshots) : 
    client_id(id),
    cola_snapshots(cola_snapshots),
    renderer(renderer),
    fondo(Texture(renderer, Surface(IMG_Load("client_src/gfx/backgrounds/dust.png")))),
    player(Texture(renderer, Surface(IMG_Load("client_src/gfx/player/ct1.bmp")))) {}
    
float calcularAngulo(int x_personaje, int y_personaje, int x_mouse, int y_mouse) {
    int dx = x_mouse - x_personaje;
    int dy = y_mouse - y_personaje;
    float angulo_radianes = std::atan2(dy, dx);
    float angulo_grados = angulo_radianes * (180.0 / M_PI);
    return angulo_grados;
}

void Dibujador::renderizar(/*bool &jugador_activo*/)
{
    Snapshot snapshotActual;
    while (cola_snapshots.try_pop(snapshotActual)) {
        ultimo_snapshot_recibido = snapshotActual;
    }

    renderer.Clear();
    renderer.Copy(fondo, NullOpt, Rect(0, 0, ANCHO_MIN, ALTO_MIN));
    for(const Jugador& jugador : snapshotActual.info_jugadores){
        int x_pixel = jugador.getX();
        int y_pixel = ALTO_MIN - jugador.getY();
        Rect destino(x_pixel-50, y_pixel-50, 100, 100);
        SDL_Point center = {50, 50};

        renderer.Copy(player, Rect(32, 32, 32, 32), destino, jugador.getAngulo(), center, 0);
    }
    renderer.Present();
}


Dibujador::~Dibujador()
{
}
