#include <string>

#include "process.h"

// Constructor con parámetros
process::process(std::string id, int at, int bt) {
    this->ID = id;
    this->At = at;
    this->Bt = bt;
}

// Obtener el ID del proceso
string process::getID() {
    return this->ID;
}

// Obtener el tiempo de llegada (AT)
int process::getAT() {
    return this->At;
}

// Obtener el tiempo de ejecución (BT)
int process::getBT() {
    return this->Bt;
}

// Modificar el tiempo de ejecución (BT)
void process::setBT(int newBT) {
    this->Bt = newBT;
}
