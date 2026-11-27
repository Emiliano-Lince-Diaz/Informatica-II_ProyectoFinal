#include "TroncoFlotante.h"
#include "Personaje.h"
#include <cstdlib>

TroncoFlotante::TroncoFlotante(float x, float y, float ancho, float alto)
    : Trampa(x, y, ancho, alto, 3, 0.0f), velocidadDeriva(0.0f) {
    // Deriva propia entre -20 y +45 px/s respecto a la corriente
    float r = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    velocidadDeriva = -20.0f + r * 65.0f;

    // Vaivén lateral suave (10..30 px/s) hacia un lado u otro
    float v = 10.0f + (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 20.0f;
    vx = (std::rand() % 2 == 0) ? v : -v;
}

TroncoFlotante::~TroncoFlotante() {
}

void TroncoFlotante::actualizar(float dt) {
    if (!activo) {
        return;
    }
    y += velocidadDeriva * dt;
    x += vx * dt;
}

void TroncoFlotante::aplicarEfecto(Personaje& p, float dt) {
    (void)dt;
    if (!activo || !p.estaVivo()) {
        return;
    }

    // Trampa mortal: pierde una vida directamente (el Fortachón solo absorbe el primer golpe)
    if (p.isInvulnerable()) {
        p.recibirDano(1);
    }
    p.recibirDano(dano);
    desactivar();
}

float TroncoFlotante::getVelocidadDeriva() const { return velocidadDeriva; }
