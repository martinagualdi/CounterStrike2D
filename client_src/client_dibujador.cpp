#include "client_dibujador.h"
#include "../common_src/ruta_base.h"
#include <thread>
#include <iostream> 

#define TAM_BALA 8
#define TAM_SIGHT 46
#define ESCALA_LETRA_GRANDE 0.06
#define ESCALA_LETRA_CHICA 0.03
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
#define OFFSET_TRANSPARENTE 100
#define ESPACIO_ENTRE_ITEMS 20
#define OFFSET_SALDO 2.1
#define CANT_ARMAS_MERCADO 3
#define CANT_SKINS_PLAYER 4
#define FADE_SPEED 100.0f

Dibujador::Dibujador(const int id, Renderer& renderer, struct Mapa mapa, EventHandler& handler, Queue<Snapshot>& cola_recibidor) : 
    client_id(id),
    renderer(renderer),
    eventHandler(handler),
    cola_recibidor(cola_recibidor),
    mapa(mapa),
    parseador(),
    snapshot(),
    estado_bomba_anterior(SIN_PLANTAR),
    explosion_en_progreso(false),
    explosion_alpha(0.0f),
    explosion_last_ticks(0),
    fuente(RUTA_IMAGENES("fonts/sourcesans.ttf"), ALTO_MIN * ESCALA_LETRA_GRANDE),
    fuenteChica(RUTA_IMAGENES("fonts/sourcesans.ttf"), ALTO_MIN * ESCALA_LETRA_CHICA),
    amarillo(Color(255, 255, 0)),
    blanco(Color(255,255,255)),
    verde(Color(100,220,100)),
    rojo(Color(200,80,80)),
    amarillento(Color(240,180,50)),
    celeste(Color(80,200,255)),
    mensaje_bomba_plantada(renderer, fuenteChica.RenderText_Blended("¡La bomba ha sido plantada!", amarillo)),
    mantenga_presionado_activar(renderer, fuenteChica.RenderText_Blended
    ("Mantenga presionado hasta finalizar el activado", amarillo)),
    mantenga_presionado_desactivar(renderer, fuenteChica.RenderText_Blended
    ("Mantenga presionado hasta finalizar el desactivado", amarillo)),
    fondo_transparente([&renderer]() {
        SDL_Surface* rawSurface = SDL_CreateRGBSurfaceWithFormat(0, 100, 100, 32, SDL_PIXELFORMAT_RGBA8888);
        Surface surface(rawSurface);
        Uint32 negroConAlpha = SDL_MapRGBA(surface.Get()->format, 0, 0, 0, 180);
        surface.FillRect(NullOpt, negroConAlpha);
        return Texture(renderer, surface);
    }()),
    balas(Texture(renderer, Surface(IMG_Load(RUTA_IMAGENES("shells.png").c_str())))),
    cs2d(Texture(renderer, Surface(IMG_Load(RUTA_IMAGENES("gametitle.png").c_str())))),
    player_legs(Texture(renderer, Surface(IMG_Load(RUTA_IMAGENES("player/legs.bmp").c_str())))),
    muerto(Texture(renderer, Surface(IMG_Load(RUTA_IMAGENES("player/muerto.png").c_str())))),
    simbolos_hud([&renderer]() {
        Surface s(IMG_Load(RUTA_IMAGENES("hud_symbols.bmp").c_str()));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 0, 0, 0));
        Texture t(renderer, s);
        t.SetAlphaMod(128);
        t.SetColorMod(29, 140, 31);
        return t;
    }()),
    numeros_hud([&renderer]() {
        Surface s(IMG_Load(RUTA_IMAGENES("hud_nums.bmp").c_str()));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 0, 0, 0));
        Texture t(renderer, s);
        t.SetAlphaMod(128);
        t.SetColorMod(29, 140, 31);
        return t;
    }()),
    sight([&renderer]() {
        Surface s(IMG_Load(RUTA_IMAGENES("pointer.bmp").c_str()));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 255, 0, 255));
        return Texture(renderer, s);
    }()),
    armas([&renderer]() {
        std::vector<SDL2pp::Texture> textures;
        textures.emplace_back(renderer, Surface(IMG_Load(RUTA_IMAGENES("weapons/ak47.bmp").c_str())));
        textures.emplace_back(renderer, Surface(IMG_Load(RUTA_IMAGENES("weapons/m3.bmp").c_str())));
        textures.emplace_back(renderer, Surface(IMG_Load(RUTA_IMAGENES("weapons/awp.bmp").c_str())));
        textures.emplace_back(renderer, Surface(IMG_Load(RUTA_IMAGENES("weapons/bomb.bmp").c_str())));
        textures.emplace_back(renderer, Surface(IMG_Load(RUTA_IMAGENES("weapons/knife.bmp").c_str())));
        textures.emplace_back(renderer, Surface(IMG_Load(RUTA_IMAGENES("weapons/glock.bmp").c_str())));
        return textures;
    }()),
    armas_mercado_y_tiradas([&renderer]() {
        Surface ak47(IMG_Load(RUTA_IMAGENES("weapons/ak47_m.bmp").c_str()));
        ak47.SetColorKey(true, SDL_MapRGB(ak47.Get()->format, 255, 0, 255));

        Surface m3(IMG_Load(RUTA_IMAGENES("weapons/m3_m.bmp").c_str()));
        m3.SetColorKey(true, SDL_MapRGB(m3.Get()->format, 255, 0, 255));

        Surface awp(IMG_Load(RUTA_IMAGENES("weapons/awp_m.bmp").c_str()));
        awp.SetColorKey(true, SDL_MapRGB(awp.Get()->format, 255, 0, 255));

        std::vector<SDL2pp::Texture> textures;
        textures.emplace_back(renderer, ak47);
        textures.emplace_back(renderer, m3);
        textures.emplace_back(renderer, awp);
        textures.emplace_back(renderer, Surface(IMG_Load(RUTA_IMAGENES("weapons/bomb_d.bmp").c_str())));
        return textures;
    }()),
    ct_players([&renderer]() {
        std::vector<SDL2pp::Texture> textures;
        for (int i = 1; i <= CANT_SKINS_PLAYER; ++i) {
            std::string path = RUTA_IMAGENES("player/ct" + std::to_string(i) + ".bmp");
            textures.emplace_back(renderer, Surface(IMG_Load(path.c_str())));
        }
        return textures;
    }()),
    tt_players([&renderer]() {
        std::vector<SDL2pp::Texture> textures;
        for (int i = 1; i <= CANT_SKINS_PLAYER; ++i) {
            std::string path = RUTA_IMAGENES("player/t" + std::to_string(i) + ".bmp");
            textures.emplace_back(renderer, Surface(IMG_Load(path.c_str())));
        }
        return textures;
    }()),
    textos_skin(),
    ct_nombres(),
    tt_nombres(),
    esperando_jugadores(),
    sprite_arma(parseador.obtener_sprite_arma()),
    sprite_bala(parseador.obtener_sprite_bala()),
    sprite_sight(parseador.obtener_sprite_sight()),
    sprites_player(parseador.obtener_sprites_jugador()),
    sprites_player_legs(parseador.obtener_sprites_pies_jugador()),
    sprites_simbolos_hud(parseador.obtener_sprites_simbolos_hud()),
    sprites_numeros_hud(parseador.obtener_sprites_numeros_hud()) 
{
    inicializar_textos();
}

