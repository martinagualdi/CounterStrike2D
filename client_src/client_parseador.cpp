#include "client_parseador.h"

#define TAM_PLAYER 32
#define TAM_SIGHT 46
#define TAM_BALA 8
#define TAM_PLAYER_LEGS 32
#define CANT_PLAYER_LEGS 8
#define CANT_PLAYERS 6
#define CANT_NUMEROS_HUD 12
#define ALTO_NUMEROS_HUD 66
#define ANCHO_NUMEROS_HUD 48
#define CANT_SIMBOLOS_HUD 13
#define TAM_SIMBOLOS_HUD 64

ParseadorSpriteSheets::ParseadorSpriteSheets() {}

SDL_Rect ParseadorSpriteSheets::obtener_sprite_arma() {
    return {0, 0, TAM_PLAYER, TAM_PLAYER};
}

SDL_Rect ParseadorSpriteSheets::obtener_sprite_bala() {
    return {0, 0, TAM_BALA, TAM_BALA};
}

SDL_Rect ParseadorSpriteSheets::obtener_sprite_sight() {
    return {0, 0, TAM_SIGHT, TAM_SIGHT};
}

void ParseadorSpriteSheets::iterar(std::vector<SDL_Rect>& sprites, const int x, const int y, 
                            const int cant_sprites, const int ancho, const int alto){

    for(int i = 0; i < cant_sprites; i++){
        SDL_Rect rect;
        rect.x = x + i * ancho;
        rect.y = y;
        rect.w = ancho;
        rect.h = alto;
        sprites.push_back(rect);
    }
}

std::vector<SDL_Rect> ParseadorSpriteSheets::obtener_sprites_pies_jugador()
{
	std::vector<SDL_Rect> sprites;
    int y = 0;
    int x = 0;
    for (int i = 0; i < CANT_PLAYER_LEGS; i++) {
        if (x == 4 * TAM_PLAYER_LEGS){
            x = 0;
            y = TAM_PLAYER_LEGS;
        }
        SDL_Rect rect;
        rect.x = x;
        rect.y = y;
        rect.w = TAM_PLAYER_LEGS;
        rect.h = TAM_PLAYER_LEGS;
        sprites.push_back(rect);
        x += TAM_PLAYER_LEGS;
    }
    return sprites;
}

std::vector<SDL_Rect> ParseadorSpriteSheets::obtener_sprites_jugador() {
    std::vector<SDL_Rect> sprites;
    
    int x = 0;
    int y = 0;

    for(int i = 0; i < CANT_PLAYERS; i++){
        if(i > 0 && i % 2 == 0){
            x = 0;
            y += TAM_PLAYER; 
        }
        SDL_Rect rect;
        rect.x = x * TAM_PLAYER;
        rect.y = y;
        rect.h = TAM_PLAYER;
        rect.w = TAM_PLAYER;
        sprites.push_back(rect);
        x++;
    }

    return sprites;
}

std::vector<SDL_Rect> ParseadorSpriteSheets::obtener_sprites_simbolos_hud() {

    std::vector<SDL_Rect> sprites;
    iterar(sprites, 0, 0, CANT_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD, TAM_SIMBOLOS_HUD);
    return sprites;
}

std::vector<SDL_Rect> ParseadorSpriteSheets::obtener_sprites_numeros_hud() {
    
    std::vector<SDL_Rect> sprites;
    iterar(sprites, 0, 0, CANT_NUMEROS_HUD, ANCHO_NUMEROS_HUD, ALTO_NUMEROS_HUD);
    return sprites;

}
ParseadorSpriteSheets::~ParseadorSpriteSheets() {}
