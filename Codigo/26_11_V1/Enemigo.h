#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "Entidad.h"
#include "Dificultad.h"
#include "AgenteLanzador.h"
#include "Proyectil.h"
#include "Personaje.h"
#include "Gift.h"
#include <vector>

class Enemigo : public Entidad {
protected:
    float cadencia;
    float tiempoDesdeDisparo;
    float rangoAlcance;
    float probabilidadGift;
    short tipoProyectil;
    AgenteLanzador* agente;
    std::vector<Proyectil*> proyectiles;

public:
    Enemigo(float x, float y, const Dificultad& d, float ancho = 42.0f, float alto = 42.0f, float rango = 650.0f, short tipoProj = 0);
    virtual ~Enemigo() override;

    // Implementación del método virtual puro de Entidad
    void actualizar(float dt) override;

    // Ciclo completo de actualización interactuando con Kapax
    virtual void actualizar(float dt, Personaje& p);

    void lanzar(const Personaje& p);
    virtual Gift* soltarGift();
    void evaluarProyectiles(Personaje& p, float dt, float minX = -200.0f, float maxX = 4000.0f, float maxY = 900.0f);
    void limpiarProyectiles();

    // Getters útiles para la interfaz gráfica y el Nivel
    const std::vector<Proyectil*>& getProyectiles() const;
    bool estaAnimandoAtaque() const;
    short getTipoProyectil() const;
};

#endif // ENEMIGO_H