void Dibujador::inicializar_textos() {
    textos_skin.emplace_back(renderer, fuente.RenderText_Blended("Seleccione el skin del personaje", amarillo));
    ct_nombres.emplace_back(renderer, fuente.RenderText_Blended("[1] Seal Force", amarillo));
    ct_nombres.emplace_back(renderer, fuente.RenderText_Blended("[2] German GSG-9", amarillo));
    ct_nombres.emplace_back(renderer, fuente.RenderText_Blended("[3] UK SAS", amarillo));
    ct_nombres.emplace_back(renderer, fuente.RenderText_Blended("[4] French GIGN", amarillo));
    tt_nombres.emplace_back(renderer, fuente.RenderText_Blended("[1] Pheonix", amarillo));
    tt_nombres.emplace_back(renderer, fuente.RenderText_Blended("[2] L337 Krew", amarillo));
    tt_nombres.emplace_back(renderer, fuente.RenderText_Blended("[3] Artic Avenger", amarillo));
    tt_nombres.emplace_back(renderer, fuente.RenderText_Blended("[4] Guerrilla", amarillo));

    std::string base("Esperando jugadores");
    for (int i = 0; i < 4; i++) {
        std::string puntos(i, '.');
        std::string texto = base + puntos;
        esperando_jugadores.emplace_back(renderer, fuenteChica.RenderText_Blended(texto, amarillo));
    }
    
    mensajes_ganadores.emplace_back(renderer, fuenteChica.RenderText_Blended("¡Los Counter-Terrorist han ganado!", amarillo));
    mensajes_ganadores.emplace_back(renderer, fuenteChica.RenderText_Blended("¡Los Terroristas han ganado!", amarillo));

    
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
        float x_pixel = 0.0f, y_pixel = 0.0f;
        convertir_a_pantalla(jugador.pos_x, jugador.pos_y, x_pixel, y_pixel);
        
        float angulo_sdl = convertir_angulo(jugador.angulo);
        
        if(!jugador.esta_vivo){
            dibujar_muerto(x_pixel, y_pixel);
            continue;
        }

        if(jugador.esta_moviendose)
            dibujar_pies(x_pixel, y_pixel, angulo_sdl);
    
        enum SkinTipos skin = jugador.skin_tipo;
        enum ArmaEnMano arma = jugador.arma_en_mano;
        enum Equipo equipo = jugador.equipo;
        dibujar_cuerpo(x_pixel, y_pixel, angulo_sdl, skin, arma, equipo);
        dibujar_arma(x_pixel, y_pixel, angulo_sdl, arma);
    }
}

void Dibujador::dibujar_muerto(int x_pixel, int y_pixel) {
    Rect dst(x_pixel - TAM_PLAYER / 2, y_pixel - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER);
    renderer.Copy(muerto, NullOpt, dst);
}

