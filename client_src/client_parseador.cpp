#include "client_parseador.h"

#define PLAYER_WIDTH 32
#define PLAYER_HEIGHT 32
#define PLAYER_LEGS 32
#define CANT_PLAYER_LEGS 8
#define CANT_SPRITES_PLAYER 6

ParseadorSpriteSheets::ParseadorSpriteSheets()
{
}

void ParseadorSpriteSheets::iterar_sobre_spritesheet(std::vector<Rect>& sprites, const int cant_sprites, 
                                                    const int x, const int y, const int tamanio_sprite){
    for(int i = 0; i < cant_sprites; i++){
        Rect rect;
        rect.SetX(x + i * tamanio_sprite);
        rect.SetY(PLAYER_HEIGHT * i);
        rect.SetW(PLAYER_WIDTH);
        rect.SetH(PLAYER_HEIGHT);
        sprites.push_back(rect);
    }
}

std::vector<Rect> ParseadorSpriteSheets::obtener_sprites_pies_jugador()
{
	std::vector<Rect> sprites_jugador;

    
    return sprites_jugador;
}

ParseadorSpriteSheets::~ParseadorSpriteSheets()
{
}
