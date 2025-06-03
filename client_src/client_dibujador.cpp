#include "client_dibujador.h"
#include <thread>
#include <iostream> 

#define TAM_SIGHT 46
#define TAM_BALA 8
#define TAM_PLAYER 60
#define TAM_SIMBOLOS_HUD 50
#define DESFASE_ANGULO 90
#define ANCHO_NUMEROS_HUD 48
#define OFFSET_TIEMPO 130
#define OFFSET_SALDO 2.1
#define CANT_SPRITESHEETS_PLAYER 4

Dibujador::Dibujador(const int id, Renderer& renderer) : 
    client_id(id),
    renderer(renderer),
    parseador(),
    snapshot(nullptr),
    fondo(Texture(renderer, Surface(IMG_Load("client_src/gfx/backgrounds/dust.png")))),
    balas(Texture(renderer, Surface(IMG_Load("client_src/gfx/shells.png")))),
    ak47(Texture(renderer, Surface(IMG_Load("client_src/gfx/weapons/ak47.bmp")))),
    awp(Texture(renderer, Surface(IMG_Load("client_src/gfx/weapons/awp.bmp")))),
    bomba(Texture(renderer, Surface(IMG_Load("client_src/gfx/weapons/bomb.bmp")))),
    cuchillo(Texture(renderer, Surface(IMG_Load("client_src/gfx/weapons/knife.bmp")))),
    dropped_bomb(Texture(renderer, Surface(IMG_Load("client_src/gfx/weapons/bomb_d.bmp")))),
    glock(Texture(renderer, Surface(IMG_Load("client_src/gfx/weapons/glock.bmp")))),
    m3(Texture(renderer, Surface(IMG_Load("client_src/gfx/weapons/m3.bmp")))),
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
        for (int i = 1; i <= CANT_SPRITESHEETS_PLAYER; ++i) {
            std::string path = "client_src/gfx/player/ct" + std::to_string(i) + ".bmp";
            textures.emplace_back(renderer, Surface(IMG_Load(path.c_str())));
        }
        return textures;
    }()),
    tt_players([&renderer]() {
        std::vector<SDL2pp::Texture> textures;
        for (int i = 1; i <= CANT_SPRITESHEETS_PLAYER; ++i) {
            std::string path = "client_src/gfx/player/t" + std::to_string(i) + ".bmp";
            textures.emplace_back(renderer, Surface(IMG_Load(path.c_str())));
        }
        return textures;
    }()),
    sprite_arma(parseador.obtener_sprite_arma()),
    sprite_bala(parseador.obtener_sprite_bala()),
    sprites_player(parseador.obtener_sprites_jugador()),
    sprites_player_legs(parseador.obtener_sprites_pies_jugador()),
    sprites_simbolos_hud(parseador.obtener_sprites_simbolos_hud()),
    sprites_numeros_hud(parseador.obtener_sprites_numeros_hud())
    {}

void Dibujador::convertir_coordenadas(float &x, float &y) {
    x = x;
    y = ALTO_MIN - y;
}

float Dibujador::convertir_angulo(float angulo){
    return 360.0f - angulo + DESFASE_ANGULO;
}

void Dibujador::dibujar_jugadores() {
    for(const InfoJugador& jugador : snapshot->info_jugadores){
        
        float x_pixel = jugador.pos_x;
        float y_pixel = jugador.pos_y;
        convertir_coordenadas(x_pixel, y_pixel);
        float angulo_sdl = convertir_angulo(jugador.angulo);

      //dibujar_pies(x_pixel, y_pixel, angulo_sdl);
        enum SkinTipos skin = jugador.skin_tipo;
        dibujar_cuerpo(x_pixel, y_pixel, angulo_sdl, skin);
        dibujar_arma(x_pixel, y_pixel, angulo_sdl);
    }
}

void Dibujador::dibujar_fondo() {
    Rect dst(Rect(0, 0, ANCHO_MIN, ALTO_MIN));
    renderer.Copy(fondo, NullOpt, dst);
}

