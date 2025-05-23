#include "client_event_handler.h"
#include <map>
#include <cmath>
#include <iostream>

#define ALTO_MIN 720

#define CMD_ESTADISTICAS 5
#define CMD_COMPRAR_BALAS_PRIMARIA 7
#define CMD_COMPRAR_BALAS_SECUNDARIA 8
#define CMD_CLICK_IZQUIERDO 9
#define CMD_SCROLL_ARRIBA 10
#define CMD_SCROLL_ABAJO 11
#define CMD_EXIT 12

EventHandler::EventHandler(Queue<ComandoDTO> &cola_enviador, const int client_id) :
    cola_enviador(cola_enviador),
    client_id(client_id),
    teclas_validas({
        SDL_SCANCODE_W,
        SDL_SCANCODE_S,
        SDL_SCANCODE_A,
        SDL_SCANCODE_D
        }){}

void EventHandler::procesarTeclado(const SDL_Event &event)
{
    if(!teclas_validas.contains(event.key.keysym.scancode)) {
        return;
    }

    if ((event.type == SDL_KEYDOWN && event.key.repeat == 0)) {
        teclas_presionadas.insert(event.key.keysym.scancode);
    } 

    else if (event.type == SDL_KEYUP){
        teclas_presionadas.erase(event.key.keysym.scancode);
    }

    bool w = teclas_presionadas.count(SDL_SCANCODE_W);
    bool a = teclas_presionadas.count(SDL_SCANCODE_A);
    bool s = teclas_presionadas.count(SDL_SCANCODE_S);
    bool d = teclas_presionadas.count(SDL_SCANCODE_D);

    ComandoDTO comando;
    comando.tipo = MOVIMIENTO;
    if (w && a)
        comando.movimiento = DIAGONAL_SUP_IZQ;
    else if (w && d)
        comando.movimiento = DIAGONAL_SUP_DER;
    else if (s && a)
        comando.movimiento = DIAGONAL_INF_IZQ;
    else if (s && d)
        comando.movimiento = DIAGONAL_INF_DER;
    else if (w)
        comando.movimiento = ARRIBA;
    else if (s)
        comando.movimiento = ABAJO;
    else if (a)
        comando.movimiento = IZQUIERDA;
    else if (d)
        comando.movimiento = DERECHA;
    else
        comando.movimiento = DETENER;

    cola_enviador.try_push(comando);
}

float EventHandler::calcularAngulo(float x_personaje, float y_personaje, int x_mouse, int y_mouse) {
    float dx = x_mouse - x_personaje;
    float dy = y_personaje  -y_mouse;
    float angulo_radianes = std::atan2(dy, dx);
    float angulo_grados = angulo_radianes * (180.0 / M_PI);
    if(angulo_grados < 0) 
        angulo_grados += 360.0f;
    return angulo_grados;
}

void EventHandler::convertir_coordenadas(float &x, float &y) {
    x = x;
    y = ALTO_MIN - y;
}

float EventHandler::procesarPuntero(Snapshot& snapshot) {
    Jugador* jugador = snapshot.getJugadorPorId(client_id);
    float jugador_x = jugador->getX();
    float jugador_y = jugador->getY();
    convertir_coordenadas(jugador_x, jugador_y);    
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    return calcularAngulo(jugador_x, jugador_y, mouseX, mouseY);
}

void EventHandler::procesarMouse(const SDL_Event &event, Snapshot& snapshot)
{
    if(event.type == SDL_MOUSEMOTION){
        
        ComandoDTO comando;
        comando.tipo = ROTACION;
        comando.angulo = procesarPuntero(snapshot);
        cola_enviador.try_push(comando);
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        ComandoDTO comando;
        comando.tipo = DISPARO;
        comando.angulo = procesarPuntero(snapshot);
        cola_enviador.try_push(comando);
    }  else if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y > 0) {
           // cola_enviador.try_push(CMD_SCROLL_ARRIBA);
        } else if (event.wheel.y < 0) {
           // cola_enviador.try_push(CMD_SCROLL_ABAJO);
        }
    }
    
}

void EventHandler::manejarEventos(bool &jugador_activo, Snapshot& snapshot)
{
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        if(event.type == SDL_QUIT){
            jugador_activo = false;
            //cola_enviador.try_push(CMD_EXIT);
            return;
        } 
        procesarMouse(event, snapshot);
        procesarTeclado(event);

    }
}

EventHandler::~EventHandler()
{
}
