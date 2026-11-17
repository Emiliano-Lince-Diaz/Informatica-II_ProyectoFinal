#ifndef MOVIMIENTOPARABOLICO_H
#define MOVIMIENTOPARABOLICO_H

#include "ModeloFisico.h"

class MovimientoParabolico : public ModeloFisico {
private:
    float x0;
    float y0;
    float v0;
    float angulo;   // En radianes
    float gravedad;

public:
    MovimientoParabolico(float x0 = 0.0f, float y0 = 0.0f, float v0 = 0.0f, float angulo = 0.0f, float gravedad = 980.0f);
    ~MovimientoParabolico() override;

    void aplicar(Entidad& e, float dt) override;
    float posicionX(float t) const;
    float posicionY(float t) const;

    void configurar(float nuevoX0, float nuevoY0, float nuevoV0, float nuevoAngulo);
};

#endif // MOVIMIENTOPARABOLICO_H