#ifndef MUNICION_H
#define MUNICION_H

class Municion {
private: 
    int id_quien_disparo;
    float pos_x;
    float pos_y;
    float angulo_disparo;
public:
    explicit Municion(int id_quien_disparo, float pos_x, float pos_y, float angulo_disparo);

    int quien_disparo() const {
        return id_quien_disparo;
    }
    float getPosX() const {
        return pos_x;
    }
    float getPosY() const {
        return pos_y;
    }
    float getAnguloDisparo() const {
        return angulo_disparo;
    }

    void setPosX(float pos_x) {
        this->pos_x = pos_x;
    }

    void setPosY(float pos_y) {
        this->pos_y = pos_y;
    }
        


};

#endif