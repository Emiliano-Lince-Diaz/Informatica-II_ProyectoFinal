#include "Remolino.h"
#include "Personaje.h"
#include "Vortice.h"

namespace {
const float PI = 3.14159265f;
// Velocidad tangencial (px/s) justo en el borde del núcleo cuando intensidad = 1
const float VELOCIDAD_BORDE_NUCLEO = 200.0f;
}

Remolino::Remolino(float x, float y, float radio, float intensidad)
    : Trampa(x - radio, y - radio, radio * 2.0f, radio * 2.0f, 3, radio),
    radioNucleo(radio * 0.30f),
    intensidad(intensidad) {
    float circulacion = 2.0f * PI * radioNucleo * VELOCIDAD_BORDE_NUCLEO * intensidad;
    modelo = new Vortice(x, y, circulacion, radio);
}

Remolino::~Remolino() {
}

void Remolino::actualizar(float dt) {
    (void)dt;
    // El remolino es arrastrado por la corriente: mantener el centro del vórtice sincronizado
    Vortice* v = dynamic_cast<Vortice*>(modelo);
    if (v != nullptr) {
        v->setCentro(x + ancho * 0.5f, y + alto * 0.5f);
    }
}

void Remolino::aplicarEfecto(Personaje& p, float dt) {
    if (!activo || !p.estaVivo()) {
        return;
    }

    Vortice* v = dynamic_cast<Vortice*>(modelo);
    if (v == nullptr) {
        return;
    }

    // Arrastre y giro (solo actúa dentro del radio de influencia)
    v->aplicar(p, dt);

    // Si llega al núcleo, el remolino se lo traga: muerte inmediata (pierde una vida)
    if (v->dentroDelNucleo(p)) {
        if (p.isInvulnerable()) {
            p.recibirDano(1);   // el Fortachón solo absorbe el golpe, no salva de una trampa mortal
        }
        p.recibirDano(dano);
        desactivar();
    }
}

float Remolino::getRadioNucleo() const { return radioNucleo; }
float Remolino::getIntensidad() const { return intensidad; }
