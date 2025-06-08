#include "client_dibujador.h"
#include <thread>
#include <iostream> 

#define TAM_BALA 8
#define TAM_SIGHT 46
#define ESCALA_LETRA_GRANDE 0.06
#define ESCALA_LETRA_CHICA 0.02
#define ESCALA_ANCHO_ARMAS 0.275
#define ESCALA_ALTO_ARMAS 0.089
#define TAM_PLAYER 60
#define TAM_SIMBOLOS_HUD 50
#define DESFASE_ANGULO 90
#define ANCHO_NUMEROS_HUD 48
#define OFFSET_TIEMPO 130
#define OFFSET_CS2D 20
#define OFFSET_NOMBRE_ARMAS 40
#define ANCHO_CS2D 96
#define ALTO_CS2D 40
#define OFFSET_MERCADO 100
#define ESPACIO_ENTRE_ITEMS 20
#define OFFSET_SALDO 2.1
#define CANT_ITEMS_MERCADO 6
#define CANT_SPRITESHEETS_PLAYER 4

Dibujador::Dibujador(const int id, Renderer& renderer, std::vector<ElementoMapa> elementos, EventHandler& handler, Queue<Snapshot>& cola_recibidor) : 
    client_id(id),
    renderer(renderer),
    eventHandler(handler),
    cola_recibidor(cola_recibidor),
    elementos(elementos),
    parseador(),
    snapshot(),
    fuente("client_src/gfx/fonts/sourcesans.ttf", ALTO_MIN * ESCALA_LETRA_GRANDE),
    fuenteChica("client_src/gfx/fonts/sourcesans.ttf", ALTO_MIN * ESCALA_LETRA_CHICA),
    amarillo(Color(255, 255, 0)),
    fondo_mercado([&renderer]() {
        SDL_Surface* rawSurface = SDL_CreateRGBSurfaceWithFormat(0, 100, 100, 32, SDL_PIXELFORMAT_RGBA8888);
        Surface surface(rawSurface);
        Uint32 negroConAlpha = SDL_MapRGBA(surface.Get()->format, 0, 0, 0, 180);
        surface.FillRect(NullOpt, negroConAlpha);
		return Texture(renderer, surface);
    }()),
    balas(Texture(renderer, Surface(IMG_Load("client_src/gfx/shells.png")))),  
    cs2d(Texture(renderer, Surface(IMG_Load("client_src/gfx/gametitle.png")))),
    dropped_bomb(Texture(renderer, Surface(IMG_Load("client_src/gfx/weapons/bomb_d.bmp")))),  
    player_legs(Texture(renderer, Surface(IMG_Load("client_src/gfx/player/legs.bmp")))),
    simbolos_hud([&renderer]() {
        Surface s(IMG_Load("client_src/gfx/hud_symbols.bmp"));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 0, 0, 0));
        Texture t(renderer, s);
        t.SetAlphaMod(128);
        t.SetColorMod(29, 140, 31);
        return t;
    }()),
    numeros_hud([&renderer]() {
        Surface s(IMG_Load("client_src/gfx/hud_nums.bmp"));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 0, 0, 0));
        Texture t(renderer, s);
        t.SetAlphaMod(128);
        t.SetColorMod(29, 140, 31);
        return t;
    }()),
    sight([&renderer]() {
        Surface s(IMG_Load("client_src/gfx/pointer.bmp"));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 255, 0, 255));
        return Texture(renderer, s);
    }()),
    armas([&renderer]() {
        std::vector<SDL2pp::Texture> textures;
        textures.emplace_back(renderer, Surface(IMG_Load("client_src/gfx/weapons/knife.bmp")));
        textures.emplace_back(renderer, Surface(IMG_Load("client_src/gfx/weapons/glock.bmp")));
        textures.emplace_back(renderer, Surface(IMG_Load("client_src/gfx/weapons/ak47.bmp")));
        textures.emplace_back(renderer, Surface(IMG_Load("client_src/gfx/weapons/m3.bmp")));
        textures.emplace_back(renderer, Surface(IMG_Load("client_src/gfx/weapons/awp.bmp")));
        textures.emplace_back(renderer, Surface(IMG_Load("client_src/gfx/weapons/bomb.bmp")));
        return textures;
    }()),
    armas_mercado([&renderer]() {
        
        Surface ak47(IMG_Load("client_src/gfx/weapons/ak47_m.bmp"));
        ak47.SetColorKey(true, SDL_MapRGB(ak47.Get()->format, 255, 0, 255));

        Surface m3(IMG_Load("client_src/gfx/weapons/m3_m.bmp"));
        m3.SetColorKey(true, SDL_MapRGB(m3.Get()->format, 255, 0, 255));

        Surface awp(IMG_Load("client_src/gfx/weapons/awp_m.bmp"));
        awp.SetColorKey(true, SDL_MapRGB(awp.Get()->format, 255, 0, 255));

        std::vector<SDL2pp::Texture> textures;
        textures.emplace_back(renderer, ak47);
        textures.emplace_back(renderer, m3);
        textures.emplace_back(renderer, awp);
        return textures;
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
    sprite_sight(parseador.obtener_sprite_sight()),
    sprites_player(parseador.obtener_sprites_jugador()),
    sprites_player_legs(parseador.obtener_sprites_pies_jugador()),
    sprites_simbolos_hud(parseador.obtener_sprites_simbolos_hud()),
    sprites_numeros_hud(parseador.obtener_sprites_numeros_hud())
    {
    }

float Dibujador::convertir_angulo(float angulo){
    return 360.0f - angulo + DESFASE_ANGULO;
}

void Dibujador::convertir_a_pantalla(float pos_x, float pos_y, float& x_pixel, float& y_pixel) {
    const InfoJugador* jugador_principal = snapshot.getJugadorPorId(client_id);
    if (!jugador_principal) return;

    float centro_x = ANCHO_MIN / 2.0f;
    float centro_y = ALTO_MIN / 2.0f;

    x_pixel = pos_x - jugador_principal->pos_x + centro_x;
    y_pixel = - pos_y + jugador_principal->pos_y + centro_y;
}


void Dibujador::dibujar_jugadores() {

    for(const InfoJugador& jugador : snapshot.info_jugadores){
        float x_pixel, y_pixel;
        convertir_a_pantalla(jugador.pos_x, jugador.pos_y, x_pixel, y_pixel);
        
        float angulo_sdl = convertir_angulo(jugador.angulo);
        
        if(jugador.esta_moviendose)
            dibujar_pies(x_pixel, y_pixel, angulo_sdl);
    
        enum SkinTipos skin = jugador.skin_tipo;
        enum ArmaEnMano arma = jugador.arma_en_mano;
        dibujar_cuerpo(x_pixel, y_pixel, angulo_sdl, skin, arma);
        dibujar_arma(x_pixel, y_pixel, angulo_sdl, arma);
    }
}

void Dibujador::dibujar_fondo(const ElementoMapa& elemento){
    renderer.Copy(*elemento.texture, NullOpt, elemento.dst);
}

void Dibujador::dibujar_balas() {
    
    for (const InfoMunicion& bala : snapshot.balas_disparadas){
        
        float x_pixel, y_pixel;
        convertir_a_pantalla(bala.pos_x, bala.pos_y, x_pixel, y_pixel);
        float angulo_bala = convertir_angulo(bala.angulo_disparo);
        SDL_FRect dst {x_pixel - TAM_PLAYER / 2, y_pixel - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
        SDL_FPoint center = {TAM_PLAYER / 2, TAM_PLAYER / 2};
        SDL_RenderCopyExF(renderer.Get(), balas.Get(), &sprite_bala, &dst, angulo_bala, &center, SDL_FLIP_NONE);
    }
}

void Dibujador::dibujar_cuerpo(float x, float y, float angulo, enum SkinTipos skin, enum ArmaEnMano arma) {


    SDL_FRect dst {x - TAM_PLAYER / 2, y - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
    SDL_Rect sprite;
    if(arma == CUCHILLO)
        sprite = sprites_player[MANO_IZQ_CUCHILLO];
    else
        sprite = sprites_player[ARMADO];

    SDL_FPoint center = {TAM_PLAYER / 2, TAM_PLAYER / 2};
    if (skin < PHEONIX) // CT players
        SDL_RenderCopyExF(renderer.Get(), ct_players[skin].Get(), &sprite, &dst, angulo, &center, SDL_FLIP_NONE);
    else // TT players
        SDL_RenderCopyExF(renderer.Get(), tt_players[skin-3].Get(), &sprite, &dst, angulo, &center, SDL_FLIP_NONE);
}

void Dibujador::dibujar_pies(float x, float y, float angulo) {
    Uint32 current_ticks = SDL_GetTicks();
    int i = (current_ticks / 100) % sprites_player_legs.size();
    SDL_Rect sprite_actual = sprites_player_legs[i];   
    SDL_FRect dst {x - TAM_PLAYER / 2, y - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
    SDL_FPoint center = {TAM_PLAYER / 2, TAM_PLAYER / 2};
    SDL_RenderCopyExF(renderer.Get(), player_legs.Get(), &sprite_actual, &dst, angulo, &center, SDL_FLIP_NONE);
}

void Dibujador::dibujar_arma(float x, float y, float angulo, enum ArmaEnMano arma_actual) {

    SDL_FRect dst;
    if(arma_actual == CUCHILLO)
        dst = {x - TAM_PLAYER / 2, (y - TAM_PLAYER / 14) - TAM_PLAYER, TAM_PLAYER, TAM_PLAYER};
    else
        dst = {x - TAM_PLAYER / 2, (y - TAM_PLAYER / 6) - TAM_PLAYER, TAM_PLAYER, TAM_PLAYER};
    SDL_FPoint center = {x - dst.x, y - dst.y};
    SDL_RenderCopyExF(renderer.Get(), armas[arma_actual].Get(), &sprite_arma, &dst, angulo, &center, SDL_FLIP_NONE);
}

void Dibujador::dibujar_sight() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    Rect dst(mouseX - TAM_SIGHT / 2, mouseY - TAM_SIGHT / 2, TAM_SIGHT, TAM_SIGHT);
    renderer.Copy(sight, sprite_sight, dst);
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
    
    dibujar_salud(snapshot.getJugadorPorId(client_id)->vida);
    dibujar_tiempo();
    dibujar_balas_hud(30);
    dibujar_saldo(snapshot.getJugadorPorId(client_id)->dinero);
}


Texture Dibujador::crearTextoArma(std::string nombre, int precio) {
    std::string texto = nombre + "$" + std::to_string(precio);
    return Texture(renderer, fuente.RenderText_Blended(texto, amarillo));
}


void Dibujador::dibujar_mercado() {

    int anchoCuadro = ANCHO_MIN - OFFSET_MERCADO;
    int altoCuadro = ALTO_MIN - OFFSET_MERCADO;
    int x = (ANCHO_MIN - anchoCuadro) / 2;
    int y = (ALTO_MIN - altoCuadro) / 2;

    renderer.Copy(fondo_mercado, NullOpt, Rect(x, y, anchoCuadro, altoCuadro));
    renderer.Copy(cs2d, NullOpt, Rect(x + OFFSET_CS2D, y + OFFSET_CS2D, ANCHO_CS2D, ALTO_CS2D));
    Texture comprarArmas(renderer, fuente.RenderText_Blended("Comprar Armas", amarillo));
    renderer.Copy(comprarArmas, NullOpt, Rect(x + 130, y + 10,  comprarArmas.GetWidth(), comprarArmas.GetHeight()));
    
    std::vector <Texture> textos;
    textos.push_back(crearTextoArma("[1] AK-47       ", 100));
    textos.push_back(crearTextoArma("[2] M3             ", 100));
    textos.push_back(crearTextoArma("[3] AWP          ", 100));
    Texture primaria(renderer, fuente.RenderText_Blended("[,] Balas de arma primaria", amarillo));
    Texture secundaria(renderer, fuente.RenderText_Blended("[.] Balas de arma secundaria", amarillo));
    Texture salir(renderer, fuente.RenderText_Blended("[ESC] Salir", amarillo));

    int alto_total = 0;
    for (int i = 0; i < CANT_ITEMS_MERCADO; i++) {
        if (i == 3) {
            alto_total += primaria.GetHeight();
        } else if (i == 4) {
            alto_total += secundaria.GetHeight();
        } else if (i == 5){
            alto_total += salir.GetHeight();
        } else {
            int alto_texto = textos[i].GetHeight();
            int alto_imagen = armas_mercado[i].GetHeight();
            alto_total += std::max(alto_texto, alto_imagen);
        }
        if (i != CANT_ITEMS_MERCADO - 1) {
            alto_total += ESPACIO_ENTRE_ITEMS;
        }
    }

    int y_inicial = (ALTO_MIN - alto_total) / 2;
    int y_pos = y_inicial;

    for (int i = 0; i < CANT_ITEMS_MERCADO; i++) {
        if (i == 3) {
            SDL_Rect dst_primaria{x + OFFSET_NOMBRE_ARMAS, y_pos, primaria.GetWidth(), primaria.GetHeight()};
            renderer.Copy(primaria, NullOpt, dst_primaria);
            y_pos += primaria.GetHeight() + ESPACIO_ENTRE_ITEMS;
            continue;
        }

        if (i == 4) {
            SDL_Rect dst_secundaria{x + OFFSET_NOMBRE_ARMAS, y_pos, secundaria.GetWidth(), secundaria.GetHeight()};
            renderer.Copy(secundaria, NullOpt, dst_secundaria);
            y_pos += secundaria.GetHeight() + ESPACIO_ENTRE_ITEMS;
            continue;
        }

        if (i == 5) {
            SDL_Rect dst_salir{x + OFFSET_NOMBRE_ARMAS, y_pos, salir.GetWidth(), salir.GetHeight()};
            renderer.Copy(salir, NullOpt, dst_salir);
            y_pos += secundaria.GetHeight() + ESPACIO_ENTRE_ITEMS;
            continue;
        }

        SDL_Rect dst_texto = {x + OFFSET_NOMBRE_ARMAS, y_pos, textos[i].GetWidth(), textos[i].GetHeight()};
        renderer.Copy(textos[i], NullOpt, dst_texto);

        SDL_Rect dst_arma = {
            ANCHO_MIN / 2 + 30,
            y_pos,
            static_cast<int>(ANCHO_MIN * ESCALA_ANCHO_ARMAS),
            static_cast<int>(ALTO_MIN * ESCALA_ALTO_ARMAS)
        };
        renderer.Copy(armas_mercado[i], NullOpt, dst_arma);

        int alto_texto = textos[i].GetHeight();
        int alto_arma = dst_arma.h;
        y_pos += std::max(alto_texto, alto_arma) + ESPACIO_ENTRE_ITEMS;
    }
}

void Dibujador::dibujar_mapa() {
    InfoJugador* jugador_principal = snapshot.getJugadorPorId(client_id);
    float centro_x = ANCHO_MIN / 2.0f;
    float centro_y = ALTO_MIN / 2.0f;

    for (const ElementoMapa& elemento : elementos) {
        if(elemento.tipo == FONDO){
            dibujar_fondo(elemento);
        }
        else if(elemento.tipo == OBSTACULO){

            float pantalla_x = elemento.dst.x - jugador_principal->pos_x + centro_x;
            float pantalla_y = elemento.dst.y - 1920 +jugador_principal->pos_y + centro_y - 64; // YA ESTA EN COORDENADA SDL

            if (pantalla_x + elemento.dst.w >= 0 && pantalla_x <= ANCHO_MIN &&
                pantalla_y + elemento.dst.h >= 0 && pantalla_y <= ALTO_MIN) {
                renderer.Copy(*elemento.texture, NullOpt, Rect(pantalla_x, pantalla_y, elemento.dst.w, elemento.dst.h));
            }

        }
    }
}

void Dibujador::renderizar()
{
    Snapshot snapshotActual;

    while (cola_recibidor.try_pop(snapshotActual)) {
        snapshot = snapshotActual;
    }
    renderer.Clear();
    dibujar_mapa();
    dibujar_balas();
    dibujar_jugadores();
    dibujar_sight();
    dibujar_hud();
    if(eventHandler.mercadoAbierto())
        dibujar_mercado();
    renderer.Present();
}


Dibujador::~Dibujador()
{
}
