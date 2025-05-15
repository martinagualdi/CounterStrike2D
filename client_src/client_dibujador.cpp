#include "client_dibujador.h"

Dibujador::Dibujador(Queue<Snapshot>& cola_snapshots) : cola_snapshots(cola_snapshots)
{
}

void Dibujador::renderizar(Renderer &renderer, bool &jugador_activo)
{
  Snapshot snapshotActual;
  while (cola_snapshots.try_pop(snapshotActual)) {
    this->ultimo_estado_recibido = snapshotActual;
  }

  Texture fondo(renderer, Surface("client_src/gfx/backgrounds/dust.png"));
  renderer.Clear();
  renderer.Copy(fondo, NullOpt, Rect(0, 0, ANCHO_MIN, ALTO_MIN));

  
  for (const auto& jugador : this->ultimo_estado_recibido.jugadores) {
    int x = jugador.getX(); 
    int y = jugador.getY(); 
    Texture jugadorTexture(renderer, Surface("client_src/gfx/sprites/player.png"));
    renderer.Copy(jugadorTexture, NullOpt, Rect(x, y, TAM_JUGADOR, TAM_JUGADOR));
  }

  renderer.Present();
}

Dibujador::~Dibujador()
{
}
