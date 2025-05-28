#include "client_dibujador.h"
#include <thread>
#include <iostream> 

#define TAM_SIGHT 46
#define TAM_PLAYER 60
#define TAM_SIMBOLOS_HUD 50
#define DESFASE_ANGULO 90
#define ANCHO_NUMEROS_HUD 48
#define OFFSET_TIEMPO 130
#define OFFSET_SALDO 2.1

Dibujador::Dibujador(const int id, Renderer& renderer) : 
    client_id(id),
    renderer(renderer),
    parseador(),
    snapshot(nullptr),
    fondo(Texture(renderer, Surface(IMG_Load("client_src/gfx/backgrounds/dust.png")))),
    player_legs(Texture(renderer, Surface(IMG_Load("client_src/gfx/player/legs.bmp")))),
    simbolos_hud([&renderer]() {
        Surface s(IMG_Load("client_src/gfx/hud_symbols.bmp"));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 0, 0, 0));
        return Texture(renderer, s);
    }()),
    numeros_hud([&renderer]() {
        Surface s(IMG_Load("client_src/gfx/hud_nums.bmp"));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 0, 0, 0));
        return Texture(renderer, s);
    }()),
    sight([&renderer]() {
        Surface s(IMG_Load("client_src/gfx/pointer.bmp"));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 255, 0, 255));
        return Texture(renderer, s);
    }()),
    ct_players([&renderer]() {
        std::vector<SDL2pp::Texture> textures;
        for (int i = 1; i <= 4; ++i) {
            std::string path = "client_src/gfx/player/ct" + std::to_string(i) + ".bmp";
            textures.emplace_back(renderer, Surface(IMG_Load(path.c_str())));
        }
        return textures;
    }()),
    tt_players([&renderer]() {
        std::vector<SDL2pp::Texture> textures;
        for (int i = 1; i <= 4; ++i) {
            std::string path = "client_src/gfx/player/t" + std::to_string(i) + ".bmp";
            textures.emplace_back(renderer, Surface(IMG_Load(path.c_str())));
        }
        return textures;
    }()),
    sprites_player(parseador.obtener_sprites_jugador()),
    sprites_player_legs(parseador.obtener_sprites_pies_jugador()),
    sprites_simbolos_hud(parseador.obtener_sprites_simbolos_hud()),
    sprites_numeros_hud(parseador.obtener_sprites_numeros_hud())
    {}


void Dibujador::dibujar_jugadores() {
    for(const Jugador& jugador : snapshot->info_jugadores){
        
        float x_pixel = jugador.getX();
        float y_pixel = ALTO_MIN -jugador.getY();
        float angulo_sdl = 360.0f - jugador.getAngulo() + DESFASE_ANGULO;

        //dibujar_pies(x_pixel, y_pixel, angulo_sdl);
        SDL_FRect dst {x_pixel - TAM_PLAYER / 2, y_pixel - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
        SDL_Rect sprite = sprites_player[3];
        SDL_FPoint center = {TAM_PLAYER / 2, TAM_PLAYER / 2};
        SDL_RenderCopyExF(renderer.Get(), tt_players[GUERRILLA].Get(), &sprite, &dst, angulo_sdl, &center, SDL_FLIP_NONE);
        
    }
}

void Dibujador::dibujar_fondo() {
    Rect dst(Rect(0, 0, ANCHO_MIN, ALTO_MIN));
    renderer.Copy(fondo, NullOpt, dst);
}

void Dibujador::dibujar_pies(float x, float y, float angulo) {
    Uint32 current_ticks = SDL_GetTicks();
    int i = (current_ticks / 10) % sprites_player_legs.size();
    SDL_Rect sprite_actual = sprites_player_legs[i];   
    SDL_FRect dst {x - TAM_PLAYER / 2, y - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
    SDL_FPoint center = {TAM_PLAYER, TAM_PLAYER};
    SDL_RenderCopyExF(renderer.Get(), player_legs.Get(), &sprite_actual, &dst, angulo, &center, SDL_FLIP_NONE);
}

void Dibujador::dibujar_sight() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    Rect sprite(0, 0, TAM_SIGHT, TAM_SIGHT);
    Rect dst(mouseX - TAM_SIGHT / 2, mouseY - TAM_SIGHT / 2, TAM_SIGHT, TAM_SIGHT);
    renderer.Copy(sight, sprite, dst);
}

std::vector<int> Dibujador::separar_digitos(int n) {

    std::vector<int> digitos;
    if (n == 0) {
        digitos.push_back(0);
        return digitos;
    }

    while (n > 0) {
        int digito = n % 10;
        digitos.insert(digitos.begin(), digito);  // Insertar al comienzo
        n /= 10;
    }

    return digitos;
}

void Dibujador::dibujar_salud() {

    Rect sprite_salud(sprites_simbolos_hud[SALUD]);
    Rect salud_dst(0, ALTO_MIN - TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprite_salud, salud_dst);

    std::vector<int> digitos_salud = separar_digitos(100);
    int cant_digitos_salud = static_cast<int>(digitos_salud.size());

    for(int i = 0; i < cant_digitos_salud; i++){

        Rect sprite_digito(sprites_numeros_hud[digitos_salud[i]]);
        int x = TAM_SIMBOLOS_HUD + i * (ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD));
        Rect dst(x, ALTO_MIN - TAM_SIMBOLOS_HUD, ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD), TAM_SIMBOLOS_HUD);
        renderer.Copy(numeros_hud, sprite_digito, dst);
    }

}

