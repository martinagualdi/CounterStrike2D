#include "client_event_handler.h"
#include <map>
#include <cmath>
#include <iostream>

EventHandler::EventHandler(Queue<ComandoDTO> &cola_enviador, 
const int client_id, int ancho_ventana, int alto_ventana) :
    cola_enviador(cola_enviador),
    client_id(client_id),
    ancho_ventana(ancho_ventana),
    alto_ventana(alto_ventana),
    mercado_abierto(false),
    puede_comprar(false),
    skin_seleccionado(false),
    aviso_desconectar_activo(false),
    estadisticas(false),
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

        if (sc == SDL_SCANCODE_ESCAPE) {
            mercado_abierto = false;
            return;
        }

        if(!mercado_abierto && puede_comprar){
            if(sc == SDL_SCANCODE_COMMA){
                ComandoDTO comando;
                comando.tipo = COMPRAR;
                comando.compra = BALAS_PRIMARIA;
                cola_enviador.try_push(comando);
            } else if(sc == SDL_SCANCODE_PERIOD){
                ComandoDTO comando;
                comando.tipo = COMPRAR;
                comando.compra = BALAS_SECUNDARIA;
                cola_enviador.try_push(comando);
            }
        }

        if (sc == SDL_SCANCODE_B && puede_comprar) {
            mercado_abierto = true;
                // Si el mercado se acaba de abrir, detener el movimiento
            if (!teclas_presionadas.empty()) {
                ComandoDTO comando;
                comando.tipo = MOVIMIENTO;
                comando.movimiento = DETENER;
                cola_enviador.try_push(comando);
                teclas_presionadas.clear();
            }
            
            return; 
        }

        if (mercado_abierto) {

            ComandoDTO comando;
            comando.tipo = COMPRAR;

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
                    comando.compra = BALAS_PRIMARIA;
                    cola_enviador.try_push(comando);
                    mercado_abierto = false;
                    break;
                case SDL_SCANCODE_PERIOD:
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

bool EventHandler::avisoDesconectarActivo() const {
    return aviso_desconectar_activo;
}

bool EventHandler::puedeMostrarEstadisticas() const {
    return estadisticas;
}

void EventHandler::cerrarMercado() {
    this->mercado_abierto = false;
}

bool EventHandler::skinSeleccionado() const {
    return skin_seleccionado;
}

float EventHandler::procesarPuntero() { 
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    return calcularAngulo(ancho_ventana / 2, alto_ventana / 2, mouseX, mouseY);
}

void EventHandler::procesarMouse(const SDL_Event &event)
{
    if(event.type == SDL_MOUSEMOTION){
        ComandoDTO comando = {};
        comando.tipo = ROTACION;
        comando.angulo = procesarPuntero();
        cola_enviador.try_push(comando);
    }
    else if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
        ComandoDTO comando = {};
        comando.tipo = DISPARO;
        comando.angulo = procesarPuntero();
        cola_enviador.try_push(comando);
    }  
    else if (event.type == SDL_MOUSEWHEEL && event.wheel.y < 0) {
        ComandoDTO comando = {};
        comando.tipo = CAMBIAR_ARMA;
        cola_enviador.try_push(comando);
    }
    
}

void EventHandler::procesarSkin(const SDL_Event &event) {

    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {

        ComandoDTO comando = {};
        comando.tipo = SELECCIONAR_SKIN;

        switch(event.key.keysym.scancode){
            case SDL_SCANCODE_1:
            case SDL_SCANCODE_KP_1:
                comando.skin = SKIN1;
                cola_enviador.try_push(comando);
                skin_seleccionado = true;
                break;
            case SDL_SCANCODE_2:
            case SDL_SCANCODE_KP_2:
                comando.skin = SKIN2;
                cola_enviador.try_push(comando);
                skin_seleccionado = true;
                break;
            case SDL_SCANCODE_3:
            case SDL_SCANCODE_KP_3:
                comando.skin = SKIN3;
                cola_enviador.try_push(comando);
                skin_seleccionado = true;
                break;
            case SDL_SCANCODE_4:
            case SDL_SCANCODE_KP_4:
                comando.skin = SKIN4;
                cola_enviador.try_push(comando);
                skin_seleccionado = true;
                break;       
            default:
                break;
        }
    }

}

void EventHandler::procesarPlantarBomba(const SDL_Event &event) {
    if(event.key.keysym.scancode == SDL_SCANCODE_E){
        ComandoDTO comando = {};
        comando.tipo = ACCION_SOBRE_BOMBA;
        if (event.type == SDL_KEYDOWN && !event.key.repeat) {
            comando.estado_bomba = ACCIONANDO;
            cola_enviador.try_push(comando);
        } else if(event.type == SDL_KEYUP){
            comando.estado_bomba = DETENIDO;
            cola_enviador.try_push(comando);
        }
    }
}

void EventHandler::procesarDrop(const SDL_Event &event) {

    if(event.type == SDL_KEYDOWN && !event.key.repeat){
        if(event.key.keysym.scancode == SDL_SCANCODE_G){
            ComandoDTO comando = {};
            comando.tipo = DROPEAR;
            cola_enviador.try_push(comando);
        }
    }
}

void EventHandler::procesarLevantar(const SDL_Event &event) {

    if(event.type == SDL_KEYDOWN && !event.key.repeat){
        if(event.key.keysym.scancode == SDL_SCANCODE_F){
            ComandoDTO comando = {};
            comando.tipo = LEVANTAR;
            cola_enviador.try_push(comando);
        }
    }

}

void EventHandler::procesarEstadisticas(const SDL_Event &event) {

    if(event.key.keysym.scancode == SDL_SCANCODE_TAB){   
        if(event.type == SDL_KEYDOWN && !event.key.repeat){
            estadisticas = true;
        } else if(event.type == SDL_KEYUP){
            estadisticas = false;
        }
    }
}

bool EventHandler::procesarDesconectar(const SDL_Event &event, bool& jugador_activo) {

    if(event.type == SDL_QUIT){
        ComandoDTO comando = {};
        comando.tipo = DESCONECTAR;
        cola_enviador.try_push(comando);
        jugador_activo = false;   
        return true;
    }

    if (event.type == SDL_KEYDOWN && event.key.repeat == 0) {
        SDL_Scancode sc = event.key.keysym.scancode;

        if(aviso_desconectar_activo && sc == SDL_SCANCODE_RETURN){
            ComandoDTO comando = {};
            comando.tipo = DESCONECTAR;
            cola_enviador.try_push(comando);        
            jugador_activo = false;
            return true;
        }

        if(sc == SDL_SCANCODE_Q){
            aviso_desconectar_activo = !aviso_desconectar_activo;
        }

    }

    return false;
}

void EventHandler::manejarEventos(bool &jugador_activo, bool puede_comprar)
{
    SDL_Event event;
    this->puede_comprar = puede_comprar;

    if(!puede_comprar && mercado_abierto){
        mercado_abierto = false;
    }

    while(SDL_PollEvent(&event)){
        
        if(procesarDesconectar(event, jugador_activo)) 
            return;

        if(!skin_seleccionado)
            procesarSkin(event);
        else{
            procesarCompra(event);
            if(!mercado_abierto){
                procesarMouse(event);
                procesarMovimiento(event);
                procesarPlantarBomba(event);
                procesarDrop(event);
                procesarLevantar(event);
                procesarEstadisticas(event);
            }
        }
        
    }
}

EventHandler::~EventHandler()
{
}
