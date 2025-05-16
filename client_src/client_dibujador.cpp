#include "client_dibujador.h"

Dibujador::Dibujador(Renderer& renderer, Queue<Snapshot>& cola_snapshots) : 
    cola_snapshots(cola_snapshots),
    renderer(renderer),
    fondo(Texture(renderer, "client_src/gfx/backgrounds/dust.png")),
    player(Texture(renderer, "client_src/gfx/player/ct1.bmp")) {}

void Dibujador::renderizar(bool &jugador_activo)
{
   /* Snapshot snapshotActual;
    while (cola_snapshots.try_pop(snapshotActual)) {
        this->ultimo_snapshot_recibido = snapshotActual;
    }*/

    renderer.Clear();
    renderer.Copy(fondo, NullOpt, Rect(0, 0, ANCHO_MIN, ALTO_MIN));

   // if (snapshotActual.jugadores.size() > 0) {
        Rect destino((ANCHO_MIN/2)-50/*snapshotActual.jugadores[0].getX()*/, (ALTO_MIN/2)-50/*snapshotActual.jugadores[0].getY()*/, 100, 100);
        renderer.Copy(player, Rect(0, 0, 32, 32), destino);
   // }
    
    /*for (const auto& jugador : this->ultimo_snapshot_recibido.jugadores) {
        int x = jugador.getX(); 
        int y = jugador.getY(); 
        Texture jugadorTexture(renderer, Surface("client_src/gfx/sprites/player.png"));
        renderer.Copy(jugadorTexture, NullOpt, Rect(x, y, TAM_JUGADOR, TAM_JUGADOR));
    }*/   

    renderer.Present();
}


Dibujador::~Dibujador()
{
}
