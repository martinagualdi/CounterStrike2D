#include "client_dibujador.h"

Dibujador::Dibujador(Queue<Snapshot>& cola_snapshots) : cola_snapshots(cola_snapshots)
{
}

void Dibujador::renderizar(Renderer &renderer, bool &jugador_activo)
{
    //Snapshot snapshotActual;
    //while (cola_snapshots.try_pop(snapshotActual)) {
      //  this->ultimo_estado_recibido = estado_actual;
    //}
 
    Texture fondo(renderer, Surface("client_src/gfx/backgrounds/dust.png"));
    renderer.Clear();
    renderer.Copy(fondo,NullOpt,Rect(0, 0, ANCHO_MIN, ALTO_MIN));
    renderer.Present();
   
}   

Dibujador::~Dibujador()
{
}
