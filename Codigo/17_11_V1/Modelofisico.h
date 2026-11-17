#ifndef MODELOFISICO_H
#define MODELOFISICO_H

#include "Entidad.h"

class ModeloFisico {
protected:
    float tiempo;

public:
    ModeloFisico();
    virtual ~ModeloFisico();

    // Método virtual puro para aplicar polimorfismo en las físicas
    virtual void aplicar(Entidad& e, float dt) = 0;
    virtual void reiniciar();

    float getTiempo() const;
};

#endif // MODELOFISICO_H