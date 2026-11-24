#include "Proyectil.h"
#include "MovimientoParabolico.h"
#include "Personaje.h"
#include <cmath>

Proyectil::Proyectil(float x, float y, float v0, float angulo, short tipo, short dano)
    : Entidad(x, y, 18.0f, 18.0f),
    dano(dano),
    tipo(tipo),
    tiempoVida(0.0f),
    distanciaMinima(99999.0f),
    errorHorizontal(0.0f),
    estado(0),
    evaluado(false),
    modelo(new MovimientoParabolico(x, y, v0, angulo, 980.0f)) {
}

Proyectil::~Proyectil() {
    if (modelo != nullptr) {
        delete modelo;
        modelo = nullptr;
    }
}

void Proyectil::actualizar(float dt) {
    if (!activo) {
        return;
    }

    tiempoVida += dt;

    if (modelo != nullptr) {
        modelo->aplicar(*this, dt);
    }

    // Seguridad: desactivar si lleva demasiado tiempo volando
    if (tiempoVida > 6.0f) {
        desactivar();
    }
}

void Proyectil::evaluarImpacto(Personaje& p) {
    if (!activo || !p.estaVivo()) {
        return;
    }

    // Calcular distancia entre centros para medir precisión (cercanía) y error horizontal
    float centroPX = x + ancho * 0.5f;
    float centroPY = y + alto * 0.5f;
    float centroJX = p.getX() + p.getAncho() * 0.5f;
    float centroJY = p.getY() + p.getAlto() * 0.5f;

    float dx = centroJX - centroPX;
    float dy = centroJY - centroPY;
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist < distanciaMinima) {
        distanciaMinima = dist;
        errorHorizontal = dx;
    }

    // Verificar colisión directa con Kapax
    if (colisionaCon(p)) {
        p.recibirDano(dano);
        p.aturdir(1.0f); // Al recibir impacto de enemigo queda brevemente aturdido
        estado = 1;      // Acertó
        desactivar();
    }
}

bool Proyectil::termino() const {
    return !activo;
}

bool Proyectil::salioDeLimites(float minX, float maxX, float maxY) const {
    return (x < minX || x > maxX || y > maxY);
}

short Proyectil::obtenerResultado() const {
    if (estado == 1) {
        return 1; // Acertó
    }
    if (!activo) {
        // Si falló pero pasó a menos de 90 px del jugador, se clasifica como "pasó cerca"
        if (distanciaMinima < 90.0f) {
            return 2; // Pasó cerca
        }
        return 3; // Falló lejos
    }
    return 0; // Aún en vuelo
}

float Proyectil::obtenerDistanciaMinima() const { return distanciaMinima; }
float Proyectil::obtenerErrorHorizontal() const { return errorHorizontal; }
short Proyectil::getTipo() const { return tipo; }
bool Proyectil::fueEvaluado() const { return evaluado; }
void Proyectil::marcarEvaluado() { evaluado = true; }