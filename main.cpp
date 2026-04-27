#include <iostream>
#include <string>
//es un codigo en c, asi q lo colocamos dentro de extern
extern "C" {
    #include "sqlite3.h"
}
// Definiciones de colores para la consola
#define RESET   "\033[00m"
#define RED     "\033[31m"      
#define GREEN   "\033[32m"      
#define YELLOW  "\033[33m"      
#define CYAN    "\033[36m"      
#define BOLD    "\033[1m"       
using namespace std;

int imprimirFila(void* data, int argc, char** argv, char** colNombre) {
    for (int i = 0; i < argc; i++) {
        cout << CYAN << colNombre[i] << ": " << RESET 
             << (argv[i] ? argv[i] : "NULL") << " | ";
    }
    cout << endl;
    return 0;
}

void listarEstudiantes(sqlite3* db) {
    cout << "\n--- Lista de Estudiantes Registrados ---" << endl;
    const char* sql = "SELECT * FROM Estudiantes;";
    char* mensajeError = 0;
    sqlite3_exec(db, sql, imprimirFila, 0, &mensajeError);
    cout << "\n---------------------------------------" << endl;
    cout << "Presione ENTER para volver al menu...";
    cin.ignore(); // Limpia cualquier residuo del teclado
    cin.get();    // Se queda esperando a que presiones una tecla
}

