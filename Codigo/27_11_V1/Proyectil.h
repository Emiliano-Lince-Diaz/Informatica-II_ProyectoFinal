#ifndef PROYECTIL_H
#define PROYECTIL_H

#include "Entidad.h"
#include "ModeloFisico.h"

class Personaje;

class Proyectil : public Entidad {
private:
    short dano;
    short tipo;              // 0: piedra/coco, 1: lanza, 2: jabalina
    float tiempoVida;
    float distanciaMinima;
    float errorHorizontal;   // Diferencia en X (jugador - proyectil) en el punto más cercano
    short estado;            // 0: en vuelo, 1: acertó, 2: pasó cerca, 3: falló
    bool evaluado;
    ModeloFisico* modelo;

public:
    Proyectil(float x = 0.0f, float y = 0.0f, float v0 = 300.0f, float angulo = 0.785f, short tipo = 0, short dano = 1);
    ~Proyectil() override;

    void actualizar(float dt) override;
    void evaluarImpacto(Personaje& p);
    bool termino() const;
    bool salioDeLimites(float minX, float maxX, float maxY) const;

    // Getters para el agente y la interfaz gráfica
    short obtenerResultado() const;
    float obtenerDistanciaMinima() const;
    float obtenerErrorHorizontal() const;
    short getTipo() const;
    bool fueEvaluado() const;
    void marcarEvaluado();
};

#endif // PROYECTIL_H