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
#define OFFSET_ANCHO_MERCADO 130
#define OFFSET_NOMBRE_ARMAS 40
#define ANCHO_CS2D 96
#define ALTO_CS2D 40
#define OFFSET_TRANSPARENTE 100
#define ESPACIO_ENTRE_ITEMS 20
#define OFFSET_SALDO 2.1
#define CANT_ARMAS_MERCADO 3
#define CANT_SKINS_PLAYER 4
#define FADE_SPEED 100.0f
#define CANT_PLAYERS 4
#define OFFSET_HUD 34

Dibujador::Dibujador(const int id, Renderer &renderer, struct Mapa mapa, EventHandler &handler,
                     Queue<Snapshot> &cola_recibidor, int ancho_ventana, int alto_ventana,
                     int ancho_real, int alto_real, InfoConfigClient infoConfig)
    : client_id(id), 
    ancho_ventana(ancho_ventana), 
    alto_ventana(alto_ventana),
    ancho_real(ancho_real),
    alto_real(alto_real),
    infoConfig(infoConfig),
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
    fuente(RUTA_IMAGENES("fonts/sourcesans.ttf"), alto_ventana * ESCALA_LETRA_GRANDE),
    fuenteChica(RUTA_IMAGENES("fonts/sourcesans.ttf"), alto_ventana * ESCALA_LETRA_CHICA),
    amarillo(Color(255, 255, 0)),
    blanco(Color(255, 255, 255)),
    verde(Color(100, 220, 100)),
    rojo(Color(200, 80, 80)),
    amarillento(Color(240, 180, 50)),
    celeste(Color(80, 200, 255)),
    mensaje_bomba_plantada(renderer, fuenteChica.RenderText_Blended("¡La bomba ha sido plantada!", amarillo)),
    mantenga_presionado_activar(renderer, fuenteChica.RenderText_Blended
    ("Mantenga presionado hasta finalizar el activado", amarillo)),
    mantenga_presionado_desactivar(renderer, fuenteChica.RenderText_Blended
    ("Mantenga presionado hasta finalizar el desactivado", amarillo)),
    partida_finalizada(renderer, fuenteChica.RenderText_Blended("La partida ha finalizado!", amarillo)),
    balas(Texture(renderer, Surface(IMG_Load(RUTA_IMAGENES("shells.png").c_str())))),
    cs2d(Texture(renderer, Surface(IMG_Load(RUTA_IMAGENES("gametitle.png").c_str())))),
    player_legs(Texture(renderer, Surface(IMG_Load(RUTA_IMAGENES("player/legs.bmp").c_str())))),
    muerto(Texture(renderer, Surface(IMG_Load(RUTA_IMAGENES("player/muerto.png").c_str())))),
    simbolos_hud([&renderer]() {
        Surface s(IMG_Load(RUTA_IMAGENES("hud_symbols.bmp").c_str()));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 0, 0, 0));
        Texture t(renderer, s);
        t.SetAlphaMod(128);
        t.SetColorMod(29, 140, 31); // Color verde hud
        return t;
    }()),
    numeros_hud([&renderer]() {
        Surface s(IMG_Load(RUTA_IMAGENES("hud_nums.bmp").c_str()));
        s.SetColorKey(true, SDL_MapRGB(s.Get()->format, 0, 0, 0));
        Texture t(renderer, s);
        t.SetAlphaMod(128);
        t.SetColorMod(29, 140, 31); // Color verde hud
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
    mensajes_ganadores(),
    sprite_arma(parseador.obtener_sprite_arma()),
    sprite_bala(parseador.obtener_sprite_bala()),
    sprite_sight(parseador.obtener_sprite_sight()),
    sprites_player(parseador.obtener_sprites_jugador()),
    sprites_player_legs(parseador.obtener_sprites_pies_jugador()),
    sprites_simbolos_hud(parseador.obtener_sprites_simbolos_hud()),
    sprites_numeros_hud(parseador.obtener_sprites_numeros_hud()),
    tam_mascara_fov((std::ceil(std::sqrt(ancho_ventana * ancho_ventana + alto_ventana * alto_ventana)))),
    mascara_fov(crearMascaraFOV(infoConfig.radio_vision, infoConfig.angulo_vision, infoConfig.opacidad)) {
    inicializar_textos();
}

Texture Dibujador::crearMascaraFOV(float radio_centro, float angulo_fov, Uint8 alpha_fondo) {

    std::vector<Uint32> pixels(tam_mascara_fov * tam_mascara_fov, 0);
    float radio_fov = tam_mascara_fov / 2;
    float angulo_inicio = -angulo_fov / 2.0f;
    float angulo_fin    = +angulo_fov / 2.0f;
    int cx = tam_mascara_fov / 2;
    int cy = tam_mascara_fov / 2;
    auto estaEnFOV = [&](int x, int y) {
        float dx = x - cx;
        float dy = y - cy;
        float dist = std::sqrt(dx*dx + dy*dy);
        if (dist > radio_fov)
            return false;
        float ang = std::atan2(dy, dx) * 180.0f / M_PI;
        return (ang >= angulo_inicio && ang <= angulo_fin);
    };

    for (int y = 0; y < tam_mascara_fov; ++y) {
        for (int x = 0; x < tam_mascara_fov; ++x) {
            float dx = x - cx;
            float dy = y - cy;
            float dist = std::sqrt(dx*dx + dy*dy);

            bool visible = false;
            // Visible si está en el círculo pequeño
            if (dist < radio_centro) visible = true;
            // O si está dentro del FOV
            else if (estaEnFOV(x, y)) visible = true;
            Uint8 a = visible ? 0 : alpha_fondo;
            Uint32 color = (0 << 24) | (0 << 16) | (0 << 8) | a;
            pixels[y * tam_mascara_fov + x] = color;
        }
    }

    SDL_Surface* sdl_surface = SDL_CreateRGBSurfaceWithFormatFrom(
    pixels.data(), tam_mascara_fov, tam_mascara_fov, 32, tam_mascara_fov * 4, SDL_PIXELFORMAT_RGBA8888);
    Surface surface(sdl_surface);
    Texture tex(renderer, surface);
    return tex;
}

void Dibujador::inicializar_textos() {
    textos_skin.emplace_back(
        renderer, fuente.RenderText_Blended("Seleccione el skin del personaje", amarillo));
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

    mensajes_ganadores.emplace_back(
        renderer, fuenteChica.RenderText_Blended("Los Counter-Terrorist han ganado!", amarillo));
    mensajes_ganadores.emplace_back(
        renderer, fuenteChica.RenderText_Blended("Los Terroristas han ganado!", amarillo));

    aviso_desconectar.emplace_back(renderer, fuenteChica.RenderText_Blended("Desea desconectarse de la partida?", amarillo));
    aviso_desconectar.emplace_back(renderer, fuenteChica.RenderText_Blended("[Q] Cancelar - [ENTER] Confirmar", amarillo));

}

float Dibujador::convertir_angulo(float angulo) {
    return 360.0f - angulo + DESFASE_ANGULO;
}

void Dibujador::convertir_a_pantalla(float pos_x, float pos_y, float &x_pixel, float &y_pixel) {
    const InfoJugador *jugador_principal = snapshot.getJugadorPorId(client_id);
    if (!jugador_principal)
        return;

    float centro_x = ancho_ventana / 2.0f;
    float centro_y = alto_ventana / 2.0f;

    x_pixel = pos_x - jugador_principal->pos_x + centro_x;
    y_pixel = -pos_y + jugador_principal->pos_y + centro_y;
}

void Dibujador::dibujar_jugadores() {

    for (const InfoJugador &jugador : snapshot.info_jugadores) {

        if (!jugador.esta_vivo)
            continue;
        float x_pixel = 0, y_pixel = 0;
      
        convertir_a_pantalla(jugador.pos_x, jugador.pos_y, x_pixel, y_pixel);
        float angulo_sdl = convertir_angulo(jugador.angulo);
        if (jugador.esta_moviendose)
            dibujar_pies(x_pixel, y_pixel, angulo_sdl);
        enum SkinTipos skin = jugador.skin_tipo;
        enum ArmaEnMano arma = jugador.arma_en_mano;
        enum Equipo equipo = jugador.equipo;
        dibujar_cuerpo(x_pixel, y_pixel, angulo_sdl, skin, arma, equipo);
        dibujar_arma(x_pixel, y_pixel, angulo_sdl, arma);
    }
}

void Dibujador::dibujar_muertos() {

    for (const InfoJugador &jugador : snapshot.info_jugadores) {

        if (jugador.esta_vivo)
            continue;
        float x_pixel = 0, y_pixel = 0;
        convertir_a_pantalla(jugador.pos_x, jugador.pos_y, x_pixel, y_pixel);
        Rect dst;
        dst.SetX(x_pixel - TAM_PLAYER / 2);
        dst.SetY(y_pixel - TAM_PLAYER / 2);
        dst.SetW(TAM_PLAYER);
        dst.SetH(TAM_PLAYER);
        renderer.Copy(muerto, NullOpt, dst);
    }
}

void Dibujador::dibujar_fondo(const ElementoMapa &elemento) {
    renderer.Copy(*elemento.texture, NullOpt, elemento.dst);
}

void Dibujador::dibujar_balas() {
    const float LARGO_BALA = 100.0f;
    const float ANCHO_BALA = 20.0f;

    for (InfoMunicion &bala : snapshot.balas_disparadas) {
        float x_pixel = 0, y_pixel = 0;
      
        convertir_a_pantalla(bala.pos_x, bala.pos_y, x_pixel, y_pixel);
        float angulo_bala = convertir_angulo(bala.angulo_disparo);
        float angle_rad = bala.angulo_disparo * M_PI / 180.0f;
        float OFFSET_ADELANTE = TAM_PLAYER; // ajustá este valor

        float origen_x = x_pixel + std::cos(angle_rad) * OFFSET_ADELANTE;
        float origen_y = y_pixel - std::sin(angle_rad) * OFFSET_ADELANTE;
        SDL_FRect dst{origen_x - ANCHO_BALA / 2, origen_y - LARGO_BALA / 2, ANCHO_BALA, LARGO_BALA};
        SDL_FPoint center = {ANCHO_BALA / 2, LARGO_BALA / 2};

        SDL_RenderCopyExF(renderer.Get(), balas.Get(), &sprite_bala, &dst, angulo_bala, &center,
                          SDL_FLIP_NONE);
    }
}

void Dibujador::dibujar_cuerpo(float x, float y, float angulo, enum SkinTipos skin,
                               enum ArmaEnMano arma, enum Equipo equipo) {

    SDL_FRect dst{x - TAM_PLAYER / 2, y - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
    SDL_Rect sprite;
    std::vector<Texture> &players = (equipo == CT) ? ct_players : tt_players;

    if (arma == CUCHILLO) sprite = sprites_player[MANO_IZQ_CUCHILLO];
    else if (arma == BOMBA_TT) sprite = sprites_player[DOS_MANOS];
    else sprite = sprites_player[ARMADO];

    SDL_FPoint center = {TAM_PLAYER / 2, TAM_PLAYER / 2};
    SDL_RenderCopyExF(renderer.Get(), players[skin].Get(), &sprite, &dst, angulo, &center,
                      SDL_FLIP_NONE);
}

void Dibujador::dibujar_pies(float x, float y, float angulo) {

    Uint32 current_ticks = SDL_GetTicks();
    int i = (current_ticks / 100) % sprites_player_legs.size();
    SDL_Rect sprite_actual = sprites_player_legs[i];
    SDL_FRect dst{x - TAM_PLAYER / 2, y - TAM_PLAYER / 2, TAM_PLAYER, TAM_PLAYER};
    SDL_FPoint center = {TAM_PLAYER / 2, TAM_PLAYER / 2};
    SDL_RenderCopyExF(renderer.Get(), player_legs.Get(), &sprite_actual, &dst, angulo, &center,
                      SDL_FLIP_NONE);
}

void Dibujador::dibujar_arma(float x, float y, float angulo, enum ArmaEnMano arma_actual) {

    SDL_FRect dst;
    if (arma_actual == CUCHILLO)
        dst = {x - TAM_PLAYER / 2, (y - TAM_PLAYER / 14) - TAM_PLAYER, TAM_PLAYER, TAM_PLAYER};
    else if (arma_actual == BOMBA_TT)
        dst = {x - TAM_PLAYER / 2 - TAM_PLAYER / 10, (y - TAM_PLAYER / 6) - TAM_PLAYER, TAM_PLAYER,
               TAM_PLAYER};
    else
        dst = {x - TAM_PLAYER / 2, (y - TAM_PLAYER / 6) - TAM_PLAYER, TAM_PLAYER, TAM_PLAYER};
    SDL_FPoint center = {x - dst.x, y - dst.y};
    SDL_RenderCopyExF(renderer.Get(), armas[arma_actual].Get(), &sprite_arma, &dst, angulo, &center,
                      SDL_FLIP_NONE);
}

void Dibujador::dibujar_sight() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    float escala = std::min(ancho_real / float(ancho_ventana), alto_real / (float)alto_ventana);
    float offset_x = (ancho_real - float(ancho_ventana) * escala) / 2.0f;
    float offset_y = (alto_real - (float)alto_ventana * escala) / 2.0f;

    float mouseX_logico = (mouseX - offset_x) / escala;
    float mouseY_logico = (mouseY - offset_y) / escala;

    Rect dst;
    dst.SetX(mouseX_logico - TAM_SIGHT / 2);
    dst.SetY(mouseY_logico - TAM_SIGHT / 2);
    dst.SetW(TAM_SIGHT);
    dst.SetH(TAM_SIGHT);
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

    if (agregar_cero)
        digitos.insert(digitos.begin(), 0);

    return digitos;
}

void Dibujador::dibujar_salud(int salud) {

    std::vector<int> digitos_salud = separar_digitos(salud);
    int cant_digitos_salud = static_cast<int>(digitos_salud.size());

    Rect sprite_salud(sprites_simbolos_hud[SALUD]);
    Rect salud_dst(0, alto_ventana - TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprite_salud, salud_dst);

    for (int i = 0; i < cant_digitos_salud; i++) {
        Rect sprite_digito(sprites_numeros_hud[digitos_salud[i]]);
        Rect dst;
        dst.SetX(TAM_SIMBOLOS_HUD + i * OFFSET_HUD);
        dst.SetY(alto_ventana - TAM_SIMBOLOS_HUD);
        dst.SetW(OFFSET_HUD);
        dst.SetH(TAM_SIMBOLOS_HUD);
        renderer.Copy(numeros_hud, sprite_digito, dst);
    }
}

void Dibujador::dibujar_tiempo(int tiempo_restante) {

    Rect sprite_reloj(sprites_simbolos_hud[TIEMPO]);
    Rect reloj_dst;
    reloj_dst.SetX((ancho_ventana / 2) - OFFSET_TIEMPO);
    reloj_dst.SetY(alto_ventana - TAM_SIMBOLOS_HUD);
    reloj_dst.SetW(TAM_SIMBOLOS_HUD);
    reloj_dst.SetH(TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprite_reloj, reloj_dst);

    int minutos = tiempo_restante / 60;
    int segundos = tiempo_restante % 60;

    std::vector<int> digitos_minutos = separar_digitos_tiempo(minutos);
    std::vector<int> digitos_segundos = separar_digitos_tiempo(segundos);

    int pos_x = ancho_ventana / 2 - OFFSET_TIEMPO + TAM_SIMBOLOS_HUD;

    Rect tiempo_dst;
    tiempo_dst.SetY(alto_ventana - TAM_SIMBOLOS_HUD);
    tiempo_dst.SetW(OFFSET_HUD);
    tiempo_dst.SetH(TAM_SIMBOLOS_HUD);

    for (int i = 0; i < (int)digitos_minutos.size(); i++) {
        tiempo_dst.SetX(pos_x);
        Rect sprite_digito = sprites_numeros_hud[digitos_minutos[i]];
        renderer.Copy(numeros_hud, sprite_digito, tiempo_dst);
        pos_x += OFFSET_HUD; // desplazamiento
    }

    Rect dos_puntos_dst;
    dos_puntos_dst.SetX(pos_x);
    dos_puntos_dst.SetY(alto_ventana - TAM_SIMBOLOS_HUD);
    dos_puntos_dst.SetW(TAM_SIMBOLOS_HUD);
    dos_puntos_dst.SetH(TAM_SIMBOLOS_HUD);
    renderer.Copy(numeros_hud, sprites_numeros_hud[DOS_PUNTOS], dos_puntos_dst);
    pos_x += ANCHO_NUMEROS_HUD / 3;

    for (int i = 0; i < (int)digitos_segundos.size(); i++) {
        tiempo_dst.SetX(pos_x);
        Rect sprite_digito = sprites_numeros_hud[digitos_segundos[i]];
        renderer.Copy(numeros_hud, sprite_digito, tiempo_dst);
        pos_x += OFFSET_HUD;
    }
}

void Dibujador::dibujar_saldo(int saldo, bool arma_con_balas) {

    std::vector<int> digitos_dinero = separar_digitos(saldo);
    int cant_digitos_dinero = static_cast<int>(digitos_dinero.size());
    int x = 0, y = 0;

    if (arma_con_balas)
        y = alto_ventana - OFFSET_SALDO * TAM_SIMBOLOS_HUD;
    else
        y = alto_ventana - TAM_SIMBOLOS_HUD;

    for (int i = 0; i < cant_digitos_dinero; i++) {
        Rect sprite_digito(sprites_numeros_hud[digitos_dinero[cant_digitos_dinero - i - 1]]);
        x = ancho_ventana - OFFSET_HUD * (i + 1);
        Rect dst(x, y, OFFSET_HUD, TAM_SIMBOLOS_HUD);
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

    for (int i = 0; i < cant_digitos_balas; i++) {
        Rect sprite_digito(sprites_numeros_hud[digitos_balas[cant_digitos_balas - i - 1]]);
        Rect dst;
        dst.SetX(ancho_ventana - OFFSET_HUD * (i + 1));
        dst.SetY(alto_ventana - TAM_SIMBOLOS_HUD);
        dst.SetW(OFFSET_HUD);
        dst.SetH(TAM_SIMBOLOS_HUD);
        renderer.Copy(numeros_hud, sprite_digito, dst);
    }
}

Texture Dibujador::crearTextoArma(std::string nombre, int precio) {
    std::string texto = nombre + "$" + std::to_string(precio);
    return Texture(renderer, fuente.RenderText_Blended(texto, amarillo));
}

void Dibujador::dibujar_simbolo_mercado() {

    Rect dst;
    dst.SetX(static_cast<int>(ancho_ventana * 5 / 8));
    dst.SetY(alto_ventana - TAM_SIMBOLOS_HUD);
    dst.SetW(TAM_SIMBOLOS_HUD);
    dst.SetH(TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprites_simbolos_hud[MERCADO], dst);
}

void Dibujador::dibujar_simbolo_tiene_bomba() {

    Rect dst;
    dst.SetX(static_cast<int>(ancho_ventana * 5 / 8) + TAM_SIMBOLOS_HUD);
    dst.SetY(alto_ventana - TAM_SIMBOLOS_HUD);
    dst.SetW(TAM_SIMBOLOS_HUD);
    dst.SetH(TAM_SIMBOLOS_HUD);
    renderer.Copy(simbolos_hud, sprites_simbolos_hud[ZONA_BOMBA], dst);
}

void Dibujador::dibujar_mantenga_presionado(bool activar) {

    Texture &mantenga_presionado =
        activar ? mantenga_presionado_activar : mantenga_presionado_desactivar;

    int ancho_mensaje = mantenga_presionado.GetWidth();
    int alto_mensaje = mantenga_presionado.GetHeight();

    Rect dst;
    dst.SetX((ancho_ventana / 2) - ancho_mensaje / 2);
    dst.SetY((alto_ventana / 3) - alto_mensaje / 2);
    dst.SetW(ancho_mensaje);
    dst.SetH(alto_mensaje);
    renderer.Copy(mantenga_presionado, NullOpt, dst);
}

void Dibujador::dibujar_cuadro_negro_transparente(Texture &titulo) {

    int anchoCuadro = ancho_ventana - OFFSET_TRANSPARENTE;
    int altoCuadro = alto_ventana - OFFSET_TRANSPARENTE;
    int x = (ancho_ventana - anchoCuadro) / 2;
    int y = (alto_ventana - altoCuadro) / 2;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(x, y, anchoCuadro, altoCuadro));

    renderer.Copy(cs2d, NullOpt, Rect(x + OFFSET_CS2D, y + OFFSET_CS2D, ANCHO_CS2D, ALTO_CS2D));
    renderer.Copy(titulo, NullOpt, Rect(x + 130, y + 10, titulo.GetWidth(), titulo.GetHeight()));
}

void Dibujador::dibujar_mercado() {

    Texture titulo(renderer, fuente.RenderText_Blended("Comprar Armas", amarillo));
    dibujar_cuadro_negro_transparente(titulo);

    int anchoCuadro = ancho_ventana - OFFSET_TRANSPARENTE;
    int x = (ancho_ventana - anchoCuadro) / 2;

    std::vector<Texture> textos;
    textos.push_back(crearTextoArma("[1] AK-47       ", infoConfig.precio_ak47));
    textos.push_back(crearTextoArma("[2] M3             ", infoConfig.precio_m3));
    textos.push_back(crearTextoArma("[3] AWP          ", infoConfig.precio_awp));
    Texture primaria(renderer, fuente.RenderText_Blended("[,] Balas de arma primaria", amarillo));
    Texture secundaria(renderer,
                       fuente.RenderText_Blended("[.] Balas de arma secundaria", amarillo));
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

    int y_inicial = (alto_ventana - alto_total) / 2;
    int y_pos = y_inicial;

    for (int i = 0; i < CANT_ARMAS_MERCADO; i++) {

        Rect dst_texto;
        dst_texto.SetX(x + OFFSET_NOMBRE_ARMAS);
        dst_texto.SetY(y_pos);
        dst_texto.SetW(textos[i].GetWidth());
        dst_texto.SetH(textos[i].GetHeight());
        renderer.Copy(textos[i], NullOpt, dst_texto);

        Rect dst_arma;
        dst_arma.SetX(ancho_ventana / 2 + 30);
        dst_arma.SetY(y_pos);
        dst_arma.SetW(ancho_ventana * ESCALA_ANCHO_ARMAS);
        dst_arma.SetH(alto_ventana * ESCALA_ALTO_ARMAS);
        renderer.Copy(armas_mercado_y_tiradas[i], NullOpt, dst_arma);

        int alto_texto = textos[i].GetHeight();
        int alto_arma = dst_arma.h;
        y_pos += std::max(alto_texto, alto_arma) + ESPACIO_ENTRE_ITEMS;
    }

    Rect dst_primaria;
    dst_primaria.SetX(x + OFFSET_NOMBRE_ARMAS);
    dst_primaria.SetY(y_pos);
    dst_primaria.SetW(primaria.GetWidth());
    dst_primaria.SetH(primaria.GetHeight());
    renderer.Copy(primaria, NullOpt, dst_primaria);
    y_pos += primaria.GetHeight() + ESPACIO_ENTRE_ITEMS;

    Rect dst_secundaria;
    dst_secundaria.SetX(x + OFFSET_NOMBRE_ARMAS);
    dst_secundaria.SetY(y_pos);
    dst_secundaria.SetW(secundaria.GetWidth());
    dst_secundaria.SetH(secundaria.GetHeight());
    renderer.Copy(secundaria, NullOpt, dst_secundaria);
    y_pos += secundaria.GetHeight() + ESPACIO_ENTRE_ITEMS;

    Rect dst_salir(x + OFFSET_NOMBRE_ARMAS, y_pos, salir.GetWidth(), salir.GetHeight());
    renderer.Copy(salir, NullOpt, dst_salir);
    y_pos += secundaria.GetHeight() + ESPACIO_ENTRE_ITEMS;
}

void Dibujador::dibujar_seleccionar_skin() {
    auto jugador = snapshot.getJugadorPorId(client_id);
    if (!jugador)
        return;

    bool es_tt = jugador->equipo;
    std::vector<Texture> &nombres = es_tt ? tt_nombres : ct_nombres;
    std::vector<Texture> &players = es_tt ? tt_players : ct_players;

    dibujar_cuadro_negro_transparente(textos_skin[0]);
    int anchoCuadro = ancho_ventana - OFFSET_TRANSPARENTE;
    int x = (ancho_ventana - anchoCuadro) / 2;

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
    int y_inicial = (alto_ventana - alto_total) / 2;
    int y_pos = y_inicial + 30;

    for (int i = 0; i < CANT_SKINS_PLAYER; i++) {
        Rect dst_nombre(x_inicial, y_pos, nombres[i].GetWidth(), nombres[i].GetHeight());
        renderer.Copy(nombres[i], NullOpt, dst_nombre);

        int tam_skin = static_cast<int>(TAM_PLAYER * 1.5);
        Rect dst_skin(ancho_ventana / 2 + ancho_ventana / 4, y_pos, tam_skin, tam_skin);
        renderer.Copy(players[i], sprites_player[DOS_MANOS], dst_skin);

        int alto_texto = nombres[i].GetHeight();
        int alto_skin = tam_skin;
        y_pos += std::max(alto_texto, alto_skin) + ESPACIO_ENTRE_ITEMS;
    }
}

void Dibujador::dibujar_armas_tiradas() {

    for (const InfoArmaEnSuelo &arma_tirada : snapshot.armas_sueltas) {

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

    if (estado_actual == DETONADA && estado_bomba_anterior != DETONADA) {
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
        renderer.FillRect(0, 0, ancho_ventana, alto_ventana);

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
    dst.SetX((ancho_ventana / 2) - ancho_mensaje / 2);
    dst.SetY((alto_ventana / 3) - alto_mensaje / 2);
    dst.SetW(ancho_mensaje);
    dst.SetH(alto_mensaje);
    renderer.Copy(mensajes_ganadores[ganador], NullOpt, dst);
}

void Dibujador::dibujar_mensaje_bomba_plantada() {

    int ancho_mensaje = mensaje_bomba_plantada.GetWidth();
    int alto_mensaje = mensaje_bomba_plantada.GetHeight();

    Rect dst;
    dst.SetX((ancho_ventana / 2) - ancho_mensaje / 2);
    dst.SetY((alto_ventana / 3) - alto_mensaje / 2);
    dst.SetW(ancho_mensaje);
    dst.SetH(alto_mensaje);
    renderer.Copy(mensaje_bomba_plantada, NullOpt, dst);
}

void Dibujador::dibujar_bomba_plantada() {

    float x_logica = snapshot.bomba_en_suelo.pos_x;
    float y_logica = snapshot.bomba_en_suelo.pos_y;
    float x_pixel = 0, y_pixel = 0;
    convertir_a_pantalla(x_logica, y_logica, x_pixel, y_pixel);
    Rect dst;
    dst.SetX(x_pixel - TAM_PLAYER / 2);
    dst.SetY(y_pixel - TAM_PLAYER / 2);
    dst.SetW(TAM_PLAYER);
    dst.SetH(TAM_PLAYER);

    renderer.Copy(armas[BOMBA_TT], NullOpt, dst);
}

void Dibujador::dibujar_vision_de_campo(float angulo_jugador) {

    int offset_x = (ancho_ventana - tam_mascara_fov) / 2;
    int offset_y = (alto_ventana - tam_mascara_fov) / 2;

    angulo_jugador = convertir_angulo(angulo_jugador) - DESFASE_ANGULO;
    Rect dst(offset_x, offset_y, tam_mascara_fov, tam_mascara_fov);
    Point point(tam_mascara_fov / 2, tam_mascara_fov / 2);

    renderer.Copy(mascara_fov, NullOpt, dst, angulo_jugador, point, SDL_FLIP_NONE);

}

void Dibujador::dibujar_estadisticas() {

    int ancho = 500;
    int altura_fila = 30;

    // Cantidad de filas: jugadores + títulos (ajustá +3/+6 según cuántos títulos/subtítulos
    // dibujás)
    int filas = snapshot.info_jugadores.size() + 6;
    // 6 si sumás: 2 títulos de equipo, 2 filas de columnas, y algún espacio extra

    int alto_calculado = 40 + filas * altura_fila + 20; // 40 arriba, 20 abajo
    int alto_minimo = 250;
    int alto = std::max(alto_calculado, alto_minimo);

    int x = (ancho_ventana - ancho) / 2;
    int y = (alto_ventana - alto) / 2;

    int y_fila_inicial = y + 40;

    renderer.SetDrawBlendMode(SDL_BLENDMODE_BLEND);
    renderer.SetDrawColor(0, 0, 0, 180);
    renderer.FillRect(Rect(x, y, ancho, alto));

    // Info de partida
    std::string info_partida = "Ronda: " + std::to_string(snapshot.rondas_info.ronda_actual) +
                               " / " + std::to_string(snapshot.rondas_info.total_rondas) +
                               "   CT: " + std::to_string(snapshot.rondas_info.rondas_ganadas_ct) +
                               "   TT: " + std::to_string(snapshot.rondas_info.rondas_ganadas_tt);
    Texture txt_info(renderer, fuenteChica.RenderText_Blended(info_partida, blanco));
    Rect dst_info(x + 20, y + 10, txt_info.GetWidth(), txt_info.GetHeight());
    renderer.Copy(txt_info, NullOpt, dst_info);

    // Columnas
    std::vector<std::string> titulos = {"Jugador", "Estado", "Puntos", "Muertes"};
    std::vector<int> col_x = {x + 20, x + 230, x + 320, x + 400}; // Offsets para que quede lindo
    int fila = 0;

    //  Counter-Terrorists
    Texture t_ct(renderer, fuenteChica.RenderText_Blended("Counter-Terrorists", celeste));
    Rect dst_titulo_ct;
    dst_titulo_ct.SetX(x + 20);
    dst_titulo_ct.SetY(y_fila_inicial + fila * altura_fila);
    dst_titulo_ct.SetW(t_ct.GetWidth());
    dst_titulo_ct.SetH(t_ct.GetHeight());
    renderer.Copy(t_ct, NullOpt, dst_titulo_ct);
    fila++;

    for (size_t i = 0; i < titulos.size(); ++i) {
        Texture txt_col(renderer, fuenteChica.RenderText_Blended(titulos[i], blanco));
        Rect dst_titulo;
        dst_titulo.SetX(col_x[i]);
        dst_titulo.SetY(y_fila_inicial + fila * altura_fila);
        dst_titulo.SetW(txt_col.GetWidth());
        dst_titulo.SetH(txt_col.GetHeight());
        renderer.Copy(txt_col, NullOpt, dst_titulo);
    }
    fila++;

    dibujar_estadisticas_jugador(col_x, y_fila_inicial, fila, altura_fila, CT);
    fila++;
    Texture t_tt(renderer, fuenteChica.RenderText_Blended("Terrorists", amarillento));
    Rect dst_titulo_tt;
    dst_titulo_tt.SetX(x + 20);
    dst_titulo_tt.SetY(y_fila_inicial + fila * altura_fila);
    dst_titulo_tt.SetW(t_tt.GetWidth());
    dst_titulo_tt.SetH(t_tt.GetHeight());
    renderer.Copy(t_tt, NullOpt, dst_titulo_tt);
    fila++;

    for (size_t i = 0; i < titulos.size(); ++i) {
        Texture txt_col(renderer, fuenteChica.RenderText_Blended(titulos[i], blanco));
        Rect dst_txt_col;
        dst_txt_col.SetX(col_x[i]);
        dst_txt_col.SetY(y_fila_inicial + fila * altura_fila);
        dst_txt_col.SetW(txt_col.GetWidth());
        dst_txt_col.SetH(txt_col.GetHeight());
        renderer.Copy(txt_col, NullOpt, dst_txt_col);
    }
    fila++;

    dibujar_estadisticas_jugador(col_x, y_fila_inicial, fila, altura_fila, TT);
}

void Dibujador::dibujar_estadisticas_jugador(std::vector<int> &col_x, int &y_fila_inicial,
                                             int &fila, int &altura_fila, enum Equipo equipo) {

    for (const InfoJugador &jug : snapshot.info_jugadores) {
        if (jug.equipo != equipo)
            continue;

        Texture user(renderer, fuenteChica.RenderText_Blended(jug.nombre, blanco));
        Rect dst_username(col_x[0], y_fila_inicial + fila * altura_fila, user.GetWidth(),
                          user.GetHeight());
        renderer.Copy(user, NullOpt, dst_username);
        if(!snapshot.termino_partida){
            std::string vivo = jug.esta_vivo ? "Vivo" : "Muerto";
            Color color_estado = jug.esta_vivo ? verde : rojo;
            Texture estado(renderer, fuenteChica.RenderText_Blended(vivo, color_estado));
            Rect dst_estado(col_x[1], y_fila_inicial + fila * altura_fila, estado.GetWidth(),
                            estado.GetHeight());
            renderer.Copy(estado, NullOpt, dst_estado);
        }
        Texture puntos(renderer, fuenteChica.RenderText_Blended(
                                     std::to_string(jug.eliminaciones_totales), blanco));
        Rect dst_puntos(col_x[2], y_fila_inicial + fila * altura_fila, puntos.GetWidth(),
                        puntos.GetHeight());
        renderer.Copy(puntos, NullOpt, dst_puntos);

        Texture muertes(renderer,
                        fuenteChica.RenderText_Blended(std::to_string(jug.muertes), blanco));
        Rect dst_muertes(col_x[3], y_fila_inicial + fila * altura_fila, muertes.GetWidth(),
                         muertes.GetHeight());
        renderer.Copy(muertes, NullOpt, dst_muertes);

        fila++;
    }
}

void Dibujador::dibujar_mapa() {

    for (const ElementoMapa &elemento : mapa.elementos) {
        if (elemento.tipo == FONDO) {
            dibujar_fondo(elemento);
        }
        else if(elemento.tipo == OBSTACULO || elemento.tipo == PISO || elemento.tipo == BOMBSITE){
            float x_pixel = 0, y_pixel = 0;
            convertir_a_pantalla(elemento.dst.x, -elemento.dst.y, x_pixel, y_pixel);
            Rect dst(x_pixel, y_pixel, elemento.dst.w, elemento.dst.h);
            renderer.Copy(*elemento.texture, NullOpt, dst);
        }
    }
}

void Dibujador::dibujar_esperando_jugadores() {

    Uint32 current_ticks = SDL_GetTicks();
    int i = (current_ticks / 500) % esperando_jugadores.size();

    int ancho_mensaje = esperando_jugadores[i].GetWidth();
    int alto_mensaje = esperando_jugadores[i].GetHeight();

    Rect dst_esperando;
    dst_esperando.SetX((ancho_ventana / 2) - esperando_jugadores[0].GetWidth() / 2);
    dst_esperando.SetY((alto_ventana / 8) - esperando_jugadores[0].GetHeight() / 2);
    dst_esperando.SetW(ancho_mensaje);
    dst_esperando.SetH(alto_mensaje);
    renderer.Copy(esperando_jugadores[i], NullOpt, dst_esperando);
}

void Dibujador::dibujar_aviso_desconectar() {

    for(int i = 0; i < (int)aviso_desconectar.size(); i++) { 
        int ancho_mensaje = aviso_desconectar[i].GetWidth();
        int alto_mensaje =  aviso_desconectar[i].GetHeight();

        Rect dst;
        dst.SetX((ancho_ventana / 2) - ancho_mensaje / 2);
        if(i == 0)
            dst.SetY((alto_ventana / 6) - alto_mensaje / 2);
        else{
            int offset = aviso_desconectar[i-1].GetHeight();
            dst.SetY((alto_ventana / 6) - alto_mensaje / 2 + offset);
        }   
        dst.SetW(ancho_mensaje);
        dst.SetH(alto_mensaje);

        renderer.Copy(aviso_desconectar[i], NullOpt, dst);
    }
    
}

void Dibujador::dibujar_partida_finalizada() {

    dibujar_estadisticas();

    int ancho_mensaje = partida_finalizada.GetWidth();
    int alto_mensaje = partida_finalizada.GetHeight();

    Rect dst;
    dst.SetX((ancho_ventana / 2) - ancho_mensaje / 2);
    dst.SetY((alto_ventana / 8) - alto_mensaje / 2);
    dst.SetW(ancho_mensaje);
    dst.SetH(alto_mensaje);
    renderer.Copy(partida_finalizada, NullOpt, dst);

}

void Dibujador::dibujar_hud() {

    const InfoJugador *jugador_principal = snapshot.getJugadorPorId(client_id);
    if (!jugador_principal)
        return;

    dibujar_salud(jugador_principal->vida);
    dibujar_tiempo(snapshot.tiempo_restante);
    if (jugador_principal->puede_comprar_ya)
        dibujar_simbolo_mercado();
    if (jugador_principal->tiene_bomba)
        dibujar_simbolo_tiene_bomba();
    if (jugador_principal->arma_en_mano != CUCHILLO) {
        dibujar_balas_hud(jugador_principal->balas);
        dibujar_saldo(jugador_principal->dinero, true);
    } else
        dibujar_saldo(jugador_principal->dinero, false);
}

void Dibujador::renderizar(Snapshot &snapshot) {
    this->snapshot = snapshot;

    renderer.Clear();
    dibujar_mapa();
    if(snapshot.termino_partida){
        dibujar_partida_finalizada();
        if(eventHandler.avisoDesconectarActivo())
            dibujar_aviso_desconectar();
        renderer.Present();
        return;
    }
    const InfoJugador *principal = snapshot.getJugadorPorId(client_id);
    if (!principal)
        return;
    dibujar_muertos();
    dibujar_armas_tiradas();
    if (snapshot.bomba_en_suelo.estado_bomba == PLANTADA)
        dibujar_bomba_plantada();
    dibujar_balas();
    dibujar_jugadores();
    if(eventHandler.skinSeleccionado() && !eventHandler.mercadoAbierto() && principal->esta_vivo)
        dibujar_vision_de_campo(principal->angulo);
    dibujar_sight();
    dibujar_hud();
    dibujar_explosion_bomba();
    if (principal->esta_plantando_bomba)
        dibujar_mantenga_presionado(true);
    if (principal->esta_desactivando_bomba)
        dibujar_mantenga_presionado(false);
    if (snapshot.rondas_info.ronda_actual == 0)
        dibujar_esperando_jugadores();
    if (eventHandler.mercadoAbierto())
        dibujar_mercado();
    if (!eventHandler.skinSeleccionado())
        dibujar_seleccionar_skin();
    if (eventHandler.puedeMostrarEstadisticas())
        dibujar_estadisticas();
    if(eventHandler.avisoDesconectarActivo())
        dibujar_aviso_desconectar();
    if (snapshot.equipo_ganador != NONE)
        dibujar_mensaje_ganador();

    renderer.Present();
}

Dibujador::~Dibujador() {}
