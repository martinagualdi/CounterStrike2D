#include "client_event_handler.h"
#include <map>
#include <cmath>
#include <iostream>

#define ALTO_MIN 720
#define ANCHO_MIN 960

EventHandler::EventHandler(Queue<ComandoDTO> &cola_enviador, const int client_id) :
    cola_enviador(cola_enviador),
    client_id(client_id),
    mercado_abierto(false),
    teclas_validas({
        SDL_SCANCODE_W,
        SDL_SCANCODE_S,
        SDL_SCANCODE_A,
        SDL_SCANCODE_D,
        SDL_SCANCODE_B
        }){}

void EventHandler::procesarMovimiento(const SDL_Event &event)
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

void EventHandler::procesarCompra(const SDL_Event &event) {
    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        SDL_Scancode sc = event.key.keysym.scancode;
    


        if (sc == SDL_SCANCODE_B) {
            mercado_abierto = !mercado_abierto;

            if (mercado_abierto) {
                // Si el mercado se acaba de abrir, detener el movimiento
                if (!teclas_presionadas.empty()) {
                    ComandoDTO comando;
                    comando.tipo = MOVIMIENTO;
                    comando.movimiento = DETENER;
                    cola_enviador.try_push(comando);
                    teclas_presionadas.clear();
                }
            }

            return; 
        }

        if (mercado_abierto) {
            if (sc == SDL_SCANCODE_ESCAPE) {
                mercado_abierto = false;
                return;
            }

            ComandoDTO comando;
            comando.tipo = COMPRAR;
            comando.compra = C_NULL;

            switch (sc) {
                case SDL_SCANCODE_1:
                case SDL_SCANCODE_KP_1:
                    comando.compra = C_AK47;
                    cola_enviador.try_push(comando);
                    mercado_abierto = false;
                    break;
                case SDL_SCANCODE_2:
                case SDL_SCANCODE_KP_2:
                    comando.compra = C_M3;
                    cola_enviador.try_push(comando);
                    mercado_abierto = false;
                    break;
                case SDL_SCANCODE_3:
                case SDL_SCANCODE_KP_3:
                    comando.compra = C_AWP;
                    cola_enviador.try_push(comando);
                    mercado_abierto = false;
                    break;
                case SDL_SCANCODE_COMMA:
                case SDL_SCANCODE_KP_COMMA:
                    comando.compra = BALAS_PRIMARIA;
                    cola_enviador.try_push(comando);
                    mercado_abierto = false;
                    break;
                case SDL_SCANCODE_PERIOD:
                case SDL_SCANCODE_KP_PERIOD:
                    comando.compra = BALAS_SECUNDARIA;
                    cola_enviador.try_push(comando);
                    mercado_abierto = false;
                    break;
                default:
                    break;
            }
        }
    }
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

bool EventHandler::mercadoAbierto() const {
    return mercado_abierto;
}

void EventHandler::convertir_coordenadas(float &x, float &y) {
    x = x;
    y = ALTO_MIN - y;
}

float EventHandler::procesarPuntero() { 
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    return calcularAngulo(ANCHO_MIN/2, ALTO_MIN/2, mouseX, mouseY);
}

void EventHandler::procesarMouse(const SDL_Event &event)
{
    if(event.type == SDL_MOUSEMOTION){
        
        ComandoDTO comando;
        comando.tipo = ROTACION;
        comando.angulo = procesarPuntero();
        cola_enviador.try_push(comando);
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        ComandoDTO comando;
        comando.tipo = DISPARO;
        comando.angulo = procesarPuntero();
        cola_enviador.try_push(comando);
    }  else if (event.type == SDL_MOUSEWHEEL) {
        if (event.wheel.y < 0) {
            ComandoDTO comando;
            comando.tipo = CAMBIAR_ARMA;
            cola_enviador.try_push(comando);
        }
    }
    
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

        procesarCompra(event);

        if(!mercado_abierto){
            procesarMouse(event);
            procesarMovimiento(event);
        }
    }
}

EventHandler::~EventHandler()
{
}