void Dibujador::dibujar_fondo(const ElementoMapa& elemento){
    renderer.Copy(*elemento.texture, NullOpt, elemento.dst);
}

void Dibujador::dibujar_balas() {
    
    for (const InfoMunicion& bala : snapshot.balas_disparadas){
        float x_pixel = 0.0f, y_pixel = 0.0f;
        convertir_a_pantalla(bala.pos_x, bala.pos_y, x_pixel, y_pixel);
        float angulo_bala = convertir_angulo(bala.angulo_disparo);
        SDL_FRect dst {x_pixel - TAM_PLAYER / 2, y_pixel - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
        SDL_FPoint center = {TAM_PLAYER / 2, TAM_PLAYER / 2};
        SDL_RenderCopyExF(renderer.Get(), balas.Get(), &sprite_bala, &dst, angulo_bala, &center, SDL_FLIP_NONE);
    }
}

void Dibujador::dibujar_cuerpo(float x, float y, float angulo, enum SkinTipos skin, enum ArmaEnMano arma, enum Equipo equipo) {

    SDL_FRect dst {x - TAM_PLAYER / 2, y - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
    SDL_Rect sprite;
    std::vector<Texture>& players = (equipo == CT) ? ct_players : tt_players;

    if(arma == CUCHILLO)
        sprite = sprites_player[MANO_IZQ_CUCHILLO];
    else if(arma == BOMBA_TT)
        sprite = sprites_player[DOS_MANOS];
    else
        sprite = sprites_player[ARMADO];

    SDL_FPoint center = {TAM_PLAYER / 2, TAM_PLAYER / 2};
    SDL_RenderCopyExF(renderer.Get(), players[skin].Get(), &sprite, &dst, angulo, &center, SDL_FLIP_NONE);
    
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
    else if(arma_actual == BOMBA_TT)
        dst = {x - TAM_PLAYER / 2  - TAM_PLAYER / 10 , (y - TAM_PLAYER / 6) - TAM_PLAYER, TAM_PLAYER, TAM_PLAYER};
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

std::vector<int> Dibujador::separar_digitos_tiempo(int n) {

    std::vector<int> digitos;

    bool agregar_cero = n < 10 ? true : false;

    if (n == 0) {
        digitos.push_back(0);
    }

    while (n > 0) {
        int digito = n % 10;
        digitos.insert(digitos.begin(), digito);
        n /= 10;
    }

    if(agregar_cero) digitos.insert(digitos.begin(), 0);    

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

void Dibujador::dibujar_tiempo(int tiempo_restante) {

    Rect sprite_reloj(sprites_simbolos_hud[TIEMPO]);
    Rect reloj_dst;
    reloj_dst.SetX((ANCHO_MIN / 2) - OFFSET_TIEMPO);
    reloj_dst.SetY(ALTO_MIN - TAM_SIMBOLOS_HUD);
    reloj_dst.SetW(TAM_SIMBOLOS_HUD);
    reloj_dst.SetH(TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprite_reloj, reloj_dst);
    
    int minutos = tiempo_restante / 60;
    int segundos = tiempo_restante % 60;
    
    std::vector<int> digitos_minutos = separar_digitos_tiempo(minutos);
    std::vector<int> digitos_segundos = separar_digitos_tiempo(segundos);

    int pos_x = ANCHO_MIN / 2 - OFFSET_TIEMPO + TAM_SIMBOLOS_HUD;

    Rect tiempo_dst;
    tiempo_dst.SetY(ALTO_MIN - TAM_SIMBOLOS_HUD);
    tiempo_dst.SetW(ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD));
    tiempo_dst.SetH(TAM_SIMBOLOS_HUD);

    for(int i = 0; i < (int)digitos_minutos.size(); i++){
        tiempo_dst.SetX(pos_x);
        Rect sprite_digito = sprites_numeros_hud[digitos_minutos[i]];
        renderer.Copy(numeros_hud, sprite_digito, tiempo_dst);
        pos_x += (ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD)); // desplazamiento
    }

    Rect dos_puntos_dst;
    dos_puntos_dst.SetX(pos_x);
    dos_puntos_dst.SetY(ALTO_MIN - TAM_SIMBOLOS_HUD);
    dos_puntos_dst.SetW(TAM_SIMBOLOS_HUD);
    dos_puntos_dst.SetH(TAM_SIMBOLOS_HUD);
    renderer.Copy(numeros_hud, sprites_numeros_hud[DOS_PUNTOS], dos_puntos_dst);
    pos_x += ANCHO_NUMEROS_HUD / 3;
    
    for(int i = 0; i < (int)digitos_segundos.size(); i++){
        tiempo_dst.SetX(pos_x);
        Rect sprite_digito = sprites_numeros_hud[digitos_segundos[i]];
        renderer.Copy(numeros_hud, sprite_digito, tiempo_dst);
        pos_x += (ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD));
    }

}

void Dibujador::dibujar_saldo(int saldo, bool arma_con_balas) {

    std::vector<int> digitos_dinero = separar_digitos(saldo);
    int cant_digitos_dinero = static_cast<int>(digitos_dinero.size());
    int x, y;
    
    if(arma_con_balas)
        y = ALTO_MIN - OFFSET_SALDO * TAM_SIMBOLOS_HUD;
    else
        y = ALTO_MIN - TAM_SIMBOLOS_HUD;

    for(int i = 0; i < cant_digitos_dinero; i++){
        Rect sprite_digito(sprites_numeros_hud[digitos_dinero[cant_digitos_dinero - i - 1]]);
        x = ANCHO_MIN - (ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD)) * (i + 1);
        Rect dst(x , y, ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD), TAM_SIMBOLOS_HUD);
        renderer.Copy(numeros_hud, sprite_digito, dst);
    }
    Rect sprite_saldo(sprites_simbolos_hud[SALDO]);
    sprite_saldo.x++; // Sale la linea del sprite de al lado si no
    x -= TAM_SIMBOLOS_HUD; 
    Rect dst(x, y, TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprite_saldo, dst);

}

