#ifndef FRICCION_H
#define FRICCION_H

#include "ModeloFisico.h"

class Friccion : public ModeloFisico {
private:
    float coeficiente;
    float gravedad;

public:
    Friccion(float coef = 0.12f, float g = 980.0f);
    ~Friccion() override;

    void aplicar(Entidad& e, float dt) override;

    float getCoeficiente() const;
};

#endif // FRICCION_H