#include "Razonamiento.h"
#include <cmath>
#include <cstdlib>

Razonamiento::Razonamiento(float epsilon, float gravedad)
    : epsilon(epsilon),
    gravedad(gravedad),
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

    // Tiempo de vuelo base proporcional a la distancia (acotado para evitar velocidades extremas)
    float dist = p.getDistancia();
    float tiempoVuelo = dist / 420.0f;
    if (tiempoVuelo < 0.35f) tiempoVuelo = 0.35f;
    if (tiempoVuelo > 1.40f) tiempoVuelo = 1.40f;

    // Ajustar punto objetivo y trayectoria según la estrategia seleccionada
    if (estrategia == 0) {
        // Estrategia 0: Tiro directo a la posición actual
    } else if (estrategia == 1) {
        // Estrategia 1: Con anticipación según la velocidad del jugador y el factor aprendido
        objetivoX += p.getJugadorVx() * tiempoVuelo * factorAnticipacion;
    } else if (estrategia == 2) {
        // Estrategia 2: Ángulo más alto (mayor tiempo de vuelo, ideal si el jugador salta)
        tiempoVuelo *= 1.35f;
        objetivoY -= 25.0f;
    } else if (estrategia == 3) {
        // Estrategia 3: Ángulo más bajo y rápido (menor tiempo de vuelo)
        tiempoVuelo *= 0.72f;
    }

    // Despeje exacto de MovimientoParabolico en coordenadas de Qt (Y crece hacia abajo):
    // x(t) = x0 + vx0 * t  =>  vx0 = dx / t
    // y(t) = y0 - vy0 * t + 0.5 * g * t^2  =>  vy0 = (0.5 * g * t^2 - dy) / t
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