void Dibujador::dibujar_balas_hud(int balas) {

    std::vector<int> digitos_balas = separar_digitos(balas);
    int cant_digitos_balas = static_cast<int>(digitos_balas.size());

    for (int i = 0; i < cant_digitos_balas; i++){
        Rect sprite_digito(sprites_numeros_hud[digitos_balas[cant_digitos_balas - i - 1]]);
        int x = ANCHO_MIN - (ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD)) * (i + 1);
        int y = ALTO_MIN - TAM_SIMBOLOS_HUD;
        int w = ANCHO_NUMEROS_HUD - (64 - TAM_SIMBOLOS_HUD);
        int h = TAM_SIMBOLOS_HUD;
        Rect dst(x, y, w, h);
        renderer.Copy(numeros_hud, sprite_digito, dst);
    }

}

Texture Dibujador::crearTextoArma(std::string nombre, int precio) {
    std::string texto = nombre + "$" + std::to_string(precio);
    return Texture(renderer, fuente.RenderText_Blended(texto, amarillo));
}

void Dibujador::dibujar_simbolo_mercado() {

    Rect dst;
    dst.SetX(static_cast<int>(ANCHO_MIN * 5/8));
    dst.SetY(ALTO_MIN - TAM_SIMBOLOS_HUD);
    dst.SetW(TAM_SIMBOLOS_HUD);
    dst.SetH(TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprites_simbolos_hud[MERCADO], dst);

}

void Dibujador::dibujar_simbolo_zona_detonar() {

    Rect dst;
    dst.SetX(static_cast<int>(ANCHO_MIN * 5/8) + TAM_SIMBOLOS_HUD);
    dst.SetY(ALTO_MIN - TAM_SIMBOLOS_HUD);
    dst.SetW(TAM_SIMBOLOS_HUD);
    dst.SetH(TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprites_simbolos_hud[ZONA_BOMBA], dst);

}

void Dibujador::dibujar_mantenga_presionado(bool activar) {

    Texture& mantenga_presionado = activar ? mantenga_presionado_activar 
    : mantenga_presionado_desactivar;

    int ancho_mensaje = mantenga_presionado.GetWidth();
    int alto_mensaje = mantenga_presionado.GetHeight();

    Rect dst;
    dst.SetX((ANCHO_MIN / 2) - ancho_mensaje / 2);
    dst.SetY((ALTO_MIN / 3) - alto_mensaje/ 2);
    dst.SetW(ancho_mensaje);
    dst.SetH(alto_mensaje);
    renderer.Copy(mantenga_presionado, NullOpt, dst);

}

