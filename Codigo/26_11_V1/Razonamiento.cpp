#include "Razonamiento.h"
#include <cmath>
#include <cstdlib>

Razonamiento::Razonamiento(float epsilon, float gravedad, float imprecision)
    : epsilon(epsilon),
    gravedad(gravedad),
    imprecision(imprecision),
    estrategiaElegida(0),
    v0Calculada(300.0f),
    anguloCalculado(0.785f) {
}

Razonamiento::~Razonamiento() {
}

short Razonamiento::determinarContexto(const Percepcion& p) const {
    short ctx = p.getEstadoJugador();
    if (ctx < 0 || ctx > 4) {
        return 0;
    }
    return ctx;
}

short Razonamiento::elegirEstrategia(const std::vector<float>& puntajesContexto) {
    short numEstrategias = static_cast<short>(puntajesContexto.size());
    if (numEstrategias <= 0) {
        estrategiaElegida = 0;
        return 0;
    }

    // Exploración: ¿Número aleatorio < epsilon?
    float aleatorio = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    if (aleatorio < epsilon) {
        estrategiaElegida = static_cast<short>(std::rand() % numEstrategias);
        return estrategiaElegida;
    }

    // Explotación: elegir la estrategia con mayor puntaje aprendido en este contexto
    short mejorIdx = 0;
    float mejorPuntaje = puntajesContexto[0];
    for (short i = 1; i < numEstrategias; ++i) {
        if (puntajesContexto[i] > mejorPuntaje) {
            mejorPuntaje = puntajesContexto[i];
            mejorIdx = i;
        }
    }

    estrategiaElegida = mejorIdx;
    return estrategiaElegida;
}

void Razonamiento::calcularTiro(float origenX, float origenY, const Percepcion& p, short estrategia, float factorAnticipacion) {
    float objetivoX = p.getJugadorX();
    float objetivoY = p.getJugadorY();

    float dist = p.getDistancia();
    float tiempoVuelo = dist / 390.0f;
    if (tiempoVuelo < 0.40f) tiempoVuelo = 0.40f;
    if (tiempoVuelo > 1.45f) tiempoVuelo = 1.45f;

    if (estrategia == 1) {
        // Anticipación según velocidad de Kapax y factor aprendido
        objetivoX += p.getJugadorVx() * tiempoVuelo * factorAnticipacion;
    } else if (estrategia == 2) {
        // Ángulo más alto
        tiempoVuelo *= 1.35f;
        objetivoY -= 25.0f;
    } else if (estrategia == 3) {
        // Ángulo más bajo
        tiempoVuelo *= 0.78f;
    }

    // Aplicar margen de error humano/natural según la dificultad para no ser infalible
    if (imprecision > 0.0f) {
        float randNorm = (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 2.0f - 1.0f;
        objetivoX += randNorm * imprecision;
    }

    float dx = objetivoX - origenX;
    float dy = objetivoY - origenY;

    float vx0 = dx / tiempoVuelo;
    float vy0 = (0.5f * gravedad * tiempoVuelo * tiempoVuelo - dy) / tiempoVuelo;

    v0Calculada = std::sqrt(vx0 * vx0 + vy0 * vy0);
    anguloCalculado = std::atan2(vy0, vx0);
}

short Razonamiento::getEstrategiaElegida() const { return estrategiaElegida; }
float Razonamiento::getV0Calculada() const { return v0Calculada; }
float Razonamiento::getAnguloCalculado() const { return anguloCalculado; }
void Razonamiento::setEpsilon(float nuevoEpsilon) { epsilon = nuevoEpsilon; }
void Razonamiento::setImprecision(float nuevaImprecision) { imprecision = nuevaImprecision; }