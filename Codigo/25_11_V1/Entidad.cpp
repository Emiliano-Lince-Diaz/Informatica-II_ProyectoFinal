#include "Entidad.h"

Entidad::Entidad(float x, float y, float ancho, float alto)
    : x(x), y(y), vx(0.0f), vy(0.0f), ancho(ancho), alto(alto), activo(true) {
}

Entidad::~Entidad() {
}

void Entidad::mover(float dx, float dy) {
    x += dx;
    y += dy;
}

bool Entidad::colisionaCon(const Entidad& otra) const {
    if (!this->activo || !otra.activo) {
        return false;
    }
    // Detección de colisiones AABB (Axis-Aligned Bounding Box)
    bool colisionX = (this->x < otra.x + otra.ancho) && (this->x + this->ancho > otra.x);
    bool colisionY = (this->y < otra.y + otra.alto) && (this->y + this->alto > otra.y);

    return colisionX && colisionY;
}

void Entidad::desactivar() {
    activo = false;
}

float Entidad::getX() const { return x; }
float Entidad::getY() const { return y; }
float Entidad::getVx() const { return vx; }
float Entidad::getVy() const { return vy; }
float Entidad::getAncho() const { return ancho; }
float Entidad::getAlto() const { return alto; }
bool Entidad::estaActivo() const { return activo; }

void Entidad::setX(float nuevoX) { x = nuevoX; }
void Entidad::setY(float nuevoY) { y = nuevoY; }
void Entidad::setVx(float nuevoVx) { vx = nuevoVx; }
void Entidad::setVy(float nuevoVy) { vy = nuevoVy; }
void Entidad::setActivo(bool estado) { activo = estado; }