void Dibujador::dibujar_mercado() {

    int anchoCuadro = ANCHO_MIN - OFFSET_TRANSPARENTE;
    int altoCuadro = ALTO_MIN - OFFSET_TRANSPARENTE;
    int x = (ANCHO_MIN - anchoCuadro) / 2;
    int y = (ALTO_MIN - altoCuadro) / 2;

    renderer.Copy(fondo_transparente, NullOpt, Rect(x, y, anchoCuadro, altoCuadro));
    renderer.Copy(cs2d, NullOpt, Rect(x + OFFSET_CS2D, y + OFFSET_CS2D, ANCHO_CS2D, ALTO_CS2D));
    Texture comprarArmas(renderer, fuente.RenderText_Blended("Comprar Armas", amarillo));
    renderer.Copy(comprarArmas, NullOpt, Rect(x + 130, y + 10,  comprarArmas.GetWidth(), comprarArmas.GetHeight()));
    
    std::vector <Texture> textos;
    textos.push_back(crearTextoArma("[1] AK-47       ", 150));
    textos.push_back(crearTextoArma("[2] M3             ", 100));
    textos.push_back(crearTextoArma("[3] AWP          ", 200));
    Texture primaria(renderer, fuente.RenderText_Blended("[,] Balas de arma primaria", amarillo));
    Texture secundaria(renderer, fuente.RenderText_Blended("[.] Balas de arma secundaria", amarillo));
    Texture salir(renderer, fuente.RenderText_Blended("[ESC] Salir", amarillo));

    int alto_total = 0;
    alto_total += primaria.GetHeight() + ESPACIO_ENTRE_ITEMS;
    alto_total += secundaria.GetHeight() + ESPACIO_ENTRE_ITEMS;
    alto_total += salir.GetHeight() + ESPACIO_ENTRE_ITEMS;
    for (int i = 0; i < CANT_ARMAS_MERCADO; i++) {
        int alto_texto = textos[i].GetHeight();
        int alto_imagen = armas_mercado_y_tiradas[i].GetHeight();
        alto_total += std::max(alto_texto, alto_imagen);
        if (i != CANT_ARMAS_MERCADO - 1) {
            alto_total += ESPACIO_ENTRE_ITEMS;
        }
    }

    int y_inicial = (ALTO_MIN - alto_total) / 2;
    int y_pos = y_inicial;

    for (int i = 0; i < CANT_ARMAS_MERCADO; i++) {

        Rect dst_texto(x + OFFSET_NOMBRE_ARMAS, y_pos, textos[i].GetWidth(), textos[i].GetHeight());
        renderer.Copy(textos[i], NullOpt, dst_texto);

        Rect dst_arma;
        dst_arma.SetX(ANCHO_MIN / 2 + 30);
        dst_arma.SetY(y_pos);
        dst_arma.SetW(ANCHO_MIN * ESCALA_ANCHO_ARMAS);
        dst_arma.SetH(ALTO_MIN * ESCALA_ALTO_ARMAS);
        renderer.Copy(armas_mercado_y_tiradas[i], NullOpt, dst_arma);

        int alto_texto = textos[i].GetHeight();
        int alto_arma = dst_arma.h;
        y_pos += std::max(alto_texto, alto_arma) + ESPACIO_ENTRE_ITEMS;
    }

    Rect dst_primaria(x + OFFSET_NOMBRE_ARMAS, y_pos, primaria.GetWidth(), primaria.GetHeight());
    renderer.Copy(primaria, NullOpt, dst_primaria);
    y_pos += primaria.GetHeight() + ESPACIO_ENTRE_ITEMS;

    Rect dst_secundaria(x + OFFSET_NOMBRE_ARMAS, y_pos, secundaria.GetWidth(), secundaria.GetHeight());
    renderer.Copy(secundaria, NullOpt, dst_secundaria);
    y_pos += secundaria.GetHeight() + ESPACIO_ENTRE_ITEMS;

    Rect dst_salir(x + OFFSET_NOMBRE_ARMAS, y_pos, salir.GetWidth(), salir.GetHeight());
    renderer.Copy(salir, NullOpt, dst_salir);
    y_pos += secundaria.GetHeight() + ESPACIO_ENTRE_ITEMS;

}

void Dibujador::dibujar_seleccionar_skin() {
    bool es_tt = snapshot.getJugadorPorId(client_id)->equipo;
    std::vector<Texture>& nombres = es_tt ? tt_nombres : ct_nombres;
    std::vector<Texture>& players = es_tt ? tt_players : ct_players;

    int anchoCuadro = ANCHO_MIN - OFFSET_TRANSPARENTE;
    int altoCuadro = ALTO_MIN - OFFSET_TRANSPARENTE;
    int x = (ANCHO_MIN - anchoCuadro) / 2;
    int y = (ALTO_MIN - altoCuadro) / 2;

    renderer.Copy(fondo_transparente, NullOpt, Rect(x, y, anchoCuadro, altoCuadro));
    renderer.Copy(cs2d, NullOpt, Rect(x + OFFSET_CS2D, y + OFFSET_CS2D, ANCHO_CS2D, ALTO_CS2D));
    Rect texto_dst(x + 130, y + 10,  textos_skin[0].GetWidth(), textos_skin[0].GetHeight());
    renderer.Copy(textos_skin[0], NullOpt, texto_dst);

    int alto_total = 0;
    for (int i = 0; i < CANT_SKINS_PLAYER; i++) {
        int alto_texto = nombres[i].GetHeight();
        int alto_imagen = static_cast<int>(TAM_PLAYER * 1.5);
        alto_total += std::max(alto_texto, alto_imagen);
        if (i != CANT_SKINS_PLAYER - 1) {
            alto_total += ESPACIO_ENTRE_ITEMS;
        }
    }

    int x_inicial = x + OFFSET_NOMBRE_ARMAS;
    int y_inicial = (ALTO_MIN - alto_total) / 2;
    int y_pos = y_inicial + 30;

    for(int i = 0; i < CANT_SKINS_PLAYER; i++){
        Rect dst_nombre(x_inicial, y_pos, nombres[i].GetWidth(), nombres[i].GetHeight());
        renderer.Copy(nombres[i], NullOpt, dst_nombre);

        int tam_skin = static_cast<int>(TAM_PLAYER * 1.5);
        Rect dst_skin(ANCHO_MIN / 2 + ANCHO_MIN/4, y_pos, tam_skin, tam_skin);
        renderer.Copy(players[i], sprites_player[DOS_MANOS], dst_skin);

        int alto_texto = nombres[i].GetHeight();
        int alto_skin = tam_skin;
        y_pos += std::max(alto_texto, alto_skin) + ESPACIO_ENTRE_ITEMS;

    }
}

