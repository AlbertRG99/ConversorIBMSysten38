#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int codnull = 170;
int coddesc = 219;

string ruta_doc = R"(C:\Users\JOSEA\Desktop\Ejemplos\original.txt)";
string ruta_salida = R"(C:\Users\JOSEA\Desktop\Ejemplos\nuevo.txt)";

void impr_normal(vector< vector<unsigned char> >&);
void impr_ascii_dec(vector< vector<unsigned char> >&);
vector< vector<unsigned char> > leer_documento(string ruta_doc);

// Implementando actualmente
vector< vector<int> > conversion_ascii_decimal(vector< vector<unsigned char> > documento_ascii);
void paso1(vector< vector<unsigned char> >&);
bool esParrafo(vector<int>&);
bool esControl(vector<int>&);
//vector<char> aplicarEstilo(vector<char> linea, char estilo);
string getInicioEtiqueta(int);
string getCierreEtiqueta(int);
void agregarLineaDoc(string, vector<string>);


int main() {

    /*
        Leer y cargar el archivo en memoria
    */
    vector< vector<unsigned char> > documento;
    documento = leer_documento(ruta_doc);

    paso1(documento);



    //impr_normal(documento);
    //impr_ascii_dec(documento);

    return 0;
}

// Leer documento y cargar en memoria
vector<vector<unsigned char>> leer_documento(string ruta_doc) {
    // Abrir el archivo en modo binario
    ifstream archivo(ruta_doc, ios::binary);

    // Verificar si el archivo se ha abierto correctamente
    if (!archivo.is_open()) {
        cerr << "[ ERROR ] -> No se puede abrir el archivo!" << endl;
        return {};  // Retornar un vector vacío en caso de error
    }

    // Vectores todo (documento completo) y línea (auxiliar)
    vector<vector<unsigned char>> todo;
    vector<unsigned char> linea;
    unsigned char byte;

    // Leer el archivo carácter por carácter
    while (archivo.read(reinterpret_cast<char*>(&byte), sizeof(byte))) {
        if (byte == '\n') {  // Si es un salto de línea (ASCII 10)
            todo.push_back(linea); // Agregar la línea completa al vector todo
            linea.clear();          // Limpiar la línea para la siguiente
        }
        else {
            linea.push_back(byte);  // Agregar byte a la línea actual
        }
    }

    // Si la última línea no termina en '\n', debemos agregarla también
    if (!linea.empty()) {
        todo.push_back(linea);
    }

    archivo.close();
    return todo;
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

vector< vector<int> > conversion_ascii_decimal(vector< vector<unsigned char> > documento_ascii) {

    vector< vector<int> > documento_decimal;
    vector< vector<unsigned char> >::iterator it;

    for (it = documento_ascii.begin(); it != documento_ascii.end(); it++) {
        vector<int> nueva_linea;
        for (unsigned char c : *it) {
            nueva_linea.push_back(static_cast<int>(c));   // aquí se transforma a decimal
        }
        documento_decimal.push_back(nueva_linea);
    }
    return documento_decimal;
}

// NOTA.
// escribo con "archivo.put()"


// Paso 1 (en proceso)
void paso1(vector< vector<unsigned char> >& documento) {

    // La información nueva se va a ir creando en un nuevo documento aparte
    ofstream archivo(ruta_salida, ios::out);

    if (!archivo.is_open()) {
        cerr << "No se pudo crear correctamente un nuevo archivo. " << endl;
    }
    else { // ------------------------------------------------------------------------>

        // TRABAJO CON EL VECTOR "documento_decimal" en ASCII DECIMAL para los NULLs
        vector< vector<int> > documento_decimal = conversion_ascii_decimal(documento);
        vector< vector< int > >::iterator it_linea_dec;
        vector< int >::iterator it_caracter_dec;

        // TRABAJO CON EL VECTOR "documento_char" en ASCII para el resto.
        vector< vector<unsigned char> > documento_nonulls_char;
        vector< vector<unsigned char> >::iterator it_documento_char;

        // TRABAJO CON EL VECTOR NUEVO SIN NULLS
        vector< vector<int> > documento_nonulls_dec;

        // TRABAJO CON VECTOR DE INDICES DE INICIO DE PÁRRAFOS
        vector<int> ind_parr;





        // Paso 1: Eliminado de NULLs (recorro documento en decimal)


        for (it_linea_dec = documento_decimal.begin(); it_linea_dec != documento_decimal.end(); ++it_linea_dec) {
            // LINEA LEÍDA

            vector<unsigned char> linea_char;
            vector<int> linea_nonull_dec;

            // -> Lectura 1: Leo la lína y modifico NULLS


            for (it_caracter_dec = (*it_linea_dec).begin(); it_caracter_dec != (*it_linea_dec).end(); ++it_caracter_dec) {
                // CARACTER LEÍDO

                if ((*it_caracter_dec) == 0) {    // NULL (0), escribo "¬" en su lugar
                    archivo.put('¬');                       // agrego al archivo de salida

                    linea_char.push_back('¬');              // agrego a linea_char null convertido
                    linea_nonull_dec.push_back(codnull);    // agrego a linea_nonull_dec null convertido
                }
                else if ((*it_caracter_dec) >= 32 && (*it_caracter_dec) <= 255) { // Caracter visible, lo dejo como está
                    archivo.put(static_cast<unsigned char>(*it_caracter_dec));                  // agrego al archivo de salida

                    linea_char.push_back(static_cast<unsigned char>(*it_caracter_dec));         // agrego a linea_char el caracter original
                    linea_nonull_dec.push_back(*it_caracter_dec);                               // agrego a linea_nonull_dec el numero original
                }
                else if ((*it_caracter_dec) == 10) {    // Salto de línea
                    archivo.put(static_cast<unsigned char>(10));

                    linea_char.push_back(static_cast<unsigned char>(*it_caracter_dec));
                    linea_nonull_dec.push_back(*it_caracter_dec);
                }
                else if ((*it_caracter_dec) == 13) {    // Retorno de carro
                    archivo.put(static_cast<unsigned char>(13));  // nada

                    linea_char.push_back(static_cast<unsigned char>(*it_caracter_dec));
                    linea_nonull_dec.push_back(*it_caracter_dec);
                }
                else { // Caracteres NO controlados (se representan con un cuadrado relleno y no está especificado en la salida)
                    archivo.put(static_cast<unsigned char>(coddesc));  // TODO: Hacer que se represente realmente en lugar del auxiliar.

                    linea_char.push_back(static_cast<unsigned char>(*it_caracter_dec));
                    linea_nonull_dec.push_back(coddesc);
                }

            }

            documento_nonulls_char.push_back(linea_char);               // agrego la línea char nueva al documento char
            documento_nonulls_dec.push_back(linea_nonull_dec);  // agrego la línea dec nueva al documento dec

        }

        // ----------------> TESTING: IMPRESIÓN NULL CASTEADOS EN CHAR Y DECIMAL

        /*
        vector< vector<unsigned char> >::iterator it_documento_nonulls_char;
        vector<unsigned char>::iterator it_linea_char;

        vector< vector<int> >::iterator it_documento_nonulls_dec;

        for (it_documento_nonulls_char = documento_nonulls_char.begin(); it_documento_nonulls_char != documento_nonulls_char.end(); ++it_documento_nonulls_char) {
            for (it_linea_char = it_documento_nonulls_char->begin(); it_linea_char != it_documento_nonulls_char->end(); ++it_linea_char) {
                cout << static_cast<char>(*it_linea_char);
            }
            cout << endl;
        }

        vector<int>::iterator iter;
        for (it_documento_nonulls_dec = documento_nonulls_dec.begin(); it_documento_nonulls_dec != documento_nonulls_dec.end(); ++it_documento_nonulls_dec) {
            for (iter = it_documento_nonulls_dec->begin(); iter != it_documento_nonulls_dec->end(); ++iter) {
                cout << "[" << (*iter) << "]";
            }
            cout << endl;
        }

        // -------------------->

        */

        // CHECKPOINT: Cambia correctamente los NULLS (hasta aquí bien).

        // - Vector char casteado: "documento_nonulls_char"
        // - Vector decimal casteado: "documento_nonulls_dec"



        // Paso 2: Interpretación de líneas de control (recorro documento en ascii)

        /*
        for (it_documento_char = documento_char.begin(); it_documento_char != documento_char.end(); ++it_documento_char) {  // buscar

            int etiq_actual = -1;               // vacío al inicio de cada línea
            vector<string> linea_estilizada;    // Aquí se van almacenando los caracteres ya estilizados con sus etiquetas

            if (esControl(*it_documento_char)) {            // Si es una línea de CONTROL:
                auto it_anterior = prev(it_documento_char); // iterador a línea anterior

                //cout << "Esta es una linea de control" << endl;

                // TODO: [ APLICAR ESTILO A LÍNEA *it_anterior ]

                // Línea actual     = *it_documento_char
                // Línea anterior   = *it_anterior


                // ° [176] -> Inicio Subrayado <u></u>
                // µ [181] -> Inicio Resaltado <mark></mark>
                // ¢ [162] -> Inicio Subrayado y Resaltado <u><mark></mark></u>
                // { [123] -> Inicio Entrada de Texto (SE DESCARTA EN EL PROCESO) --
                // \ [92] -> Inicio Entrada de Texto y Resaltado (SE PROCESA COMO µ -> Inicio Resaltado) <mark></mark> (opcional)
                // Ÿ [159] -> Fin Inicio anterior, su posición está afectada por el formato que finaliza. </CERRAR_ANTERIOR>
                // Ø [216] -> No se hace nada. --



                for (int i = 0; i < (*it_documento_char).size(); ++i) { // recorro la línea actual de control

                    // Agrego el caracter actual de la línea anterior (datos)
                    // En caso de insertar etiqueta afectará al siguiente caracter y no al actual.
                    linea_estilizada.push_back(string(1, (*it_anterior)[i]));   // casting char a string

                    cout << "Agregado el caracter: " << (*it_anterior)[i] << endl;

                    if ((*it_documento_char)[i] == '°') {                                         // APERTURA: Inicio de Subrayado
                        etiq_actual = 0;
                        linea_estilizada.push_back(getInicioEtiqueta(etiq_actual)); // agrego a el vector estilizado
                    }
                    else if ((*it_documento_char)[i] == 'µ' || (*it_documento_char)[i] == '\\') {    // APERTURA: Inicio Resaltado / Entrada de texto resaltado
                        etiq_actual = 1;
                        linea_estilizada.push_back(getInicioEtiqueta(etiq_actual));
                    }
                    else if ((*it_documento_char)[i] == '¢') {                                      // APERTURA: Inicio Subrayado y Resaltado
                        etiq_actual = 2;
                        linea_estilizada.push_back(getInicioEtiqueta(etiq_actual));
                    }
                    else if ((*it_documento_char)[i] == 'Ÿ') {                                      // CIERRE: Cerrar la etiqueta anterior
                        linea_estilizada.push_back(getCierreEtiqueta(etiq_actual));
                    }
                    else {
                        // No hacer nada con: Ø
                    }

                }

                etiq_actual = -1;   // reseteo etiqueta

            }
            else {
                //cout << "Esta es una linea normal" << endl;
            }

            // salvo la línea estilizada (la imprimo en el documento)
            string archivo_salida = "C:\\Users\\75943540\\Desktop\\SYS38\\base\\actual\\nuevo.txt";
            agregarLineaDoc(archivo_salida, linea_estilizada);

        }
        */


        // AUXULIAR PASO 2
        // 
        // recorro el documento casteado en decimal

        vector< vector<string> > doc_estilizado;    // documento estilizado
        vector<int> parrafos;                      // números de línea donde están los párrafos

        int lin_actual = 0;

        for (auto it_documento_nonulls_dec = documento_nonulls_dec.begin(); it_documento_nonulls_dec!= documento_nonulls_dec.end(); ++it_documento_nonulls_dec) {

            vector<string> linea_estilizada;
            int etiq_actual = -1;

            if ( esControl(*it_documento_nonulls_dec) ) {       // Es de control (decimal)
                //leer control y meter ambos (anterior y control)

                
                
                auto it_linea_ant = prev(it_documento_nonulls_dec); // iterador a línea anterior

                // Linea actual:   "*it_documento_nonulls_dec"
                // Linea anterior: "*it_linea_ant"

                // ° [176] -> Inicio Subrayado <u></u>
                // µ [181] -> Inicio Resaltado <mark></mark>
                // ¢ [162] -> Inicio Subrayado y Resaltado <u><mark></mark></u>
                // { [123] -> Inicio Entrada de Texto (SE DESCARTA EN EL PROCESO) --
                // \ [92] -> Inicio Entrada de Texto y Resaltado (SE PROCESA COMO µ -> Inicio Resaltado) <mark></mark> (opcional)
                // Ÿ [159] -> Fin Inicio anterior, su posición está afectada por el formato que finaliza. </CERRAR_ANTERIOR>
                // Ø [216] -> No se hace nada. --
                
                for (int i = 0; i < (*it_documento_nonulls_dec).size(); ++i) { // recorro la línea actual de control

                    // Agrego el caracter actual de la línea anterior (datos)
                    // En caso de insertar etiqueta afectará al siguiente caracter y no al actual.
                    
                    linea_estilizada.push_back(string(1, (*it_linea_ant)[i]));   // casting char a string

                    if ((*it_documento_nonulls_dec)[i] == '°') {                                         // APERTURA: Inicio de Subrayado
                        etiq_actual = 0;
                        linea_estilizada.push_back(getInicioEtiqueta(etiq_actual)); // agrego a el vector estilizado
                    }
                    else if ((*it_documento_nonulls_dec)[i] == 'µ' || (*it_documento_nonulls_dec)[i] == '\\') {    // APERTURA: Inicio Resaltado / Entrada de texto resaltado
                        etiq_actual = 1;
                        linea_estilizada.push_back(getInicioEtiqueta(etiq_actual));
                    }
                    else if ((*it_documento_nonulls_dec)[i] == '¢') {                                      // APERTURA: Inicio Subrayado y Resaltado
                        etiq_actual = 2;
                        linea_estilizada.push_back(getInicioEtiqueta(etiq_actual));
                    }
                    else if ((*it_documento_nonulls_dec)[i] == 'Ÿ') {                                      // CIERRE: Cerrar la etiqueta anterior
                        linea_estilizada.push_back(getCierreEtiqueta(etiq_actual));
                    }
                    else {
                        // No hacer nada con: Ø
                    }

                }


            }
            else if ( esParrafo(*it_documento_nonulls_dec) ) {  // Es de párrafo (decimal)
                //apunto el numero de línea para saber dónde están los párrafos
                parrafos.push_back(lin_actual);


            }
            else {                                              // Es otro tipo cualquiera
                // simplemente agregar la línea al documento estilizado
                for (auto it = it_documento_nonulls_dec->begin(); it != it_documento_nonulls_dec->end(); ++it) {
                    linea_estilizada.push_back(string(1, *it));
                }
                doc_estilizado.push_back(linea_estilizada);
            }

            lin_actual++;

        }

        // Paso 3: Interpretación de párrafos y estilado de los mismos


        /*

        // Paso 3: Interpretación de párrafos (recorro documento en ascii)

        //int contador_ind = 0;

        //for (it_documento_char = documento_char.begin(); it_documento_char != documento_char.end(); ++it_documento_char) {  // buscar

        //    if (esParrafo(*it_documento_char)) {
        //        ind_parr.push_back(contador_ind);   // almacena el número de línea de párrafos
        //    }
        //    contador_ind++;
        //}

        // TODO: [ APLICAR ESTILO A CADA PÁRRAFO DESDE ind_parr[n] HASTA ind_parr[n+1]-1 ]

        */



        archivo.close();

        cout << "Archivo creado correctamente" << endl;

        // --------------------------------------------------------------------------->
    }
}

/*
vector<char> aplicarEstilo(vector<char> linea, char estilo) {



}
*/

bool esParrafo(vector<int>& linea) {   // Comprueba si se trata de una linea de inicio de párrafo

    if (linea[0] == codnull && linea[1] >= 97 && linea[1] <= 122) {    // NULL + [a,..,z] = Párrafo
        return true;
    }
    else {
        return false;
    }

}

bool esControl(vector<int>& linea) {

    // Lo comparo en decimal

    if ((linea[0] == codnull && linea[1] == codnull) ||     // NULL ó (más) al principio
        (linea[0] == codnull) &&                            // NULL y (carácter control) al principio
            (
                linea[1] == 176 || // '°'
                linea[1] == 181 || // 'µ'
                linea[1] == 123 || // '{'
                linea[1] == 159 || // 'Ÿ'
                linea[1] == 92     // '\'          // representa '\' en C++
                )
            ) {
        return true;
    }
    else {
        return false;
    }

}

string getCierreEtiqueta(int etiqueta) {

    if (etiqueta == 0) {        // '°' (subrayado)
        return "</u>";
    }
    else if (etiqueta == 1) {   // 'µ' ó '\' (resaltado)
        return "</mark>";
    }
    else if (etiqueta == 2) {   // '¢' (subrayado y resaltado)
        return "</u></mark>";
    }
    else {
        // No hacer nada
    }

}

string getInicioEtiqueta(int etiqueta) {
    if (etiqueta == 0) {        // '°' (subrayado)
        return "<u>";
    }
    else if (etiqueta == 1) {   // 'µ' ó '\' (resaltado)
        return "<mark>";
    }
    else if (etiqueta == 2) {   // '¢' (subrayado y resaltado)
        return "<u><mark>";
    }
    else {
        // No hacer nada
    }
}

void agregarLineaDoc(string nombreArchivo, vector<string> linea_estilizada) {
    ofstream archivo(nombreArchivo, ios::out | ios::app);

    if (!archivo.is_open()) {
        cerr << "No se pudo abrir el archivo " << nombreArchivo << endl;
        return;  // Si no se puede abrir el archivo, salir de la función
    }

    vector<string>::iterator it;

    for (auto& parte : linea_estilizada) {
        cout << parte;  // Imprimir cada parte de la línea estilizada
    }

    // Cerrar el archivo
    archivo.close();
}