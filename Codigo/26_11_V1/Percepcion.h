#ifndef PERCEPCION_H
#define PERCEPCION_H

#include "Entidad.h"
#include "Personaje.h"

class Percepcion {
private:
    float jugadorX;
    float jugadorY;
    float jugadorVx;
    float jugadorVy;
    float distancia;
    short estadoJugador;     // 0: suelo, 1: aire, 2: lodazal, 3: aturdido, 4: invulnerable
    bool enRango;
    float rangoMaximo;
    short ultimoResultado;   // 0: sin dato, 1: acertó, 2: pasó cerca, 3: falló

public:
    Percepcion(float rangoMax = 650.0f);
    ~Percepcion();

    void observar(const Entidad& origen, const Personaje& jugador, short resultadoPrevio);

    float getJugadorX() const;
    float getJugadorY() const;
    float getJugadorVx() const;
    float getJugadorVy() const;
    float getDistancia() const;
    short getEstadoJugador() const;
    bool estaEnRango() const;
    short getUltimoResultado() const;
    void setRangoMaximo(float rango);
};

#endif // PERCEPCION_H