void Dibujador::dibujar_armas_tiradas() {

    for(const InfoArmaEnSuelo& arma_tirada : snapshot.armas_sueltas){
        
        enum ArmaEnMano arma = arma_tirada.tipo_arma;
        int ancho_arma = armas_mercado_y_tiradas[arma].GetWidth();
        int alto_arma = armas_mercado_y_tiradas[arma].GetHeight();
        float x_pixel, y_pixel;
        convertir_a_pantalla(arma_tirada.pos_x, arma_tirada.pos_y, x_pixel, y_pixel);
        Rect dst;
        dst.SetX(x_pixel - ancho_arma / 2);
        dst.SetY(y_pixel - alto_arma / 2);
        dst.SetW(ancho_arma);
        dst.SetH(alto_arma);
        
        renderer.Copy(armas_mercado_y_tiradas[arma], NullOpt, dst);
       
    }

}

void Dibujador::dibujar_explosion_bomba() {

    enum EstadoBombaRonda estado_actual = snapshot.bomba_en_suelo.estado_bomba;

    if (estado_actual ==  DETONADA && estado_bomba_anterior != DETONADA) {
        explosion_en_progreso = true;
        explosion_alpha = 255.0f;
        explosion_last_ticks = SDL_GetTicks();
    }

    estado_bomba_anterior = estado_actual;

    if (explosion_en_progreso) {
        Uint32 ahora = SDL_GetTicks();
        float delta_time = (ahora - explosion_last_ticks) / 1000.0f;
        explosion_last_ticks = ahora;

        renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
        renderer.SetDrawColor(255, 255, 255, static_cast<Uint8>(explosion_alpha));
        renderer.FillRect(0, 0, ANCHO_MIN, ALTO_MIN);

        explosion_alpha -= FADE_SPEED * delta_time;
        if (explosion_alpha <= 0.0f) {
            explosion_alpha = 0.0f;
            explosion_en_progreso = false;
        }
    }

}

void Dibujador::dibujar_mensaje_ganador() {

    int ganador = static_cast<int>(snapshot.equipo_ganador);
    int ancho_mensaje = mensajes_ganadores[ganador].GetWidth();
    int alto_mensaje = mensajes_ganadores[ganador].GetHeight();

    Rect dst;
    dst.SetX((ANCHO_MIN / 2) - ancho_mensaje / 2);
    dst.SetY((ALTO_MIN / 3) - alto_mensaje/ 2);
    dst.SetW(ancho_mensaje);
    dst.SetH(alto_mensaje);
    renderer.Copy(mensajes_ganadores[ganador], NullOpt, dst);
}

void Dibujador::dibujar_mensaje_bomba_plantada() {

    int ancho_mensaje = mensaje_bomba_plantada.GetWidth();
    int alto_mensaje = mensaje_bomba_plantada.GetHeight();

    Rect dst;
    dst.SetX((ANCHO_MIN / 2) - ancho_mensaje / 2);
    dst.SetY((ALTO_MIN / 3) - alto_mensaje/ 2);
    dst.SetW(ancho_mensaje);
    dst.SetH(alto_mensaje);
    renderer.Copy(mensaje_bomba_plantada, NullOpt, dst);

}

void Dibujador::dibujar_bomba_plantada() {

    float x_logica = snapshot.bomba_en_suelo.pos_x;
    float y_logica = snapshot.bomba_en_suelo.pos_y;
    float x_pixel, y_pixel;
    convertir_a_pantalla(x_logica, y_logica, x_pixel, y_pixel);
    Rect dst;
    dst.SetX(x_pixel - TAM_PLAYER / 2);
    dst.SetY(y_pixel - TAM_PLAYER / 2);
    dst.SetW(TAM_PLAYER);
    dst.SetH(TAM_PLAYER);
        
    renderer.Copy(armas[BOMBA_TT], NullOpt, dst);
       
}

