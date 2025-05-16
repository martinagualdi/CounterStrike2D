#ifndef CLIENT_DIBUJADOR_H
#define CLIENT_DIBUJADOR_H

#include "../common_src/queue.h"
#include "../common_src/snapshot.h"
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

#define ANCHO_MIN 960
#define ALTO_MIN 720
#define FPS 30
#define CANT_SKINS_JUGADORES 4

enum ct_types {SEAL_FORCE, GERMAN_GSG_9, UK_SAS, FRENCH_GIGN};
enum tt_types {PHEONIX, L337_KREW, ARTIC_AVENGER, GUERRILLA};

using namespace SDL2pp; 

class Dibujador{
private:
    Queue<Snapshot>& cola_snapshots;
    Renderer& renderer;
    Snapshot ultimo_snapshot_recibido;
    Texture fondo;
    Texture player;
public:
    Dibujador(Renderer& renderer, Queue<Snapshot>& cola_snapshots);
    void renderizar(bool& jugador_activo);
    Dibujador(const Dibujador&) = delete;
    Dibujador& operator=(const Dibujador&) = delete;
    Dibujador(Dibujador&&) = default;
    Dibujador& operator=(Dibujador&&) = default;
    ~Dibujador();
};


#endif