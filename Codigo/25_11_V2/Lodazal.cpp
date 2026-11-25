#include "Lodazal.h"
#include "Personaje.h"
#include "Viscosidad.h"

Lodazal::Lodazal(float x, float y, float ancho, float alto, float coef)
    : Trampa(x, y, ancho, alto, 0, 0.0f), coeficienteViscosidad(coef) {
    modelo = new Viscosidad(coeficienteViscosidad, 45.0f);
}

Lodazal::~Lodazal() {
}

void Lodazal::aplicarEfecto(Personaje& p, float dt) {
    if (!activo || !p.estaVivo()) {
        return;
    }

    // Reduce el factor de velocidad del personaje (lo detecta el Agente Inteligente como estado 2: Lodazal)
    float factor = 1.0f - coeficienteViscosidad;
    if (factor < 0.35f) {
        factor = 0.35f;
    }
    p.modificarFactorVelocidad(factor);

    if (modelo != nullptr) {
        modelo->aplicar(p, dt);
    }
}

float Lodazal::getCoeficienteViscosidad() const {
    return coeficienteViscosidad;
}