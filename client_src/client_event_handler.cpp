#include "client_event_handler.h"
#include <map>

#define CMD_ARRIBA 0x01
#define CMD_ABAJO 0x02
#define CMD_IZQUIERDA 0x03
#define CMD_DERECHA 0x04
#define CMD_COMPRAR 0x05
#define CMD_ESTADISTICAS 0x06
#define CMD_COMPRAR_BALAS_PRIMARIA 0x07
#define CMD_COMPRAR_BALAS_SECUNDARIA 0x08
#define CMD_CLICK_IZQUIERDO 0x09
#define CMD_SCROLL_ARRIBA 0x10
#define CMD_SCROLL_ABAJO 0x11
#define CMD_EXIT 0x12

EventHandler::EventHandler(Queue<uint8_t> &cola_enviador) :
    cola_enviador(cola_enviador),
    codigos_teclado({
        {SDL_SCANCODE_W, CMD_ARRIBA}, 
        {SDL_SCANCODE_S, CMD_ABAJO}, 
        {SDL_SCANCODE_A, CMD_IZQUIERDA},
        {SDL_SCANCODE_D, CMD_DERECHA},
        {SDL_SCANCODE_B, CMD_COMPRAR},
        {SDL_SCANCODE_TAB, CMD_ESTADISTICAS},
        {SDL_SCANCODE_PERIOD, CMD_COMPRAR_BALAS_PRIMARIA},
        {SDL_SCANCODE_COMMA, CMD_COMPRAR_BALAS_SECUNDARIA},
    })
{
}

void EventHandler::procesarTeclado(const SDL_Event &event)
{
    if (event.type == SDL_KEYDOWN) {
        auto it = codigos_teclado.find(event.key.keysym.scancode);
        if (it != codigos_teclado.end()) {
            cola_enviador.try_push(it->second);
        }
    } else if(event.type == SDL_KEYUP){
        
    }
}

void EventHandler::procesarMouse(const SDL_Event &event)
{
    if (event.button.button == SDL_BUTTON_LEFT) {
        cola_enviador.try_push(CMD_CLICK_IZQUIERDO);
    }  else if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y > 0) {
            cola_enviador.try_push(CMD_SCROLL_ARRIBA);
        } else if (event.wheel.y < 0) {
            cola_enviador.try_push(CMD_SCROLL_ABAJO);
        }
    }
    
}

void EventHandler::manejarEventos(bool &jugador_activo)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            jugador_activo = false;
            cola_enviador.try_push(CMD_EXIT);
        } 
        procesarMouse(event);
        procesarTeclado(event);

    }
}

EventHandler::~EventHandler()
{
}
