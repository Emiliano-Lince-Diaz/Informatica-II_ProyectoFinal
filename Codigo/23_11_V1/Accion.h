#ifndef ACCION_H
#define ACCION_H

#include "Proyectil.h"

class Accion {
private:
    bool animando;
    float tiempoAnimacion;
    short ultimoContexto;
    short ultimaEstrategia;

public:
    Accion();
    ~Accion();

    Proyectil* lanzar(float x, float y, float v0, float angulo, short tipo, short contexto, short estrategia);
    void actualizar(float dt);

    bool estaAnimando() const;
    short getUltimoContexto() const;
    short getUltimaEstrategia() const;
};

#endif // ACCION_H