void registrarEstudiante(sqlite3* db) {
    string nombre, apellido;
    cout << "\nNombre: ";
    getline(cin >> ws, nombre); 
    cout << "Apellido: ";
    getline(cin >> ws, apellido);
    string sql = "INSERT INTO Estudiantes (nombre, apellido) VALUES ('" + nombre + "', '" + apellido + "');";
    sqlite3_exec(db, sql.c_str(), 0, 0, 0);
    cout << "Registrado!" << endl;
}
void registrarMateria(sqlite3* db) {
    string nombreMateria;
    cout << "\n--- Registrar Nueva Materia ---" << endl;
    cout << "Nombre de la materia: ";
    getline(cin >> ws, nombreMateria);

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
    cout << "\n--- Lista de Materias ---" << endl;
    const char* sql = "SELECT * FROM Materias;";
    sqlite3_exec(db, sql, imprimirFila, 0, 0);
    cout << "\n---------------------------------------" << endl;
    cout << "Presione ENTER para volver al menu...";
    cin.ignore(); 
    cin.get();    
}
void asignarNota(sqlite3* db) {
    int idEstudiante, idMateria;
    double nota;

    cout << "\n--- Asignar Calificacion ---" << endl;
    cout << "ID del Estudiante: ";
    cin >> idEstudiante;
    cout << "ID de la Materia: ";
    cin >> idMateria;

    // Validación de la nota
    do {
        cout << "Nota Final (0-100): ";
        cin >> nota;

        if (nota < 0 || nota > 100) {
            cout << "[!] Error: La nota debe estar entre 0 y 100. Intente de nuevo." << endl;
        }
    } while (nota < 0 || nota > 100);

    string sql = "INSERT INTO Calificaciones (id_estudiante, id_materia, nota) VALUES (" 
                 + to_string(idEstudiante) + ", " 
                 + to_string(idMateria) + ", " 
                 + to_string(nota) + ");";

    char* mensajeError = 0;
    int rc = sqlite3_exec(db, sql.c_str(), 0, 0, &mensajeError);

    if (rc != SQLITE_OK) {
        cerr << "Error: " << mensajeError << endl;
        sqlite3_free(mensajeError);
    } else {
        cout << "Nota registrada correctamente!" << endl;
    }

    cout << "Presione ENTER para continuar...";
    cin.ignore();
    cin.get();
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
    cout << "\n---------------------------------------" << endl;
    cout << "Presione ENTER para volver al menu...";
    cin.ignore(); // limpia cualquier residuo del teclado
    cin.get();    // se queda esperando a que presiones una tecla
}
void borrarEstudiante(sqlite3* db) {
    int id;
    cout << "\n--- Borrar Estudiante ---" << endl;
    cout << "ID a eliminar: ";
    cin >> id;

    string sqlNotas = "DELETE FROM Calificaciones WHERE id_estudiante = " + to_string(id) + ";";
    string sqlEstudiante = "DELETE FROM Estudiantes WHERE id_estudiante = " + to_string(id) + ";";

    sqlite3_exec(db, sqlNotas.c_str(), 0, 0, 0);
    int rc = sqlite3_exec(db, sqlEstudiante.c_str(), 0, 0, 0);

    if (rc == SQLITE_OK) {
        cout << GREEN << "Estudiante eliminado." << RESET << endl;

        
        // Si no hay filas, reiniciamos el contador de la base de datos
        const char* sqlCheck = "SELECT COUNT(*) FROM Estudiantes;";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sqlCheck, -1, &stmt, 0);
        sqlite3_step(stmt);
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);

        if (count == 0) {
            // resetear el incremento automático de IDs
            sqlite3_exec(db, "DELETE FROM sqlite_sequence WHERE name='Estudiantes';", 0, 0, 0);
            cout << YELLOW << "Tabla vacia. El siguiente ID sera 1." << RESET << endl;
        }
    }

    cout << "Presione Enter para continuar...";
    cin.ignore();
    cin.get();
}
void borrarMateria(sqlite3* db) {
    int id;
    cout << "\n--- Borrar Materia ---" << endl;
    cout << "Ingrese el ID de la materia que desea eliminar: ";
    cin >> id;
    //se borran las notas vinculadas
    string sqlNotas = "DELETE FROM Calificaciones WHERE id_materia = " + to_string(id) + ";";
    string sqlMateria = "DELETE FROM Materias WHERE id_materia = " + to_string(id) + ";";

    char* mensajeError = 0;
    sqlite3_exec(db, sqlNotas.c_str(), 0, 0, 0);
    int rc = sqlite3_exec(db, sqlMateria.c_str(), 0, 0, &mensajeError);

    if (rc == SQLITE_OK) {
        cout << GREEN << "Materia eliminada con exito." << RESET << endl;

        const char* sqlCheck = "SELECT COUNT(*) FROM Materias;";
        sqlite3_stmt* stmt;
        sqlite3_prepare_v2(db, sqlCheck, -1, &stmt, 0);
        sqlite3_step(stmt);
        int count = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);

        if (count == 0) {
            sqlite3_exec(db, "DELETE FROM sqlite_sequence WHERE name='Materias';", 0, 0, 0);
            cout << YELLOW << "Tabla de materias limpia. El proximo ID sera 1." << RESET << endl;
        }
    } else {
        cerr << RED << "Error al borrar: " << mensajeError << RESET << endl;
        sqlite3_free(mensajeError);
    }

    cout << "\nPresione Enter para continuar...";
    cin.ignore();
    cin.get();
}
void mostrarBanner() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    cout << CYAN << BOLD << "========================================" << RESET << endl;
    cout << CYAN << BOLD << "       PROYECTO NEXUS ACADEMY     " << RESET << endl;
    cout << CYAN << BOLD << "========================================" << RESET << endl;
}
int main() {
    sqlite3* db;
    int rc;

    rc = sqlite3_open("sistema_academico.db", &db);

    if (rc) {
        cerr << RED << "No se pudo abrir la base de datos: " << sqlite3_errmsg(db) << RESET << endl;
        return 1;
    }


    int opcion;
    do {
        mostrarBanner(); 

        cout << BOLD << "  --- GESTION DE DATOS ---" << RESET << endl;
        cout << YELLOW << "  1." << RESET << " Registrar Estudiante" << endl;
        cout << YELLOW << "  2." << RESET << " Ver Lista de Estudiantes" << endl;
        cout << YELLOW << "  3." << RESET << " Registrar Materia" << endl;
        cout << YELLOW << "  4." << RESET << " Ver Catalogo de Materias" << endl;
        
        cout << BOLD << "\n  --- CALIFICACIONES ---" << RESET << endl;
        cout << YELLOW << "  5." << RESET << " Asignar Nota (Validado 0-100)" << endl;
        cout << YELLOW << "  6." << RESET << " Ver Reporte Academico Completo" << endl;
        
        cout << BOLD << "\n  --- MANTENIMIENTO ---" << RESET << endl;
        cout << YELLOW << "  7." << RESET << " Borrar Estudiante" << endl;
        cout << YELLOW << "  8." << RESET << " Borrar Materia" << endl;
        
        cout << RED << "\n  0. Salir del Sistema" << RESET << endl;
        
        cout << "\n> Seleccione una opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1: registrarEstudiante(db); break;
            case 2: listarEstudiantes(db); break;
            case 3: registrarMateria(db); break;
            case 4: listarMaterias(db); break;
            case 5: asignarNota(db); break;
            case 6: verReporteNotas(db); break;
            case 7: borrarEstudiante(db); break;
            case 8: borrarMateria(db); break;
            case 0: 
                cout << GREEN << "\nSaliendo de Nexus... Hasta pronto!" << RESET << endl; 
                break;
            default: 
                cout << RED << "\n[!] Opción no válida. Intente de nuevo." << RESET << endl;
                cout << "Presione Enter para continuar...";
                cin.get();
                break;
        }

    } while (opcion != 0);

    // cierre de conexion a la base de datos
    sqlite3_close(db);

    return 0;
}