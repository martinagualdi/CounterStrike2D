#ifndef JUGADOR_H
#define JUGADOR_H

class Jugador {
private:
    int id; 
    int x;
    int y;

public:
    
    Jugador();

    bool comparar_id(int id) { return this->id == id; }
    
    int getId() const { return id; }

    int getX() const { return x; }
    int getY() const { return y; }

    
    void setX(int posX) { x = posX; }
    void setY(int posY) { y = posY; }

    Jugador(const Jugador&) = default;
    Jugador& operator=(const Jugador&) = default;
    Jugador(Jugador&&) = default;
    Jugador& operator=(Jugador&&) = default;
    
};

#endif 