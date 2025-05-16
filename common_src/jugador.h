#ifndef JUGADOR_H
#define JUGADOR_H

class Jugador {
private:
    float x;
    float y;

public:
    
    Jugador(float posX = 0.0f, float posY = 0.0f) : x(posX), y(posY) {}

    
    float getX() const { return x; }
    float getY() const { return y; }

    
    void setX(float posX) { x = posX; }
    void setY(float posY) { y = posY; }
};

#endif 