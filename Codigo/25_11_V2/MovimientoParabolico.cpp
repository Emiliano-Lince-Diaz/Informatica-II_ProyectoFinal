#include "MovimientoParabolico.h"
#include <cmath>

MovimientoParabolico::MovimientoParabolico(float x0, float y0, float v0, float angulo, float gravedad)
    : ModeloFisico(), x0(x0), y0(y0), v0(v0), angulo(angulo), gravedad(gravedad) {
}

MovimientoParabolico::~MovimientoParabolico() {
}

void MovimientoParabolico::aplicar(Entidad& e, float dt) {
    tiempo += dt;

    float nuevoX = posicionX(tiempo);
    float nuevoY = posicionY(tiempo);

    // Actualizar velocidades instantáneas en la entidad
    e.setVx(v0 * std::cos(angulo));
    e.setVy(-v0 * std::sin(angulo) + gravedad * tiempo);

    // Actualizar posición según la trayectoria parabólica
    e.setX(nuevoX);
    e.setY(nuevoY);
}

float MovimientoParabolico::posicionX(float t) const {
    return x0 + v0 * std::cos(angulo) * t;
}

float MovimientoParabolico::posicionY(float t) const {
    // En Qt Y crece hacia abajo: restamos el impulso inicial hacia arriba y sumamos la caída por gravedad
    return y0 - (v0 * std::sin(angulo) * t) + (0.5f * gravedad * t * t);
}

void MovimientoParabolico::configurar(float nuevoX0, float nuevoY0, float nuevoV0, float nuevoAngulo) {
    x0 = nuevoX0;
    y0 = nuevoY0;
    v0 = nuevoV0;
    angulo = nuevoAngulo;
    reiniciar();
}