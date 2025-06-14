#ifndef CLIENT_DIBUJADOR_H
#define CLIENT_DIBUJADOR_H

#include "../common_src/queue.h"
#include "../common_src/snapshot.h"
#include "client_event_handler.h"
#include "client_parseador.h"
#include "client_map.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2pp/Font.hh>
#include <SDL2pp/SDL2pp.hh>

#define ANCHO_MIN 960
#define ALTO_MIN 720
#define CANT_PLAYERS 4

using namespace SDL2pp; 

class Dibujador{
private:
    const int client_id;
    Renderer& renderer;
    EventHandler& eventHandler;
    Queue<Snapshot>& cola_recibidor;
    struct Mapa mapa;
    ParseadorSpriteSheets parseador;
    Snapshot snapshot;
    bool estado_bomba_anterior;
    bool explosion_en_progreso;
    float explosion_alpha;
    Uint32 explosion_last_ticks;
    Font fuente;
    Font fuenteChica;
    Color amarillo;
    Texture mensaje_bomba_plantada;
    Texture fondo_transparente;
    Texture balas;
    Texture cs2d;
    Texture player_legs;
    Texture muerto;
    Texture simbolos_hud;
    Texture numeros_hud;
    Texture sight;
    std::vector<Texture> armas;
    std::vector<Texture> armas_mercado_y_tiradas;
    std::vector<Texture> ct_players;
    std::vector<Texture> tt_players;
    std::vector<Texture> textos_skin;
    std::vector<Texture> ct_nombres;
    std::vector<Texture> tt_nombres;
    std::vector<Texture> esperando_jugadores;
    std::vector<Texture> mensajes_ganadores;
    SDL_Rect sprite_arma;
    SDL_Rect sprite_bala;
    SDL_Rect sprite_sight;
    std::vector<SDL_Rect> sprites_player;
    std::vector<SDL_Rect> sprites_player_legs;
    std::vector<SDL_Rect> sprites_simbolos_hud;
    std::vector<SDL_Rect> sprites_numeros_hud;
    std::vector<int> separar_digitos(int n);
    std::vector<int> separar_digitos_tiempo(int n);
    void inicializar_textos();
    float convertir_angulo(float angulo);
    void convertir_a_pantalla(float pos_x, float pos_y, float& pantalla_x, float& pantalla_y);
    void convertir_coordenadas(float &x, float &y);
    Texture crearTextoArma(std::string nombre, int precio);
    void dibujar_salud(int salud);
    void dibujar_saldo(int saldo, bool arma_con_balas);
    void dibujar_balas_hud(int balas);
    void dibujar_tiempo(int tiempo_restante);
    void dibujar_jugadores();
    void dibujar_fondo(const ElementoMapa& elemento);
    void dibujar_balas();
    void dibujar_muerto(int x_pixel, int y_pixel);
    void dibujar_cuerpo(float x, float y, float angulo, enum SkinTipos skin,
             enum ArmaEnMano arma, enum Equipo equipo);
    void dibujar_pies(float x, float y, float angulo);
    void dibujar_arma(float x, float y, float angulo, enum ArmaEnMano arma_actual);
    void dibujar_sight();
    void dibujar_simbolo_mercado();
    void dibujar_hud();
    void dibujar_mercado();
    void dibujar_mapa();
    void dibujar_esperando_jugadores();
    void dibujar_seleccionar_skin();
    void dibujar_armas_tiradas();
    void dibujar_explosion_bomba();
    void dibujar_mensaje_ganador();
    void dibujar_mensaje_bomba_plantada();
    void dibujar_vision_de_campo();
    
public:
    explicit Dibujador(const int id, Renderer& renderer, struct Mapa mapa, 
                EventHandler& handler, Queue<Snapshot>& cola_recibidor);
    void renderizar(Snapshot& snapshot);
    Dibujador(const Dibujador&) = delete;
    Dibujador& operator=(const Dibujador&) = delete;
    Dibujador(Dibujador&&) = default;
    Dibujador& operator=(Dibujador&&) = default;
    ~Dibujador();
};

#endif
