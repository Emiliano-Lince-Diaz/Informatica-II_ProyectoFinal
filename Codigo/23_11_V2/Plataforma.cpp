#include "Plataforma.h"

Plataforma::Plataforma(float x, float y, float ancho, float alto)
    : x(x), y(y), ancho(ancho), alto(alto) {
}

Plataforma::~Plataforma() {
}

bool Plataforma::colisionaCon(const Entidad& e) const {
    if (!e.estaActivo()) {
        return false;
    }
    bool colisionX = (e.getX() < x + ancho) && (e.getX() + e.getAncho() > x);
    bool colisionY = (e.getY() < y + alto) && (e.getY() + e.getAlto() > y);
    return colisionX && colisionY;
}

float Plataforma::superficieY() const {
    return y;
}

float Plataforma::getX() const { return x; }
float Plataforma::getY() const { return y; }
float Plataforma::getAncho() const { return ancho; }
float Plataforma::getAlto() const { return alto; }