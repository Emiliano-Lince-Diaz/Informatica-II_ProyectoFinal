#include "Vortice.h"
#include "Personaje.h"
#include <cmath>

Vortice::Vortice(float x, float y, float diametro, float velocidad)
    : Trampa(x, y, diametro, diametro, 3), velocidadCorriente(velocidad) {
}

void Vortice::actualizar(float dt) {
    if (!activo) {
        return;
    }

    y += velocidadCorriente * dt;
    if (y > 600.0f) {
        y = -alto;
    }
}

void Vortice::aplicarEfecto(Personaje& p, float dt) {
    if (!activo || !p.estaVivo()) {
        return;
    }

    float dx = x + ancho * 0.5f - (p.getX() + p.getAncho() * 0.5f);
    float dy = y + alto * 0.5f - (p.getY() + p.getAlto() * 0.5f);
    float distancia = std::sqrt(dx * dx + dy * dy);

    if (distancia <= ancho * 0.18f) {
        p.recibirDano(dano);
        desactivar();
    } else if (distancia > 0.0f) {
        const float velocidadAtraccion = 90.0f;
        p.mover(dx / distancia * velocidadAtraccion * dt,
                dy / distancia * velocidadAtraccion * dt);
    }
}

void Vortice::setVelocidadCorriente(float velocidad) {
    velocidadCorriente = velocidad;
}
