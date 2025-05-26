#ifndef CLIENT_PARSEADOR_H
#define CLIENT_PARSEADOR_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

enum ct_types {SEAL_FORCE, GERMAN_GSG_9, UK_SAS, FRENCH_GIGN};
enum tt_types {PHEONIX, L337_KREW, ARTIC_AVENGER, GUERRILLA};

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