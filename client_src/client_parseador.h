#ifndef CLIENT_PARSEADOR_H
#define CLIENT_PARSEADOR_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>


using namespace SDL2pp;

class ParseadorSpriteSheets{
private:
    void ParseadorSpriteSheets::iterar_sobre_spritesheet(std::vector<Rect>& sprites, const int cant_sprites, 
                                                    const int x, const int y, const int tamanio_sprite);

public:
    ParseadorSpriteSheets();
    std::vector<Rect> obtener_sprites_pies_jugador();
    ParseadorSpriteSheets(const ParseadorSpriteSheets&) = delete;
    ParseadorSpriteSheets& operator=(const ParseadorSpriteSheets&) = delete;
    ParseadorSpriteSheets(ParseadorSpriteSheets&&) = default;
    ParseadorSpriteSheets& operator=(ParseadorSpriteSheets&&) = default;
    ~ParseadorSpriteSheets();
};

#endif