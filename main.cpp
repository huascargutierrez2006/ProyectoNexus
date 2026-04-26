#include <iostream>
#include <string>
extern "C" {
    #include "sqlite3.h"
}

using namespace std;

// Función para registrar un estudiante
void registrarEstudiante(sqlite3* db) {
    string nombre, apellido;
    cout << "\n--- Registrar Nuevo Estudiante ---" << endl;
    cout << "Nombre: ";
    cin >> nombre;
    cout << "Apellido: ";
    cin >> apellido;

    // Preparamos la consulta SQL
    string sql = "INSERT INTO Estudiantes (nombre, apellido) VALUES ('" + nombre + "', '" + apellido + "');";

    char* mensajeError = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &mensajeError);

    if (rc != SQLITE_OK) {
        cerr << "Error al registrar: " << mensajeError << endl;
        sqlite3_free(mensajeError);
    } else {
        cout << "Estudiante registrado con éxito." << endl;
    }
}

int main() {
    sqlite3* db;
    if (sqlite3_open("sistema_academico.db", &db)) {
        cerr << "Error al abrir DB" << endl;
        return 1;
    }

    int opcion;
    do {
        cout << "\n==== GESTION ACADEMICA ====" << endl;
        cout << "1. Registrar Estudiante" << endl;
        cout << "0. Salir" << endl;
        cout << "Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                registrarEstudiante(db);
                break;
            case 0:
                cout << "Saliendo..." << endl;
                break;
            default:
                cout << "Opcion no valida." << endl;
        }
    } while (opcion != 0);

    sqlite3_close(db);
    return 0;
}