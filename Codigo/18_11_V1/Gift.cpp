#include "Gift.h"

Gift::Gift(float x, float y, float velCaida)
    : Entidad(x, y, 28.0f, 28.0f), recogido(false), velocidadCaida(velCaida) {
    vy = velocidadCaida;
}

Gift::~Gift() {
}

void Gift::actualizar(float dt) {
    if (!activo || recogido) {
        return;
    }
    // Si tiene velocidad de caída (ej. soltado por un enemigo), desciende
    y += vy * dt;
}

bool Gift::fueRecogido() const {
    return recogido;
}

void Gift::marcarRecogido() {
    recogido = true;
    desactivar();
}

void Gift::setVelocidadCaida(float vel) {
    velocidadCaida = vel;
    vy = vel;
}