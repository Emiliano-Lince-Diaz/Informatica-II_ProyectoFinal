#ifndef NIVELRIO_H
#define NIVELRIO_H

#include "Nivel.h"

// Nivel 2 - "Nadando el Magdalena": vista cenital fija con scroll vertical.
// Kapax nada dentro del río contra la corriente, esquiva troncos y remolinos, atraviesa
// bancos de lodo (viscosidad) y soporta los lanzamientos de las tribus de las orillas.
// Se gana al resistir el tiempo estipulado; mientras pasa el tiempo el río se vuelve
// más rápido y violento.
class NivelRio : public Nivel {
private:
    float velocidadCorriente;
    float incrementoCorriente;
    float scrollY;
    float anchoRio;
    float intervaloGeneracion;
    float tiempoDesdeGeneracion;

    // Auxiliares de la implementación
    float anchoVentana;
    float limiteRioIzq;
    float limiteRioDer;
    float velocidadInicial;
    float velocidadMaxima;
    short vidasAnteriores;
    float tiempoMensajeVida;

    float aleatorio(float minimo, float maximo) const;
    int contarRemolinosActivos() const;
    void despejarZonaReaparicion();

public:
    NivelRio(Personaje* p, short nivelDificultad, float anchoVista = 960.0f);
    ~NivelRio() override;

    void cargar() override;
    void actualizar(float dt) override;
    bool verificarVictoria() override;

    void aplicarCorriente(float dt);
    void incrementarViolencia(float dt);
    void generarObstaculos();

    // Getters para la vista gráfica de Qt
    float getVelocidadCorriente() const;
    float getVelocidadInicial() const;
    float getScrollY() const;
    float getAnchoRio() const;
    float getLimiteRioIzq() const;
    float getLimiteRioDer() const;
    float getProgreso() const;
    bool mostrandoAvisoVida() const;
};

#endif // NIVELRIO_H
