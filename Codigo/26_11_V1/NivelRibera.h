#ifndef NIVELRIBERA_H
#define NIVELRIBERA_H

#include "Nivel.h"
#include "Plataforma.h"
#include <vector>

class NivelRibera : public Nivel {
private:
    float scrollX;
    float limiteScrollIzq;
    float limiteScrollDer;
    float anchoMundo;
    float posicionOrilla;
    float anchoVentana;
    std::vector<Plataforma*> plataformas;

    // Sistema de Checkpoint y aviso grande de pérdida de vida
    short vidasAnteriores;
    float checkpointX;
    float checkpointY;
    float posicionCheckpoint;
    bool checkpointAlcanzado;
    float tiempoMensajeVida;
    float tiempoMensajeCheckpoint;

public:
    NivelRibera(Personaje* p, short nivelDificultad, float anchoVista = 960.0f);
    ~NivelRibera() override;

    void cargar() override;
    void actualizar(float dt) override;
    bool verificarVictoria() override;

    void actualizarScroll();
    void resolverPlataformas(float dt);

    // Getters para la vista gráfica de Qt
    float getScrollX() const;
    float getAnchoMundo() const;
    float getPosicionOrilla() const;
    float getPosicionCheckpoint() const;
    bool isCheckpointAlcanzado() const;
    bool mostrandoAvisoVida() const;
    bool mostrandoAvisoCheckpoint() const;
    const std::vector<Plataforma*>& getPlataformas() const;
};

#endif // NIVELRIBERA_H