void Dibujador::dibujar_tiempo() {

    Rect sprite_tiempo(sprites_simbolos_hud[TIEMPO]);
    Rect tiempo_dst((ANCHO_MIN / 2) - OFFSET_TIEMPO, ALTO_MIN - TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprite_tiempo, tiempo_dst);

    //Falta el tiempo
}

void Dibujador::dibujar_saldo() {

    std::vector<int> digitos_dinero = separar_digitos(16000);
    int cant_digitos_dinero = static_cast<int>(digitos_dinero.size());
    int x;
    for(int i = 0; i < cant_digitos_dinero; i++){
        Rect sprite_digito(sprites_numeros_hud[digitos_dinero[cant_digitos_dinero - i - 1]]);
        x = ANCHO_MIN - (ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD)) * (i + 1);
        Rect dst(x , ALTO_MIN - OFFSET_SALDO * TAM_SIMBOLOS_HUD, ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD), TAM_SIMBOLOS_HUD);
        renderer.Copy(numeros_hud, sprite_digito, dst);
    }
    Rect sprite_saldo(sprites_simbolos_hud[SALDO]);
    sprite_saldo.x++; // Sale la linea del sprite de al lado si no
    x -= TAM_SIMBOLOS_HUD; 
    Rect dst(x, ALTO_MIN - OFFSET_SALDO * TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprite_saldo, dst);

}

void Dibujador::dibujar_balas_hud() {

    std::vector<int> digitos_balas = separar_digitos(30);
    int cant_digitos_balas = static_cast<int>(digitos_balas.size());

    for (int i = 0; i < cant_digitos_balas; i++){
        Rect sprite_digito(sprites_numeros_hud[digitos_balas[cant_digitos_balas - i - 1]]);
        int x = ANCHO_MIN - (ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD)) * (i + 1);
        Rect dst(x, ALTO_MIN - TAM_SIMBOLOS_HUD, ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD), TAM_SIMBOLOS_HUD);
        renderer.Copy(numeros_hud, sprite_digito, dst);
    }

}

void Dibujador::dibujar_hud() {

    simbolos_hud.SetColorMod(255, 165, 0);
    simbolos_hud.SetAlphaMod(128);
    numeros_hud.SetColorMod(255, 165, 0);
    numeros_hud.SetAlphaMod(128);
    
    dibujar_salud();
    dibujar_tiempo();
    dibujar_balas_hud();
    dibujar_saldo();
}

void Dibujador::renderizar(Snapshot* snapshot/*, bool &jugador_activo*/)
{
    this->snapshot = snapshot;
    renderer.Clear();
    dibujar_fondo();
    dibujar_jugadores();
    dibujar_sight();
    dibujar_hud();
    
    renderer.Present();
}


Dibujador::~Dibujador()
{
}
