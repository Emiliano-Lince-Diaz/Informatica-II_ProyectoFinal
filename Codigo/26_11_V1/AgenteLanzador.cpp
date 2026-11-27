#include "AgenteLanzador.h"

AgenteLanzador::AgenteLanzador(const Dificultad& dif, float rangoMax)
    : percepcion(new Percepcion(rangoMax)),
    razonamiento(new Razonamiento(0.25f, 980.0f, 65.0f)),
    accion(new Accion()),
    aprendizaje(new Aprendizaje(dif.getTasaAprendizaje(), 20)),
    ultimoResultado(0) {
    configurarDificultad(dif);
}

AgenteLanzador::~AgenteLanzador() {
    delete percepcion;
    delete razonamiento;
    delete accion;
    delete aprendizaje;
}

Proyectil* AgenteLanzador::ejecutarCiclo(const Entidad& origen, const Personaje& jugador, short tipoProyectil) {
    // 1. Percepción
    percepcion->observar(origen, jugador, ultimoResultado);
    if (!percepcion->estaEnRango()) {
        return nullptr;
    }

    // 2. Razonamiento
    short contexto = razonamiento->determinarContexto(*percepcion);
    const std::vector<float>& puntajes = aprendizaje->obtenerPuntajes(contexto);
    short estrategia = razonamiento->elegirEstrategia(puntajes);

    float origenX = origen.getX() + origen.getAncho() * 0.5f;
    float origenY = origen.getY() + origen.getAlto() * 0.35f;
    razonamiento->calcularTiro(origenX, origenY, *percepcion, estrategia, aprendizaje->getFactorAnticipacion());

    // 3. Acción
    return accion->lanzar(
        origenX,
        origenY,
        razonamiento->getV0Calculada(),
        razonamiento->getAnguloCalculado(),
        tipoProyectil,
        contexto,
        estrategia
        );
}

void AgenteLanzador::evaluarProyectil(Proyectil* p) {
    if (p == nullptr || !p->termino() || p->fueEvaluado()) {
        return;
    }

    // 4. Aprendizaje
    short resultado = p->obtenerResultado();
    float distMin = p->obtenerDistanciaMinima();
    float errorX = p->obtenerErrorHorizontal();

    aprendizaje->aprender(
        accion->getUltimoContexto(),
        accion->getUltimaEstrategia(),
        resultado,
        distMin,
        errorX
        );

    ultimoResultado = resultado;
    p->marcarEvaluado();
}

void AgenteLanzador::actualizar(float dt) {
    if (accion != nullptr) {
        accion->actualizar(dt);
    }
}

void AgenteLanzador::configurarDificultad(const Dificultad& dif) {
    if (aprendizaje != nullptr) {
        aprendizaje->setTasaAprendizaje(dif.getTasaAprendizaje());
    }
    if (razonamiento != nullptr) {
        short niv = dif.obtenerNivel();
        if (niv == 1) {
            // Fácil: falla seguido y prueba estrategias al azar con frecuencia
            razonamiento->setEpsilon(0.55f);
            razonamiento->setImprecision(150.0f);
        } else if (niv == 2) {
            // Normal: margen moderado
            razonamiento->setEpsilon(0.25f);
            razonamiento->setImprecision(65.0f);
        } else {
            // Difícil: muy preciso
            razonamiento->setEpsilon(0.10f);
            razonamiento->setImprecision(15.0f);
        }
    }
}

bool AgenteLanzador::estaAnimandoAtaque() const {
    return (accion != nullptr) ? accion->estaAnimando() : false;
}