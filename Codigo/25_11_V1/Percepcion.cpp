#include "Percepcion.h"
#include <cmath>

Percepcion::Percepcion(float rangoMax)
    : jugadorX(0.0f),
    jugadorY(0.0f),
    jugadorVx(0.0f),
    jugadorVy(0.0f),
    distancia(99999.0f),
    estadoJugador(0),
    enRango(false),
    rangoMaximo(rangoMax),
    ultimoResultado(0) {
}

Percepcion::~Percepcion() {
}

void Percepcion::observar(const Entidad& origen, const Personaje& jugador, short resultadoPrevio) {
    jugadorX = jugador.getX() + jugador.getAncho() * 0.5f;
    jugadorY = jugador.getY() + jugador.getAlto() * 0.5f;
    jugadorVx = jugador.getVx();
    jugadorVy = jugador.getVy();

    float origenX = origen.getX() + origen.getAncho() * 0.5f;
    float origenY = origen.getY() + origen.getAlto() * 0.5f;

    float dx = jugadorX - origenX;
    float dy = jugadorY - origenY;
    distancia = std::sqrt(dx * dx + dy * dy);

    enRango = (distancia <= rangoMaximo) && jugador.estaVivo();
    estadoJugador = jugador.estadoActual();
    ultimoResultado = resultadoPrevio;
}

float Percepcion::getJugadorX() const { return jugadorX; }
float Percepcion::getJugadorY() const { return jugadorY; }
float Percepcion::getJugadorVx() const { return jugadorVx; }
float Percepcion::getJugadorVy() const { return jugadorVy; }
float Percepcion::getDistancia() const { return distancia; }
short Percepcion::getEstadoJugador() const { return estadoJugador; }
bool Percepcion::estaEnRango() const { return enRango; }
short Percepcion::getUltimoResultado() const { return ultimoResultado; }
void Percepcion::setRangoMaximo(float rango) { rangoMaximo = rango; }