#include "TroncoFlotante.h"
#include "Personaje.h"

TroncoFlotante::TroncoFlotante(float x, float y, float velocidad)
    : Trampa(x, y, 120.0f, 30.0f, 3), velocidadCorriente(velocidad) {
}

void TroncoFlotante::actualizar(float dt) {
    if (!activo) {
        return;
    }

    y += velocidadCorriente * dt;
    if (y > 600.0f) {
        y = -alto;
    }
}

void TroncoFlotante::aplicarEfecto(Personaje& p, float dt) {
    (void)dt;
    if (activo && p.estaVivo()) {
        p.recibirDano(dano);
        desactivar();
    }
}

void TroncoFlotante::setVelocidadCorriente(float velocidad) {
    velocidadCorriente = velocidad;
}
