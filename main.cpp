// 1. INCLUSIONES (Librerías)
#include <iostream>
#include <string>
extern "C" {
    #include "sqlite3.h"
}

using namespace std;

// 2. FUNCIONES DE APOYO (Deben ir ARRIBA del main)

// Esta es la función de impresión que usamos para leer datos
int imprimirFila(void* data, int argc, char** argv, char** colNombre) {
    for (int i = 0; i < argc; i++) {
        cout << colNombre[i] << ": " << (argv[i] ? argv[i] : "NULL") << " | ";
    }
    cout << endl;
    return 0;
}

// Función para listar
void listarEstudiantes(sqlite3* db) {
    cout << "\n--- Lista de Estudiantes Registrados ---" << endl;
    const char* sql = "SELECT * FROM Estudiantes;";
    char* mensajeError = 0;
    sqlite3_exec(db, sql, imprimirFila, 0, &mensajeError);
}

// Función para registrar
void registrarEstudiante(sqlite3* db) {
    string nombre, apellido;
    cout << "\nNombre: "; cin >> nombre;
    cout << "\nApellido: "; cin >> apellido;
    string sql = "INSERT INTO Estudiantes (nombre, apellido) VALUES ('" + nombre + "', '" + apellido + "');";
    sqlite3_exec(db, sql.c_str(), 0, 0, 0);
    cout << "Registrado!" << endl;
}

// 3. FUNCIÓN PRINCIPAL (El corazón del programa)
int main() {
    sqlite3* db;
    sqlite3_open("sistema_academico.db", &db);

    int opcion;
    do {
        cout << "\n1. Registrar\n2. Listar\n0. Salir\nOpcion: ";
        cin >> opcion;

        if (opcion == 1) registrarEstudiante(db);
        else if (opcion == 2) listarEstudiantes(db);

    } while (opcion != 0);

    sqlite3_close(db);
    return 0;
}