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
void registrarMateria(sqlite3* db) {
    string nombreMateria;
    cout << "\n--- Registrar Nueva Materia ---" << endl;
    cout << "Nombre de la materia: ";
    cin.ignore(); // Limpia el buffer del teclado
    getline(cin, nombreMateria); // Permite nombres con espacios como "Algebra Lineal"

    string sql = "INSERT INTO Materias (nombre_materia) VALUES ('" + nombreMateria + "');";

    char* mensajeError = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &mensajeError);

    if (rc != SQLITE_OK) {
        cerr << "Error: " << mensajeError << endl;
        sqlite3_free(mensajeError);
    } else {
        cout << "Materia guardada con éxito." << endl;
    }
}
void listarMaterias(sqlite3* db) {
    cout << "\n--- Catalogo de Materias ---" << endl;
    const char* sql = "SELECT * FROM Materias;";
    sqlite3_exec(db, sql, imprimirFila, 0, 0);
}
void asignarNota(sqlite3* db) {
    int idEstudiante, idMateria;
    double nota;

    cout << "\n--- Asignar Calificacion ---" << endl;
    
    // Primero, mostramos quiénes están para que el usuario sepa los IDs
    cout << "ID del Estudiante: ";
    cin >> idEstudiante;
    
    cout << "ID de la Materia: ";
    cin >> idMateria;
    
    cout << "Nota Final: ";
    cin >> nota;

    // Creamos la consulta SQL
    // Convertimos los números a string automáticamente al concatenar
    string sql = "INSERT INTO Calificaciones (id_estudiante, id_materia, nota) VALUES (" 
                 + to_string(idEstudiante) + ", " 
                 + to_string(idMateria) + ", " 
                 + to_string(nota) + ");";

    char* mensajeError = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &mensajeError);

    if (rc != SQLITE_OK) {
        cerr << "Error al asignar nota: " << mensajeError << endl;
        sqlite3_free(mensajeError);
    } else {
        cout << "Nota registrada correctamente!" << endl;
    }
}
void verReporteNotas(sqlite3* db) {
    cout << "\n--- REPORTE DE CALIFICACIONES ---" << endl;
    
    // Esta consulta junta las 3 tablas para mostrar nombres en lugar de puros IDs
    const char* sql = "SELECT Estudiantes.nombre, Estudiantes.apellido, "
                      "Materias.nombre_materia, Calificaciones.nota "
                      "FROM Calificaciones "
                      "INNER JOIN Estudiantes ON Calificaciones.id_estudiante = Estudiantes.id_estudiante "
                      "INNER JOIN Materias ON Calificaciones.id_materia = Materias.id_materia;";

    char* mensajeError = 0;
    sqlite3_exec(db, sql, imprimirFila, 0, &mensajeError);
}
// 3. FUNCIÓN PRINCIPAL (El corazón del programa)
int main() {
    sqlite3* db;
    sqlite3_open("sistema_academico.db", &db);

    int opcion;
    do {
        cout << "\n==== SISTEMA NEXUS ACADEMICO ====" << endl;
        cout << "1. Registrar Estudiante" << endl;
        cout << "2. Ver Estudiantes" << endl;
        cout << "3. Registrar Materia" << endl;
        cout << "4. Ver Materias" << endl;
        cout << "5. Asignar Nota" << endl;
        cout << "6. Ver Reporte de Notas" << endl;
        cout << "0. Salir" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        switch(opcion) {
            case 1: registrarEstudiante(db); break;
            case 2: listarEstudiantes(db); break;
            case 3: registrarMateria(db); break;
            case 4: listarMaterias(db); break;
            case 5: asignarNota(db); break;
            case 6: verReporteNotas(db); break;
        }
    } while (opcion != 0);

    sqlite3_close(db);
    return 0;
}