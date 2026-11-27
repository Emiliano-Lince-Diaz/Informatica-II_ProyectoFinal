#include "Friccion.h"
#include <cmath>

Friccion::Friccion(float coef, float g)
    : ModeloFisico(), coeficiente(coef), gravedad(g) {
}

Friccion::~Friccion() {
}

void Friccion::aplicar(Entidad& e, float dt) {
    tiempo += dt;

    float vxActual = e.getVx();
    if (std::abs(vxActual) > 0.001f) {
        // Desaceleración por fricción cinética: dv = mu * g * dt
        float desaceleracion = coeficiente * gravedad * dt;

        if ( std::abs(vxActual) <= desaceleracion ) {
            vxActual = 0.0f;
        } else {
            vxActual -= (vxActual > 0.0f ? desaceleracion : -desaceleracion);
        }

        e.setVx(vxActual);
    }
}

float Friccion::getCoeficiente() const { return coeficiente; }