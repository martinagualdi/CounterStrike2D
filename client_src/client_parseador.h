#ifndef CLIENT_PARSEADOR_H
#define CLIENT_PARSEADOR_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

enum NUMEROS_HUD{CERO, UNO, DOS, TRES, CUATRO, CINCO, SEIS, SIETE, OCHO, NUEVE, DOS_PUNTOS, PIPE};
enum SIMBOLOS_HUD{SALUD, ESCUDO, TIEMPO, DEFUSE, COMPRAS, SIGHT, BOMBA, SALDO, EXCLAMACION, CONO, PENSAR, INFINITO, CALABERA};
enum SPRITES_JUGADOR{MANO_IZQ, MANO_DER, DOS_MANOS, ARMADO, MANO_IZQ_CUCHILLO, MANO_DER_CUCHILLO};

using namespace SDL2pp;

class ParseadorSpriteSheets{
private:
    void iterar(std::vector<SDL_Rect>& sprites, const int x, const int y, 
                const int cant_sprites, const int ancho, const int alto);
public:
    ParseadorSpriteSheets();
    SDL_Rect obtener_sprite_arma();
    SDL_Rect obtener_sprite_bala();
    SDL_Rect obtener_sprite_sight();
    std::vector<SDL_Rect> obtener_sprites_jugador();
    std::vector<SDL_Rect> obtener_sprites_pies_jugador();
    std::vector<SDL_Rect> obtener_sprites_simbolos_hud();
    std::vector<SDL_Rect> obtener_sprites_numeros_hud();
    ParseadorSpriteSheets(const ParseadorSpriteSheets&) = delete;
    ParseadorSpriteSheets& operator=(const ParseadorSpriteSheets&) = delete;
    ParseadorSpriteSheets(ParseadorSpriteSheets&&) = default;
    ParseadorSpriteSheets& operator=(ParseadorSpriteSheets&&) = default;
    ~ParseadorSpriteSheets();
};

#endif