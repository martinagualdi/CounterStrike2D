#include "client_event_handler.h"
#include <map>
#include <cmath>
#include <iostream>

#define ALTO_MIN 720

enum {CMD_ARRIBA, CMD_ABAJO, CMD_IZQUIERDA, CMD_DERECHA, CMD_COMPRAR};

#define CMD_ESTADISTICAS 5
#define CMD_COMPRAR_BALAS_PRIMARIA 7
#define CMD_COMPRAR_BALAS_SECUNDARIA 8
#define CMD_CLICK_IZQUIERDO 9
#define CMD_SCROLL_ARRIBA 10
#define CMD_SCROLL_ABAJO 11
#define CMD_EXIT 12

EventHandler::EventHandler(Queue<ComandoDTO> &cola_enviador) :
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
            ComandoDTO comando;
            if(it->second <= 3){
                comando.tipo = MOVIMIENTO;
                comando.movimiento = (enum Movimiento)it->second;
                cola_enviador.try_push(comando);
            }

        }
    } 

}

void EventHandler::procesarBotonesMouse(const SDL_Event &event)
{
    if (event.button.button == SDL_BUTTON_LEFT) {
        ComandoDTO comando;
        comando.tipo = DISPARO;
        cola_enviador.try_push(comando);
    }  else if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y > 0) {
           // cola_enviador.try_push(CMD_SCROLL_ARRIBA);
        } else if (event.wheel.y < 0) {
           // cola_enviador.try_push(CMD_SCROLL_ABAJO);
        }
    }
    
}

void EventHandler::convertir_coordenadas(int &x, int &y) {
    x = x;
    y = ALTO_MIN - y;
}

void EventHandler::procesarPuntero() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    convertir_coordenadas(mouseX, mouseY);
    ComandoDTO comando;
    comando.tipo = ROTACION;
    comando.mouseX = static_cast<uint16_t>(mouseX);
    comando.mouseY = static_cast<uint16_t>(mouseY);
    cola_enviador.try_push(comando);
}

void EventHandler::manejarEventos(bool &jugador_activo)
{
    SDL_Event event;
     while(SDL_PollEvent(&event)){
         if(event.type == SDL_QUIT){
            jugador_activo = false;
            //cola_enviador.try_push(CMD_EXIT);
            return;
        } 
        procesarPuntero();
        procesarBotonesMouse(event);
        procesarTeclado(event);

    }
}

EventHandler::~EventHandler()
{
}
