#include "Resbaladero.h"
#include "Personaje.h"
#include "Friccion.h"
#include <cmath>

Resbaladero::Resbaladero(float x, float y, float ancho, float alto, float coef, float duracion)
    : Trampa(x, y, ancho, alto, 0, 0.0f), coeficienteFriccion(coef), duracionDeslizamiento(duracion) {
    modelo = new Friccion(coeficienteFriccion, 980.0f);
}

Resbaladero::~Resbaladero() {
}

void Resbaladero::aplicarEfecto(Personaje& p, float dt) {
    if (!activo || !p.estaVivo()) {
        return;
    }

    // Si entra al resbaladero moviéndose, recibe un pequeño impulso de deslizamiento
    if (std::abs(p.getVx()) < 60.0f) {
        float dir = (p.getVx() >= 0.0f) ? 1.0f : -1.0f;
        p.setVx(dir * 230.0f);
    }

    p.modificarFactorVelocidad(1.25f);

    if (modelo != nullptr) {
        modelo->aplicar(p, dt);
    }
}

float Resbaladero::getCoeficienteFriccion() const { return coeficienteFriccion; }
float Resbaladero::getDuracionDeslizamiento() const { return duracionDeslizamiento; }