void Dibujador::dibujar_vision_de_campo() {

    const int ancho = ANCHO_MIN;
    const int alto = ALTO_MIN;
    const float angulo_fov = 90.0f;
    const float radio_cono = 1.5f * std::max(ancho, alto);
    const float radio_centro = 55.0f;
    const float direccion_centro = convertir_angulo(snapshot.getJugadorPorId(client_id)->angulo) - DESFASE_ANGULO;
    const Uint8 opacidad_fondo = 200;

    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, ancho, alto, 32, SDL_PIXELFORMAT_RGBA8888);
    if (!surf) throw std::runtime_error("No se pudo crear surface");

    Uint32 negroConAlpha = SDL_MapRGBA(surf->format, 0, 0, 0, opacidad_fondo);
    SDL_FillRect(surf, nullptr, negroConAlpha);

    int cx = ancho / 2;
    int cy = alto / 2;

    float desde = direccion_centro - angulo_fov / 2.0f;
    float hasta = direccion_centro + angulo_fov / 2.0f;

    auto normalizar = [](float ang) {
        while (ang < 0) ang += 360.0f;
        while (ang >= 360.0f) ang -= 360.0f;
        return ang;
    };

    desde = normalizar(desde);
    hasta = normalizar(hasta);

    for (int y = 0; y < alto; ++y) {
        for (int x = 0; x < ancho; ++x) {
            float dx = x - cx;
            float dy = y - cy;
            float dist = std::sqrt(dx*dx + dy*dy);
            bool en_circulo = (dist <= radio_centro);
            bool en_cono = false;
            if (dist <= radio_cono) {
                float ang = std::atan2(dy, dx) * 180.0f / M_PI;
                if (ang < 0) ang += 360.0f;
                if (desde < hasta)
                    en_cono = (ang >= desde && ang <= hasta);
                else
                    en_cono = (ang >= desde || ang <= hasta);
            }

            if (en_circulo || en_cono) {
                Uint32* pixel = (Uint32*)((Uint8*)surf->pixels + y*surf->pitch + x*4);
                *pixel = SDL_MapRGBA(surf->format, 0, 0, 0, 0); // transparente total
            }
        }
    }   

    Texture campo_de_vision(renderer, Surface(surf));
    campo_de_vision.SetBlendMode(SDL_BLENDMODE_BLEND);
    renderer.Copy(campo_de_vision, NullOpt, NullOpt);
}

void Dibujador::dibujar_estadisticas() {

    int ancho = 500;
    int altura_fila = 30;

    // Cantidad de filas: jugadores + títulos (ajustá +3/+6 según cuántos títulos/subtítulos dibujás)
    int filas = snapshot.info_jugadores.size() + 6; // 6 si sumás: 2 títulos de equipo, 2 filas de columnas, y algún espacio extra

    int alto_calculado = 40 + filas * altura_fila + 20; // 40 arriba, 20 abajo
    int alto_minimo = 250;
    int alto = std::max(alto_calculado, alto_minimo);

    int x = (ANCHO_MIN-ancho)/2;
    int y = (ALTO_MIN-alto)/2;

    int y_fila_inicial = y + 40;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(x, y, ancho, alto));
    
    // Info de partida
    std::string info_partida = 
        "Ronda: " + std::to_string(snapshot.rondas_info.ronda_actual) + 
        " / "     + std::to_string(snapshot.rondas_info.total_rondas) +
        "   CT: " + std::to_string(snapshot.rondas_info.rondas_ganadas_ct) +
        "   TT: " + std::to_string(snapshot.rondas_info.rondas_ganadas_tt);
    Texture txt_info(renderer, fuenteChica.RenderText_Blended(info_partida, blanco));
    Rect dst_info(x+20, y+10, txt_info.GetWidth(), txt_info.GetHeight());
    renderer.Copy(txt_info, NullOpt, dst_info);

    // Columnas
    std::vector<std::string> titulos = {"Jugador", "Estado", "Puntos", "Muertes"};
    std::vector<int> col_x = {x+20, x+230, x+320, x+400};
    int fila = 0;

    // --- Counter-Terrorists ---
    Texture t_ct(renderer, fuenteChica.RenderText_Blended("Counter-Terrorists", celeste));
    Rect dst_titulo_ct(x+20, y_fila_inicial+fila*altura_fila, t_ct.GetWidth(), t_ct.GetHeight());
    renderer.Copy(t_ct, NullOpt, dst_titulo_ct);
    fila++;

    // Títulos
    for (size_t i = 0; i < titulos.size(); ++i) {
        Texture txt_col(renderer, fuenteChica.RenderText_Blended(titulos[i], blanco));
        Rect dst_titulo(col_x[i], y_fila_inicial+fila*altura_fila, txt_col.GetWidth(), txt_col.GetHeight());
        renderer.Copy(txt_col, NullOpt, dst_titulo);
    }
    fila++;

    dibujar_estadisticas_jugador(col_x, y_fila_inicial, fila, altura_fila, CT);
    fila++; // Espacio entre equipos
    Texture t_tt(renderer, fuenteChica.RenderText_Blended("Terrorists", amarillento));
    Rect dst_titulo_tt(x+20, y_fila_inicial+fila*altura_fila, t_tt.GetWidth(), t_tt.GetHeight());
    renderer.Copy(t_tt, NullOpt, dst_titulo_tt);
    fila++;

    for (size_t i=0; i<titulos.size(); ++i) {
        Texture txt_col(renderer, fuenteChica.RenderText_Blended(titulos[i], blanco));
        Rect dst_txt_col(col_x[i], y_fila_inicial+fila*altura_fila, txt_col.GetWidth(), txt_col.GetHeight());
        renderer.Copy(txt_col, NullOpt, dst_txt_col);
    }
    fila++;

    dibujar_estadisticas_jugador(col_x, y_fila_inicial, fila, altura_fila, TT);

}

