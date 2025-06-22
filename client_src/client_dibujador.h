#ifndef CLIENT_DIBUJADOR_H
#define CLIENT_DIBUJADOR_H

#include "../common_src/comando_dto.h"
#include "../common_src/queue.h"
#include "../common_src/snapshot.h"
#include "client_event_handler.h"
#include "client_parseador.h"
#include "client_map.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2pp/Font.hh>
#include <SDL2pp/SDL2pp.hh>

using namespace SDL2pp; 

class Dibujador{
private:
    const int client_id;
    int ancho_ventana;
    int alto_ventana;
    int ancho_real;
    int alto_real;
    InfoConfigClient infoConfig;
    Renderer& renderer;
    EventHandler& eventHandler;
    Queue<Snapshot>& cola_recibidor;
    struct Mapa mapa;
    ParseadorSpriteSheets parseador;
    Snapshot snapshot;
    enum EstadoBombaRonda estado_bomba_anterior;
    bool explosion_en_progreso;
    float explosion_alpha;
    Uint32 explosion_last_ticks;
    Font fuente;
    Font fuenteChica;
    Color amarillo;
    Color blanco;
    Color verde;
    Color rojo;
    Color amarillento;
    Color celeste;
    Texture mensaje_bomba_plantada;
    Texture mantenga_presionado_activar;
    Texture mantenga_presionado_desactivar;
    Texture partida_finalizada;
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
    std::vector<Texture> aviso_desconectar;
    std::vector<Texture> mensajes_ganadores;
    SDL_Rect sprite_arma;
    SDL_Rect sprite_bala;
    SDL_Rect sprite_sight;
    std::vector<SDL_Rect> sprites_player;
    std::vector<SDL_Rect> sprites_player_legs;
    std::vector<SDL_Rect> sprites_simbolos_hud;
    std::vector<SDL_Rect> sprites_numeros_hud;
    int tam_mascara_fov;
    Texture mascara_fov;
    std::vector<int> separar_digitos(int n);
    std::vector<int> separar_digitos_tiempo(int n);
    void inicializar_textos();
    float convertir_angulo(float angulo);
    void convertir_a_pantalla(float pos_x, float pos_y, float& pantalla_x, float& pantalla_y);
    Texture crearTextoArma(std::string nombre, int precio);
    void dibujar_salud(int salud);
    void dibujar_saldo(int saldo, bool arma_con_balas);
    void dibujar_balas_hud(int balas);
    void dibujar_tiempo(int tiempo_restante);
    void dibujar_jugadores();
    void dibujar_fondo(const ElementoMapa& elemento);
    void dibujar_balas();
    void dibujar_muertos();
    void dibujar_cuerpo(float x, float y, float angulo, enum SkinTipos skin,
             enum ArmaEnMano arma, enum Equipo equipo);
    void dibujar_pies(float x, float y, float angulo);
    void dibujar_arma(float x, float y, float angulo, enum ArmaEnMano arma_actual);
    void dibujar_sight();
    void dibujar_simbolo_mercado();
    void dibujar_simbolo_tiene_bomba();
    void dibujar_mantenga_presionado(bool activar);
    void dibujar_hud();
    void dibujar_mercado();
    void dibujar_mapa();
    void dibujar_cuadro_negro_transparente(Texture& titulo);
    void dibujar_esperando_jugadores();
    void dibujar_seleccionar_skin();
    void dibujar_armas_tiradas();
    void dibujar_explosion_bomba();
    void dibujar_mensaje_ganador();
    void dibujar_mensaje_bomba_plantada();
    void dibujar_bomba_plantada();
    void dibujar_vision_de_campo(float angulo_jugador);
    Texture crearMascaraFOV(float radio_centro, float angulo_fov, Uint8 alpha_fondo);
    void dibujar_estadisticas();
    void dibujar_aviso_desconectar();
    void dibujar_partida_finalizada();
    void dibujar_estadisticas_jugador(std::vector<int>& col_x, 
    int& y_fila_inicial, int& fila, int& altura_fila, enum Equipo equipo);
    
public:
    explicit Dibujador(
    const int id,
    Renderer& renderer,
    struct Mapa mapa,
    EventHandler& handler, 
    Queue<Snapshot>& cola_recibidor,
    int ancho_ventana,
    int alto_ventana,
    int ancho_real,
    int alto_real,
    InfoConfigClient infoConfig
    );
    void renderizar(Snapshot& snapshot);
    Dibujador(const Dibujador&) = delete;
    Dibujador& operator=(const Dibujador&) = delete;
    Dibujador(Dibujador&&) = default;
    Dibujador& operator=(Dibujador&&) = default;
    ~Dibujador();
};

#endif
