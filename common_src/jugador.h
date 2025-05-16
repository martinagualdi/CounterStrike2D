#ifndef JUGADOR_H
#define JUGADOR_H

class Jugador {
private:
    int x;
    int y;

public:
    
    Jugador(int posX = 0, int posY = 0) : x(posX), y(posY) {}

    
    int getX() const { return x; }
    int getY() const { return y; }

    
    void setX(int posX) { x = posX; }
    void setY(int posY) { y = posY; }
};

#endif 