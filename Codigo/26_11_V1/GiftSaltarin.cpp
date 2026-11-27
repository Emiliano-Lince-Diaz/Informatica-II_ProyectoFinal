#include "GiftSaltarin.h"
#include "Personaje.h"

GiftSaltarin::GiftSaltarin(float x, float y, short extra)
    : Gift(x, y, 0.0f), saltosExtra(extra) {
    // Al estar en una posición estática en el mapa, no cae (velocidadCaida = 0)
}

GiftSaltarin::~GiftSaltarin() {
}

void GiftSaltarin::aplicarEfecto(Personaje& p) {
    if (saltosExtra > 0) {
        p.activarDobleSalto();
    }
}