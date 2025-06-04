#ifndef CLIENT_EVENT_HANDLER_H
#define CLIENT_EVENT_HANDLER_H

#include <SDL2/SDL.h>
#include <unordered_set>
#include "../common_src/queue.h"
#include "../common_src/snapshot.h"
#include "../common_src/comando_dto.h"
class EventHandler {

private:
    Queue<ComandoDTO>& cola_enviador;
    const int client_id;
    std::unordered_set<SDL_Scancode> teclas_validas;
    std::unordered_set<SDL_Scancode> teclas_presionadas;
    void procesarTeclado(const SDL_Event& event);
    void procesarMouse(const SDL_Event& event);
    float procesarPuntero();
    void convertir_coordenadas(float& x, float& y);
    float calcularAngulo(float x_personaje, float y_personaje, int x_mouse, int y_mouse);

public:
    explicit EventHandler(Queue<ComandoDTO>& cola_enviador, const int client_id);
    void manejarEventos(bool& isRunning);
    EventHandler(const EventHandler&) = delete;
    EventHandler& operator=(const EventHandler&) = delete;
    EventHandler(EventHandler&&) = default;
    EventHandler& operator=(EventHandler&&) = default;
    ~EventHandler();
};
#endif
