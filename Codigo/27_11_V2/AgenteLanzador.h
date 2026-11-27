#ifndef AGENTELANZADOR_H
#define AGENTELANZADOR_H

#include "Percepcion.h"
#include "Razonamiento.h"
#include "Accion.h"
#include "Aprendizaje.h"
#include "Dificultad.h"
#include "Entidad.h"
#include "Personaje.h"
#include "Proyectil.h"

class AgenteLanzador {
private:
    Percepcion* percepcion;
    Razonamiento* razonamiento;
    Accion* accion;
    Aprendizaje* aprendizaje;
    short ultimoResultado;

public:
    AgenteLanzador(const Dificultad& dif, float rangoMax = 650.0f);
    ~AgenteLanzador();

    // Ejecuta Percepción -> Razonamiento -> Acción y retorna el proyectil creado (o nullptr si está fuera de rango)
    Proyectil* ejecutarCiclo(const Entidad& origen, const Personaje& jugador, short tipoProyectil);

    // Ejecuta la fase de Aprendizaje cuando el proyectil impactó o salió de pantalla
    void evaluarProyectil(Proyectil* p);

    void actualizar(float dt);
    void configurarDificultad(const Dificultad& dif);
    bool estaAnimandoAtaque() const;
};

#endif // AGENTELANZADOR_H