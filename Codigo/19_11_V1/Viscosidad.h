#ifndef VISCOSIDAD_H
#define VISCOSIDAD_H

#include "ModeloFisico.h"

class Viscosidad : public ModeloFisico {
private:
    float coeficiente;
    float velocidadMinima;

public:
    Viscosidad(float coef = 0.65f, float velMin = 45.0f);
    ~Viscosidad() override;

    void aplicar(Entidad& e, float dt) override;

    float getCoeficiente() const;
    float getVelocidadMinima() const;
};

#endif // VISCOSIDAD_H