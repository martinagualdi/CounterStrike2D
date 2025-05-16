#ifndef CLIENT_PARSEADOR_H
#define CLIENT_PARSEADOR_H

#include <vector>
#include <SDL2/SDL.h>
#include <SDL2pp/SDL2pp.hh>

enum ct_types {SEAL_FORCE, GERMAN_GSG_9, UK_SAS, FRENCH_GIGN};
enum tt_types {PHEONIX, L337_KREW, ARTIC_AVENGER, GUERRILLA};

class ParseadorSpriteSheets{
private:

public:
    ParseadorSpriteSheets();
    std::vector<SDL_Rect> obtener_sprites_ct(int ct_type);
    std::vector<SDL_Rect> obtener_sprites_tt(int ct_type);

    ParseadorSpriteSheets(const ParseadorSpriteSheets&) = delete;
    ParseadorSpriteSheets& operator=(const ParseadorSpriteSheets&) = delete;
    ParseadorSpriteSheets(ParseadorSpriteSheets&&) = default;
    ParseadorSpriteSheets& operator=(ParseadorSpriteSheets&&) = default;
    ~ParseadorSpriteSheets();
};

#endif