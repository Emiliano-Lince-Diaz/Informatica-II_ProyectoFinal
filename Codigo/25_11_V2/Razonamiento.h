#ifndef RAZONAMIENTO_H
#define RAZONAMIENTO_H

#include "Percepcion.h"
#include <vector>

class Razonamiento {
private:
    float epsilon;            // Probabilidad de exploración aleatoria (ej. 0.20 = 20%)
    float gravedad;
    short estrategiaElegida;  // 0: directo, 1: anticipación, 2: ángulo alto, 3: ángulo bajo
    float v0Calculada;
    float anguloCalculado;    // En radianes

public:
    Razonamiento(float epsilon = 0.20f, float gravedad = 980.0f);
    ~Razonamiento();

    short determinarContexto(const Percepcion& p) const;
    short elegirEstrategia(const std::vector<float>& puntajesContexto);
    void calcularTiro(float origenX, float origenY, const Percepcion& p, short estrategia, float factorAnticipacion);

    short getEstrategiaElegida() const;
    float getV0Calculada() const;
    float getAnguloCalculado() const;
    void setEpsilon(float nuevoEpsilon);
};

#endif // RAZONAMIENTO_H