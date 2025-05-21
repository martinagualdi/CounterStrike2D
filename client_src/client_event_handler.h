#ifndef CLIENT_EVENT_HANDLER_H
#define CLIENT_EVENT_HANDLER_H

#include <SDL2/SDL.h>
#include <map>
#include "../common_src/queue.h"
#include "../common_src/comando_dto.h"
class EventHandler {

private:
    Queue<ComandoDTO>& cola_enviador;
    std::map<SDL_Scancode, uint8_t> codigos_teclado;
    void procesarTeclado(const SDL_Event& event);
    void procesarBotonesMouse(const SDL_Event& event);
    void procesarPuntero();
    void convertir_coordenadas(int& x, int& y);

public:
    explicit EventHandler(Queue<ComandoDTO>& cola_enviador);
    void manejarEventos(bool& isRunning);
    EventHandler(const EventHandler&) = delete;
    EventHandler& operator=(const EventHandler&) = delete;
    EventHandler(EventHandler&&) = default;
    EventHandler& operator=(EventHandler&&) = default;
    ~EventHandler();
};
#endif
