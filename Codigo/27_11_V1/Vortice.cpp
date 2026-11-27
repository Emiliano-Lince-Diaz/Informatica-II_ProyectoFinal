#include "Vortice.h"
#include <cmath>
#include <stdexcept>

namespace {
const float PI = 3.14159265f;
const float FACTOR_SUMIDERO = 0.30f;   // velocidad radial hacia el centro = 30 % de la tangencial
const float VELOCIDAD_MAXIMA = 420.0f; // límite para que el arrastre nunca sea absurdo
}

Vortice::Vortice(float cx, float cy, float circulacion, float radio)
    : ModeloFisico(),
    centroX(cx),
    centroY(cy),
    circulacion(circulacion),
    radioNucleo(radio * 0.30f),
    radioInfluencia(radio) {
    if (!(radio > 0.0f)) {
        throw std::invalid_argument("El radio de influencia del vortice debe ser mayor que cero");
    }
}

Vortice::~Vortice() {
}

void Vortice::aplicar(Entidad& e, float dt) {
    tiempo += dt;

    float ex = e.getX() + e.getAncho() * 0.5f;
    float ey = e.getY() + e.getAlto() * 0.5f;
    float dx = ex - centroX;
    float dy = ey - centroY;
    float r = std::sqrt(dx * dx + dy * dy);

    if (r > radioInfluencia || r < 0.001f) {
        return;
    }

    // Velocidad tangencial (modelo de Rankine)
    float vt;
    if (r <= radioNucleo) {
        vt = circulacion * r / (2.0f * PI * radioNucleo * radioNucleo);
    } else {
        vt = circulacion / (2.0f * PI * r);
    }
    float vr = vt * FACTOR_SUMIDERO;

    // Vector tangencial (giro antihorario en pantalla) y vector radial hacia el centro
    float tx = -dy / r;
    float ty = dx / r;
    float nx = -dx / r;
    float ny = -dy / r;

    float vx = vt * tx + vr * nx;
    float vy = vt * ty + vr * ny;

    float modulo = std::sqrt(vx * vx + vy * vy);
    if (modulo > VELOCIDAD_MAXIMA) {
        float k = VELOCIDAD_MAXIMA / modulo;
        vx *= k;
        vy *= k;
    }

    // Se desplaza la entidad (la velocidad propia del nadador se integra aparte)
    e.setX(e.getX() + vx * dt);
    e.setY(e.getY() + vy * dt);
}

bool Vortice::dentroDelNucleo(const Entidad& e) const {
    float ex = e.getX() + e.getAncho() * 0.5f;
    float ey = e.getY() + e.getAlto() * 0.5f;
    float dx = ex - centroX;
    float dy = ey - centroY;
    return (dx * dx + dy * dy) <= (radioNucleo * radioNucleo);
}

void Vortice::setCentro(float cx, float cy) {
    centroX = cx;
    centroY = cy;
}

float Vortice::getCentroX() const { return centroX; }
float Vortice::getCentroY() const { return centroY; }
float Vortice::getCirculacion() const { return circulacion; }
float Vortice::getRadioNucleo() const { return radioNucleo; }
float Vortice::getRadioInfluencia() const { return radioInfluencia; }
