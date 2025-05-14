#ifndef CLIENT_EVENT_HANDLER_H
#define CLIENT_EVENT_HANDLER_H

#include <SDL2/SDL.h>
#include <map>
#include "../common_src/queue.h"
class EventHandler {

private:
    Queue<uint8_t>& cola_enviador;
    const std::map<SDL_Scancode, uint8_t> codigos_teclado;
    void procesarTeclado(const SDL_Event& event);
    void procesarMouse(const SDL_Event& event);

public:
    explicit EventHandler(Queue<uint8_t>& cola_enviador);
    void manejarEventos(bool& isRunning);
    EventHandler(const EventHandler&) = delete;
    EventHandler& operator=(const EventHandler&) = delete;
    EventHandler(EventHandler&&) = default;
    EventHandler& operator=(EventHandler&&) = default;
    ~EventHandler();
};

#endif