void Dibujador::dibujar_estadisticas_jugador(std::vector<int>& col_x, int& y_fila_inicial,
 int& fila, int& altura_fila, enum Equipo equipo) {

    for (const InfoJugador& jug : snapshot.info_jugadores) {
        if (jug.equipo != equipo) continue;

        Texture user(renderer, fuenteChica.RenderText_Blended(jug.nombre, blanco));
        Rect dst_username(col_x[0], y_fila_inicial + fila * altura_fila, user.GetWidth(), user.GetHeight());
        renderer.Copy(user, NullOpt, dst_username);

        std::string vivo = jug.esta_vivo ? "Vivo" : "Muerto";
        Color color_estado = jug.esta_vivo ? verde : rojo;
        Texture estado(renderer, fuenteChica.RenderText_Blended(vivo, color_estado));
        Rect dst_estado(col_x[1], y_fila_inicial + fila * altura_fila, estado.GetWidth(), estado.GetHeight());
        renderer.Copy(estado, NullOpt, dst_estado);

        Texture puntos(renderer, fuenteChica.RenderText_Blended(std::to_string(jug.eliminaciones_totales), blanco));
        Rect dst_puntos(col_x[2], y_fila_inicial + fila * altura_fila, puntos.GetWidth(), puntos.GetHeight());
        renderer.Copy(puntos, NullOpt, dst_puntos);

        Texture muertes(renderer, fuenteChica.RenderText_Blended(std::to_string(jug.muertes), blanco));
        Rect dst_muertes(col_x[3], y_fila_inicial + fila * altura_fila, muertes.GetWidth(), muertes.GetHeight());
        renderer.Copy(muertes, NullOpt, dst_muertes);

        fila++;
    }

}

void Dibujador::dibujar_mapa() {
    
    for (const ElementoMapa& elemento : mapa.elementos) {
        if(elemento.tipo == FONDO){
            dibujar_fondo(elemento);
        }
        else if(elemento.tipo == OBSTACULO || elemento.tipo == PISO){
            float x_pixel, y_pixel;
            convertir_a_pantalla(elemento.dst.x, -elemento.dst.y, x_pixel, y_pixel);
            if (x_pixel + elemento.dst.w >= 0 && x_pixel <= ANCHO_MIN &&
                y_pixel + elemento.dst.h >= 0 && y_pixel <= ALTO_MIN) {
                Rect dst(x_pixel, y_pixel, elemento.dst.w, elemento.dst.h);
                renderer.Copy(*elemento.texture, NullOpt, dst);
            }
        }
    }
}

void Dibujador::dibujar_esperando_jugadores() {
    
    Uint32 current_ticks = SDL_GetTicks();
    int i = (current_ticks / 500) % esperando_jugadores.size();
    
    int ancho_mensaje = esperando_jugadores[i].GetWidth();
    int alto_mensaje = esperando_jugadores[i].GetHeight();

    Rect dst_esperando;
    dst_esperando.SetX((ANCHO_MIN / 2) -  esperando_jugadores[0].GetWidth() / 2);
    dst_esperando.SetY((ALTO_MIN / 3) -  esperando_jugadores[0].GetWidth() / 2);
    dst_esperando.SetW(ancho_mensaje);
    dst_esperando.SetH(alto_mensaje);
    renderer.Copy(esperando_jugadores[i], NullOpt, dst_esperando);

}

void Dibujador::dibujar_hud() {
    
    const InfoJugador* jugador_principal = snapshot.getJugadorPorId(client_id);

    dibujar_salud(jugador_principal->vida);
    dibujar_tiempo(snapshot.tiempo_restante);
    if(jugador_principal->puede_comprar_ya) dibujar_simbolo_mercado();
    if(jugador_principal->tiene_bomba && jugador_principal->esta_en_zona_de_plantar)
        dibujar_simbolo_zona_detonar();
    if(jugador_principal->arma_en_mano != CUCHILLO){
        dibujar_balas_hud(jugador_principal->balas);
        dibujar_saldo(jugador_principal->dinero, true);
    }    
    else dibujar_saldo(jugador_principal->dinero, false);

}

void Dibujador::renderizar(Snapshot& snapshot)
{
    this->snapshot = snapshot;

    const InfoJugador* principal = snapshot.getJugadorPorId(client_id);
    if(!principal) return;

    renderer.Clear();
    dibujar_mapa();
    dibujar_armas_tiradas();
    if(snapshot.bomba_en_suelo.estado_bomba == PLANTADA) dibujar_bomba_plantada();
    dibujar_balas();
    dibujar_jugadores();
    dibujar_sight();
    dibujar_hud();
    dibujar_explosion_bomba();
    if(principal->esta_plantando_bomba) dibujar_mantenga_presionado(true);
    if(principal->esta_desactivando_bomba) dibujar_mantenga_presionado(false);
    if(snapshot.rondas_info.ronda_actual == 0) dibujar_esperando_jugadores();
    if(eventHandler.mercadoAbierto()) dibujar_mercado();
    if(!eventHandler.skinSeleccionado()) dibujar_seleccionar_skin();
    if(eventHandler.puedeMostrarEstadisticas()) dibujar_estadisticas();
    if(snapshot.equipo_ganador != NONE) dibujar_mensaje_ganador();

    //dibujar_vision_de_campo();

    renderer.Present();
}

Dibujador::~Dibujador()
{
}
