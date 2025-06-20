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
    int ancho_ventana;
    int alto_ventana;
    bool mercado_abierto;
    bool puede_comprar;
    bool skin_seleccionado;
    bool aviso_desconectar_activo;
    bool estadisticas;
    std::unordered_set<SDL_Scancode> teclas_validas;
    std::unordered_set<SDL_Scancode> teclas_presionadas;
    void procesarMovimiento(const SDL_Event& event);
    void procesarCompra(const SDL_Event& event);
    void procesarMouse(const SDL_Event& event);
    void procesarSkin(const SDL_Event& event);
    void procesarPlantarBomba(const SDL_Event& event);
    void procesarDrop(const SDL_Event& event);
    void procesarLevantar(const SDL_Event& event);
    void procesarEstadisticas(const SDL_Event& event);
    bool procesarDesconectar(const SDL_Event& event, bool& jugador_activo);
    float procesarPuntero();
    float calcularAngulo(float x_personaje, float y_personaje, int x_mouse, int y_mouse);

public:
    EventHandler(Queue<ComandoDTO>& cola_enviador, 
        const int client_id, int ancho_ventana, int alto_ventana);
    void manejarEventos(bool& isRunning, bool puede_comprar);
    bool mercadoAbierto() const;
    bool avisoDesconectarActivo() const;
    bool puedeMostrarEstadisticas() const;
    void cerrarMercado();
    bool skinSeleccionado() const;
    EventHandler(const EventHandler&) = delete;
    EventHandler& operator=(const EventHandler&) = delete;
    EventHandler(EventHandler&&) = default;
    EventHandler& operator=(EventHandler&&) = default;
    ~EventHandler();
};
#endif
