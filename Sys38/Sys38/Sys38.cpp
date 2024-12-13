#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void impr_normal(vector< vector<unsigned char> >&);
void impr_ascii_dec(vector< vector<unsigned char> >&);

int main() {
    // Ruta del archivo
    string documento = R"(C:\Users\75943540\Desktop\SYS38\auxiliar\original.ANSI.txt)";

    // Abrir el archivo en modo binario
    ifstream archivo(documento, ios::binary);

    // Verificar si el archivo se ha abierto correctamente
    if (!archivo.is_open()) {
        cerr << "[ ERROR ] -> No se puede abrir el archivo!" << endl;
        return 1;
    }

    // Variable auxiliar para lectura de byte
    unsigned char byte;

    // Vectores todo (documento completo) y línea (auxiliar)
    vector< vector<unsigned char> > todo;
    vector<unsigned char> linea;

    // Leer el archivo carácter por carácter
    while (archivo.get(reinterpret_cast<char&>(byte))) {

        /*
        // Mostrar el carácter y su código ANSI
        if (byte == '\r') {  // Retorno de carro
            cout << "Caracter: '\\r' - Codigo ANSI: 13" << endl;
        }
        else if (byte == '\n') {  // Salto de línea
            cout << "Caracter: '\\n' - Codigo ANSI: 10" << endl;
        }
        else {
            cout << "Caracter: '" << byte << "' - Codigo ANSI: " << static_cast<int>(byte) << endl;
        }
        */


        if (byte == '\n') { // si es un retorno de carro [10] (lo que no lee con getline)
            linea.push_back(byte);
            todo.push_back(linea);  // agrego al vector total
            linea.clear();          // borro su contenido para que no ocupe memoria (importante!)
        }
        else {              // si es otro tipo de caracter normal (lectura normal con getline)
            linea.push_back(byte);
        }

    }

    // ---------------- TESTING -------------------
    //impr_normal(todo);
    impr_ascii_dec(todo);



    archivo.close();
    return 0;
}

// Imprimir el documento normalmente
void impr_normal(vector< vector<unsigned char> >& todo) {


    vector< vector<unsigned char> >::iterator it_linea;
    for (it_linea = todo.begin(); it_linea != todo.end(); it_linea++) {
        // leo una línea (*it_linea)

        for (unsigned char c : *it_linea) {
            cout << c;
        }
        cout << endl;
    }

}

// Imprimir el documento en ASCII (decimal)
void impr_ascii_dec(vector< vector<unsigned char> >& todo) {

    vector< vector<unsigned char> >::iterator it_linea;
    for (it_linea = todo.begin(); it_linea != todo.end(); it_linea++) {
        // leo una línea (*it_linea)

        for (unsigned char c : *it_linea) {
            cout << "[" << static_cast<int>(c) << "]";    // aquí se transforma a decimal ascii
        }
        cout << endl;
    }

}
