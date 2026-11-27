#include "ModeloFisico.h"

ModeloFisico::ModeloFisico() : tiempo(0.0f) {
}

ModeloFisico::~ModeloFisico() {
}

void ModeloFisico::reiniciar() {
    tiempo = 0.0f;
}

float ModeloFisico::getTiempo() const {
    return tiempo;
}