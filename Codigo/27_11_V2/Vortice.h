#ifndef VORTICE_H
#define VORTICE_H

#include "Modelofisico.h"

// Vórtice de Rankine: dentro del núcleo gira como sólido rígido y fuera la velocidad
// tangencial decae como Γ / (2π r). Además tiene un pequeño sumidero radial que
// arrastra a la entidad hacia el centro ("se lo traga el remolino").
class Vortice : public ModeloFisico {
private:
    float centroX;
    float centroY;
    float circulacion;
    float radioNucleo;
    float radioInfluencia;

public:
    Vortice(float cx = 0.0f, float cy = 0.0f, float circulacion = 0.0f, float radio = 85.0f);
    ~Vortice() override;

    void aplicar(Entidad& e, float dt) override;
    bool dentroDelNucleo(const Entidad& e) const;
    void setCentro(float cx, float cy);

    float getCentroX() const;
    float getCentroY() const;
    float getCirculacion() const;
    float getRadioNucleo() const;
    float getRadioInfluencia() const;
};

#endif // VORTICE_H
