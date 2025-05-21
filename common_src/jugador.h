#ifndef JUGADOR_H
#define JUGADOR_H

class Jugador {
  private:
    int id;
    int x = 10;
    int y = 10;
    float angulo = 0;

  public:
    explicit Jugador(int id) : id(id) {};
    Jugador(int id, int x, int y, float angulo) : id(id), x(x), y(y), angulo(angulo) {}

    bool comparar_id(int id) {
        return this->id == id;
    }

    int getId() const {
        return id;
    }

    int getX() const {
        return x;
    }
    int getY() const {
        return y;
    }

    float getAngulo() const {
        return angulo;
    }
    void setAngulo(float angulo) {
        this->angulo = angulo;
    }

    void setX(int posX) {
        this->x = posX;
    }
    void setY(int posY) {
        this->y = posY;
    }

    Jugador(const Jugador &) = default;
    Jugador &operator=(const Jugador &) = default;
    Jugador(Jugador &&) = default;
    Jugador &operator=(Jugador &&) = default;
};

#endif