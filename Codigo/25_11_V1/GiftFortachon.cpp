#include "GiftFortachon.h"
#include "Personaje.h"

GiftFortachon::GiftFortachon(float x, float y, float duracion)
    : Gift(x, y, 90.0f), duracionInvulnerable(duracion) {
}

GiftFortachon::~GiftFortachon() {
}

void GiftFortachon::aplicarEfecto(Personaje& p) {
    p.activarFortachon(duracionInvulnerable);
}