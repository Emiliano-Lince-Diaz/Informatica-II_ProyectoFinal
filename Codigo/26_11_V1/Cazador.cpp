#include "Cazador.h"
#include "GiftFortachon.h"
#include <cstdlib>

Cazador::Cazador(float x, float y, short orilla, const Dificultad& d, float distanciaOrilla)
    : Enemigo(x, y, d, 42.0f, 56.0f, 720.0f, 1),   // tipoProyectil = 1 (lanza)
    orilla(orilla),
    distanciaOrilla(distanciaOrilla) {
    // Los cazadores disparan bastante menos seguido que los monos: en el río Kapax no puede
    // refugiarse en el suelo y se mueve en 2 ejes, así que cada lanza cuenta más
    cadencia *= 2.0f;
    // Primer lanzamiento retrasado y desfasado (1 a 3 s extra) para que no disparen todos a la vez
    tiempoDesdeDisparo = -(1.0f + 2.0f * (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)));
}

Cazador::~Cazador() {
}

bool Cazador::estaEnPantalla() const {
    return (y + alto > 0.0f) && (y < 590.0f);
}

void Cazador::actualizar(float dt, Personaje& p) {
    if (estaEnPantalla()) {
        // Ciclo completo: cadencia, agente inteligente (percepción -> razonamiento -> acción),
        // evaluación de proyectiles y aprendizaje
        Enemigo::actualizar(dt, p);
    } else {
        // Fuera de pantalla no lanza, pero sus proyectiles en vuelo se siguen evaluando
        Enemigo::actualizar(dt);
        evaluarProyectiles(p, dt);
        limpiarProyectiles();
    }
}

Gift* Cazador::soltarGift() {
    if (!estaEnPantalla()) {
        return nullptr;
    }

    float aleatorio = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    if (aleatorio > probabilidadGift) {
        return nullptr;
    }

    // El cazador arroja el gift al agua, frente a él, para que Kapax pueda alcanzarlo nadando
    float desplazamiento = distanciaOrilla + 50.0f
                           + (static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX)) * 110.0f;
    float gx = (orilla == 0) ? (x + ancho + desplazamiento)
                             : (x - desplazamiento - 28.0f);
    return new GiftFortachon(gx, y + alto * 0.4f, 6.0f);
}

short Cazador::getOrilla() const { return orilla; }
float Cazador::getDistanciaOrilla() const { return distanciaOrilla; }
