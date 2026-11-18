#include "Viscosidad.h"
#include <cmath>

Viscosidad::Viscosidad(float coef, float velMin)
    : ModeloFisico(), coeficiente(coef), velocidadMinima(velMin) {
}

Viscosidad::~Viscosidad() {
}

void Viscosidad::aplicar(Entidad& e, float dt) {
    tiempo += dt;

    // Fuerza de arrastre viscoso: reduce progresivamente la velocidad horizontal
    float factor = 1.0f - (coeficiente * dt * 5.0f);
    if (factor < 0.2f) {
        factor = 0.2f;
    }

    float vxActual = e.getVx() * factor;

    // Respetar el límite de velocidad mínima mientras el jugador intente moverse en el lodo
    if (std::abs(vxActual) > 0.01f && std::abs(vxActual) < velocidadMinima) {
        vxActual = (vxActual > 0.0f) ? velocidadMinima : -velocidadMinima;
    }

    e.setVx(vxActual);
}

float Viscosidad::getCoeficiente() const { return coeficiente; }
float Viscosidad::getVelocidadMinima() const { return velocidadMinima; }