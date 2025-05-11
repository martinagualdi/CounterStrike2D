#ifndef CLIENT_EVENT_HANDLER_H
#define CLIENT_EVENT_HANDLER_H

#include <SDL2/SDL.h>

class ClientEventHandler {

private:
    void procesarTeclado(const SDL_Event& event);
    void procesarMouse(const SDL_Event& event);

public:
    ClientEventHandler();
    void manejarEventos(bool& isRunning);

};

#endif