void Dibujador::dibujar_balas() {
    for (const InfoMunicion& bala : snapshot->balas_disparadas){
        float x_pixel = bala.pos_x;
        float y_pixel = bala.pos_y;
        std::cout << x_pixel << ":" << y_pixel << ":" << bala.angulo_disparo<< std::endl; 
        convertir_coordenadas(x_pixel, y_pixel);
        float angulo_bala = convertir_angulo(bala.angulo_disparo);
        SDL_FRect dst {x_pixel - TAM_PLAYER / 2, y_pixel - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
        SDL_FPoint center = {TAM_PLAYER / 2, TAM_PLAYER / 2};
        SDL_RenderCopyExF(renderer.Get(), balas.Get(), &sprite_bala, &dst, angulo_bala, &center, SDL_FLIP_NONE);
    }
}

void Dibujador::dibujar_cuerpo(float x, float y, float angulo, enum SkinTipos skin) {


    SDL_FRect dst {x - TAM_PLAYER / 2, y - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
    SDL_Rect sprite = sprites_player[ARMADO];
    SDL_FPoint center = {TAM_PLAYER / 2, TAM_PLAYER / 2};
    if (skin < 4) // CT players
        SDL_RenderCopyExF(renderer.Get(), ct_players[skin].Get(), &sprite, &dst, angulo, &center, SDL_FLIP_NONE);
    else // TT players
        SDL_RenderCopyExF(renderer.Get(), tt_players[skin-3].Get(), &sprite, &dst, angulo, &center, SDL_FLIP_NONE);
}

void Dibujador::dibujar_pies(float x, float y, float angulo) {
    Uint32 current_ticks = SDL_GetTicks();
    int i = (current_ticks / 10) % sprites_player_legs.size();
    SDL_Rect sprite_actual = sprites_player_legs[i];   
    SDL_FRect dst {x - TAM_PLAYER / 2, y - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
    SDL_FPoint center = {TAM_PLAYER, TAM_PLAYER};
    SDL_RenderCopyExF(renderer.Get(), player_legs.Get(), &sprite_actual, &dst, angulo, &center, SDL_FLIP_NONE);
}

void Dibujador::dibujar_arma(float x, float y, float angulo) {

    SDL_FRect dst {x - TAM_PLAYER / 2, (y - TAM_PLAYER / 5) - TAM_PLAYER, TAM_PLAYER, TAM_PLAYER};
    SDL_FPoint center = {x - dst.x, y - dst.y};
    SDL_RenderCopyExF(renderer.Get(), ak47.Get(), &sprite_arma, &dst, angulo, &center, SDL_FLIP_NONE);
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
        digitos.insert(digitos.begin(), digito);
        n /= 10;
    }

    return digitos;
}

void Dibujador::dibujar_salud(int salud) {

    std::vector<int> digitos_salud = separar_digitos(salud);
    int cant_digitos_salud = static_cast<int>(digitos_salud.size());

    Rect sprite_salud(sprites_simbolos_hud[SALUD]);
    Rect salud_dst(0, ALTO_MIN - TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprite_salud, salud_dst);

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

void Dibujador::dibujar_saldo(int saldo) {

    std::vector<int> digitos_dinero = separar_digitos(saldo);
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

void Dibujador::dibujar_balas_hud(int balas) {

    std::vector<int> digitos_balas = separar_digitos(balas);
    int cant_digitos_balas = static_cast<int>(digitos_balas.size());

    for (int i = 0; i < cant_digitos_balas; i++){
        Rect sprite_digito(sprites_numeros_hud[digitos_balas[cant_digitos_balas - i - 1]]);
        int x = ANCHO_MIN - (ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD)) * (i + 1);
        Rect dst(x, ALTO_MIN - TAM_SIMBOLOS_HUD, ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD), TAM_SIMBOLOS_HUD);
        renderer.Copy(numeros_hud, sprite_digito, dst);
    }

}

void Dibujador::dibujar_hud() {

    simbolos_hud.SetColorMod(29, 140, 31);
    simbolos_hud.SetAlphaMod(128);
    numeros_hud.SetColorMod(29, 140, 31);
    numeros_hud.SetAlphaMod(128);
    
    dibujar_salud(snapshot->getJugadorPorId(client_id)->vida);
    dibujar_tiempo();
    dibujar_balas_hud(30);
    dibujar_saldo(snapshot->getJugadorPorId(client_id)->dinero);
}

void Dibujador::renderizar(Snapshot* snapshot/*, bool &jugador_activo*/)
{
    this->snapshot = snapshot;
    renderer.Clear();
    dibujar_fondo();
    dibujar_balas();
    dibujar_jugadores();
    dibujar_sight();
    dibujar_hud();
    
    renderer.Present();
}


Dibujador::~Dibujador()
{
}
