#ifndef PLATAFORMA_H
#define PLATAFORMA_H

#include "Entidad.h"

class Plataforma {
private:
    float x;
    float y;
    float ancho;
    float alto;

public:
    Plataforma(float x = 0.0f, float y = 0.0f, float ancho = 100.0f, float alto = 20.0f);
    ~Plataforma();

    bool colisionaCon(const Entidad& e) const;
    float superficieY() const;

    float getX() const;
    float getY() const;
    float getAncho() const;
    float getAlto() const;
};

#endif // PLATAFORMA_H