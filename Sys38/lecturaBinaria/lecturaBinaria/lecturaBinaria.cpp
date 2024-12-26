#include<iostream>
#include<fstream>
#include<vector> 
#include<string> 
#include<sstream> 
#include<map>
#include<variant>
#include<bitset> 

using namespace std;

/*
    POR HACER:

    - Procesar la incrustación de documentos            []
    - Automatizar conversión a DOCX                     [x]
    - Automatizar conversion a PDF                      [x]
    - Aplicar y eliminar las instrucciones del final    []

*/


// Ruta PC casa (windows)
string ruta_doc_orig = R"(C:\Users\JOSEA\Documents\GitHub\ConversorIBMSysten38\Sys38\lecturaBinaria\lecturaBinaria\PL0200.txt)";

// Ruta PC casa (ubuntu)
//string ruta_doc_orig = R"(/media/albert/1TB_BARRACUDA/COMPARTIDO/ProyectoSystem400/Ejemplos/primario/original.txt)";

// Ruta PC ayuntamiento
//string ruta_doc_orig = R"(C:\Users\75943540\Desktop\PL0200.txt)";


vector< vector< bitset<8> > > leerASCIIBinario(string&);
vector< vector<int> > convBinarioDecimal(vector< vector< bitset<8> > >&);
vector<vector<int>> convBinarioDecimal(vector<vector<bitset<8>>>&);
vector<vector<unsigned char>> leerASCII(string&);
vector<vector<variant<int, unsigned char>>> crearDocMixto(vector<vector<unsigned char>>&, vector<vector<int>>&);
vector<vector<variant<int, unsigned char, string>>> procesarTextoMixto(vector<vector<variant<int, unsigned char>>>&);
bool esParrafo(vector<variant<int, unsigned char>>& linea);
bool esControl(vector<variant<int, unsigned char>>& linea);
bool esControl(vector<int>&);
string getIniMarca(int especial);
string getFinMarca(int especial);
void volcarEnArchivo(const string& nombreArchivo, const vector<vector<variant<int, unsigned char, string>>>& estiloDocMix);
vector<vector<variant<int, unsigned char, string>>> procesarSaltosLinea(const vector<vector<variant<int, unsigned char, string>>>& estiloDocMix);
vector<vector<variant<int, unsigned char, string>>> procesarEspaciosDuros(const vector<vector<variant<int, unsigned char, string>>>& estiloDocMix);
void eliminarIniciosParrafo(vector<vector<variant<int, unsigned char, string>>>&);
string insertarPieHTML();
string insertarCabeceraHTML();
bool esIncrutacion(vector<variant<int, unsigned char, string>>&);
bool esIncrutacion(vector<variant<int, unsigned char>>&);
void insertarIncrustacion(vector<vector<variant<int, unsigned char, string>>>&, vector<vector<variant<int, unsigned char, string>>>&, int linea_inc);
vector<pair<int, string>> getLineasInc(vector<vector<variant<int, unsigned char, string>>>&);
string getNombreInc(vector<variant<int, unsigned char>>&);
string getNombreInc(vector<variant<int, unsigned char, string>>&);
vector<vector<variant<int, unsigned char, string>>> procesarIncrustaciones(vector<vector<variant<int, unsigned char, string>>>&);
vector<vector<variant<int, unsigned char, string>>> importarIncrustacion(string);
void eliminarInstruccionesFinales(const string& );

void imprimirBinario(vector< vector< bitset<8> > >& doc_binario);
void imprimirDocumentoDecimal(const vector<vector<int>>& docDec);
void imprimirDocumentoASCII(const vector<vector<unsigned char>>& docASCII);
void imprimirDocumentoMixto(const vector<vector<variant<int, unsigned char>>>& docMix);




int main() {

    // --> PASO 1: LECTURA DEL DOCUMENTO

    // Leer el archivo y almacenarlo en ASCII
    vector<vector<unsigned char>> docASCII = leerASCII(ruta_doc_orig);

    // Leer el archivo y obtener el binario
    vector<vector<bitset<8>>> docBin = leerASCIIBinario(ruta_doc_orig);

    // Pasar de binario a decimal
    vector<vector<int>> docDec = convBinarioDecimal(docBin);

    // Crear el archivo mixto (decimal + char)
    vector<vector<variant<int, unsigned char>>> docMix = crearDocMixto(docASCII, docDec);



    // --> PASO 2: DETECCIÓN DE LÍNEAS Y APLICACIÓN DE ESTILOS

    // Procesar el texto mixto y obtener el documento con estilos aplicados
    vector<vector<variant<int, unsigned char, string>>> estiloDocMix = procesarTextoMixto(docMix);

    // Volcar el documento procesado en un archivo HTML
    volcarEnArchivo("nuevo.html", estiloDocMix);

    // Eliminar instrucciones finales
	eliminarInstruccionesFinales("nuevo.html");

    return 0;
}

// Elimina las instrucciones finales del archivo HTML

void eliminarInstruccionesFinales(const string& nombreArchivo) {
    ifstream archivoEntrada(nombreArchivo);
    if (!archivoEntrada) {
        cerr << "Error al abrir el archivo " << nombreArchivo << " para lectura." << endl;
        return;
    }

    // Leer todas las líneas del archivo y almacenarlas en un vector
    vector<string> lineas;
    string linea;
    while (getline(archivoEntrada, linea)) {
        lineas.push_back(linea);
    }
    archivoEntrada.close();

    // Eliminar las líneas que comienzan con las secuencias especificadas
    vector<string> secuencias = { ".im ", ".PF", ".PR", ".CH" };
    auto it = lineas.begin();
    while (it != lineas.end()) {
        string lineaSinEspacios = *it;
        // Eliminar espacios en blanco al inicio
        lineaSinEspacios.erase(0, lineaSinEspacios.find_first_not_of(" \t"));

        bool eliminar = false;
        for (const auto& secuencia : secuencias) {
            if (lineaSinEspacios.find(secuencia) == 0) {
                eliminar = true;
                break;
            }
        }

        if (eliminar) {
            it = lineas.erase(it);
        }
        else {
            ++it;
        }
    }

    // Escribir las líneas modificadas de nuevo al archivo
    ofstream archivoSalida(nombreArchivo);
    if (!archivoSalida) {
        cerr << "Error al abrir el archivo " << nombreArchivo << " para escritura." << endl;
        return;
    }

    for (const auto& linea : lineas) {
        archivoSalida << linea << '\n';
    }

    archivoSalida.close();

    cout << "Las instrucciones finales que comienzan con las secuencias especificadas han sido eliminadas del archivo " << nombreArchivo << "." << endl;
}


// Lee el fichero en ANSII y lo almacena en un vector en modo binario

vector< vector< bitset<8> > > leerASCIIBinario(string& ruta_doc) {
    ifstream file(ruta_doc, ios::binary);

    if (!file) {
        cerr << "No se pudo abrir el archivo para lectura binaria: " << ruta_doc << endl;
        return {};
    }

    // Determinar el tamaño del archivo de entrada
    file.seekg(0, ios::end);
    streamsize fileSize = file.tellg();
    file.seekg(0, ios::beg);

    // Búfer para almacenar los datos leídos
    vector<char> buffer(fileSize);

    // Leer los datos del archivo en el búfer
    if (!file.read(buffer.data(), fileSize)) {
        cerr << "Error al leer el archivo: " << ruta_doc << endl;
        return {};
    }

    vector<vector<bitset<8>>> doc_binario;  // Vector que almacenará las líneas
    vector<bitset<8>> linea_binaria;  // Línea actual

    // Procesar el contenido del archivo
    for (size_t i = 0; i < buffer.size(); i++) {

        bitset<8> byte_binario(static_cast<unsigned char>(buffer[i]));

        // Si el byte es 10 en decimal (salto de línea en ASCII), agregamos la línea
        if (byte_binario == bitset<8>(10)) {
            linea_binaria.push_back(byte_binario);  // añadir el salto de línea a la línea
            doc_binario.push_back(linea_binaria);   // almacenar la línea en el documento binario
            linea_binaria.clear();                  // limpiar la línea para comenzar una nueva
        }
        else {
            linea_binaria.push_back(byte_binario);  // agregar el byte normalmente
        }
    }

    // Si la última línea no está vacía, agregarla al documento binario
    if (!linea_binaria.empty()) {
        doc_binario.push_back(linea_binaria);
    }

    file.close();

    return doc_binario;
}

// Convierte un vector binario y lo almacena en un vector en modo decimal entero

vector<vector<int>> convBinarioDecimal(vector< vector< bitset<8> > >& docBin) {

    vector<vector<bitset<8>>>::iterator it_doc;
    vector<bitset<8>>::iterator it_linea;

    vector<vector<int>> docDec;
    for (it_doc = docBin.begin(); it_doc != docBin.end(); ++it_doc) {
        vector<int> lineaDec;
        for (it_linea = it_doc->begin(); it_linea != it_doc->end(); ++it_linea) {
            lineaDec.push_back(static_cast<int>(static_cast<unsigned char>(bitset<8>(*it_linea).to_ulong())));
        }
        docDec.push_back(lineaDec);
    }

    return docDec;
}

// Pasa un documento ANSII a un vector

vector<vector<unsigned char>> leerASCII(string& ruta_doc) {
    // Abrir el archivo en modo binario (se lee byte a byte)
    ifstream file(ruta_doc, ios::binary);

    if (!file) {
        cerr << "No se pudo abrir el archivo: " << ruta_doc << endl;
        return {};  // Devuelve un vector vacío si no se puede abrir el archivo
    }

    // Determinar el tamaño del archivo
    file.seekg(0, ios::end);
    streamsize fileSize = file.tellg();
    file.seekg(0, ios::beg);

    // Búfer para almacenar los datos leídos
    vector<unsigned char> buffer(fileSize);

    // Leer los datos del archivo en el búfer
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        cerr << "Error al leer el archivo: " << ruta_doc << endl;
        return {};  // Devuelve un vector vacío si hay un error de lectura
    }

    vector<vector<unsigned char>> docASCII;  // Vector para almacenar el documento como vectores de unsigned char
    vector<unsigned char> linea;  // Línea actual

    // Procesar el contenido del archivo
    for (size_t i = 0; i < buffer.size(); i++) {
        if (buffer[i] == 10) {  // Si encontramos un salto de línea (10 en ASCII)
            linea.push_back(buffer[i]);  // Añadimos el salto de línea a la línea actual
            docASCII.push_back(linea);  // Añadimos la línea al documento
            linea.clear();  // Limpiar la línea para empezar una nueva
        }
        else {
            linea.push_back(buffer[i]);  // Agregar el carácter a la línea
        }
    }

    // Si la última línea no está vacía, agregarla al documento
    if (!linea.empty()) {
        docASCII.push_back(linea);
    }

    file.close();
    return docASCII;
}

// Crea un vector documento mixto

vector<vector<variant<int, unsigned char>>> crearDocMixto(vector<vector<unsigned char>>& docASCII, vector<vector<int>>& docDec) {

    // Verificamos que ambos vectores tienen el mismo tamaño
    if (docASCII.size() == docDec.size()) {

        vector<vector<variant<int, unsigned char>>> docMixto;

        for (int linea = 0; linea < docASCII.size(); linea++) {

            vector<variant<int, unsigned char>> lineaMixta;

            // si es linea de control, mantener
            // si es linea normal, dejar los codigos en char



            if (esControl(docDec[linea])) { // si es una línea de control

                for (int caracter = 0; caracter < docASCII[linea].size(); caracter++) {

                    if (((docDec[linea][caracter] >= 32 && docDec[linea][caracter] <= 127) &&    // caracteres comunes
                        docDec[linea][caracter] != 92)     // caracter de control '\'
                        ||
                        (docDec[linea][caracter] >= 192 && docDec[linea][caracter] <= 255)  // letras con tilde
                        ||
                        docDec[linea][caracter] == 170  // 'ª'

                        ) { // Si está en el rango ASCII común
                        lineaMixta.push_back(docASCII[linea][caracter]); // Lo mantenemos como unsigned char
                    }
                    else {
                        lineaMixta.push_back(docDec[linea][caracter]); // Lo cambiamos a decimal
                    }
                }
            }
            else {  // si es una línea normal

                for (int caracter = 0; caracter < docASCII[linea].size(); caracter++) {

                    if (((docDec[linea][caracter] >= 32 && docDec[linea][caracter] <= 127) &&    // caracteres comunes
                        docDec[linea][caracter] != 92)     // caracter de control '\'
                        ||
                        (docDec[linea][caracter] >= 192 && docDec[linea][caracter] <= 255)  // letras con tilde
                        ||
                        (docDec[linea][caracter] == 170)  // 'ª'
                        ||
                        // CARACTERES QUE SON IGUALES A LOS DE CONTROL PERO QUE SE DEBEN MANTENER COMO CHAR

                        (docDec[linea][caracter] == 186)  // 'ª'

                        ) { // Si está en el rango ASCII común
                        lineaMixta.push_back(docASCII[linea][caracter]); // Lo mantenemos como unsigned char
                    }
                    else {
                        lineaMixta.push_back(docDec[linea][caracter]); // Lo cambiamos a decimal
                    }
                }
            }

            // Añadimos la línea mixta al documento mixto
            docMixto.push_back(lineaMixta);
        }

        return docMixto;

    }
    else {
        cout << "Error: Los vectores ASCII y decimal no tienen el mismo tamaño" << endl;
    }

    // Si no hay coincidencia de tamaños, devolvemos un vector vacío
    return {};
}

// Detección de tipos de líneas (párrafos/control/normal) y aplicación de estilos

/*
        ° [176] -> Inicio Subrayado <u></u>
        µ [181] -> Inicio Resaltado <mark></mark>
        ¢ [162] -> Inicio Subrayado y Resaltado <u><mark></u></mark>
        { [123] -> Inicio Entrada de Texto (SE DESCARTA EN EL PROCESO)
        \ [92] -> Inicio Entrada de Texto y Resaltado (SE PROCESA COMO µ -> Inicio Resaltado) <mark></mark>
        Ÿ [159] -> Fin Inicio anterior, su posición está afectada por el formato que finaliza.
        Ø [216] -> No se hace nada.
*/

vector<vector<variant<int, unsigned char, string>>> procesarTextoMixto(vector<vector<variant<int, unsigned char>>>& docMix) {
    
    /*
    // Paso 1: Nuevo almacén con soporte para etiquetas HTML
    vector<vector<variant<int, unsigned char, string>>> estiloDocMix;

    for (auto it_linea = docMix.begin(); it_linea != docMix.end(); ++it_linea) {
        if (esControl(*it_linea)) { // Si es una línea de control
            // Obtengo la última línea del vector modificado
            vector<variant<int, unsigned char, string>> linea_superior = estiloDocMix.back();
            estiloDocMix.pop_back();

            // La modifico agregándole las líneas HTML
            vector<variant<int, unsigned char, string>> linea_estilada;
            vector<variant<int, unsigned char>> linea_actual_control = *it_linea; // Línea actual

            int ultima_etiqueta = -1;
            int max_size = max(linea_superior.size(), linea_actual_control.size()); // Longitud máxima

            for (int indice = 0; indice < max_size; ++indice) {
                // Procesar elementos de la línea superior
                auto procesarElemento = [&](const auto& elemento) {
                    if (holds_alternative<int>(elemento)) {
                        linea_estilada.push_back(get<int>(elemento));
                    }
                    else if (holds_alternative<unsigned char>(elemento)) {
                        linea_estilada.push_back(get<unsigned char>(elemento));
                    }
                    else if (holds_alternative<string>(elemento)) {
                        linea_estilada.push_back(get<string>(elemento));
                    }
                    };

                if (indice < linea_actual_control.size()) { // Si hay un elemento en la línea inferior
                    const auto& caracter = linea_actual_control[indice];

                    if (holds_alternative<int>(caracter)) { // Si es un entero de control
                        int codigo_control = get<int>(caracter);
                        if (codigo_control == 176) { // °
                            ultima_etiqueta = 0;
                            if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                            linea_estilada.push_back(getIniMarca(ultima_etiqueta));
                        }
                        else if (codigo_control == 181) { // µ o '\'
                            ultima_etiqueta = 1;
                            if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                            linea_estilada.push_back(getIniMarca(ultima_etiqueta));
                        }
                        else if (codigo_control == 162 || codigo_control == 92) { // ¢
                            ultima_etiqueta = 2;
                            if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                            linea_estilada.push_back(getIniMarca(ultima_etiqueta));
                        }
                        else if (codigo_control == 159) { // Ÿ (cierre)
                            if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                            linea_estilada.push_back(getFinMarca(ultima_etiqueta));
                            ultima_etiqueta = -1; // Cierra etiqueta
                        }
                        else { // Otros códigos
                            if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                        }
                    }
                    else { // No es un entero, agregar directamente
                        if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                    }
                }
                else if (indice < linea_superior.size()) { // Si la línea inferior terminó, agregar superior
                    procesarElemento(linea_superior[indice]);
                }
            }

            // Cerrar etiquetas pendientes al final
            if (ultima_etiqueta != -1) {
                linea_estilada.push_back(getFinMarca(ultima_etiqueta));
            }

            // Agregar la línea estilada al documento
            estiloDocMix.push_back(linea_estilada);
        }
        else { // Si no es una línea de control (normal)

            // Crear una nueva línea en el formato adecuado para estiloDocMix
            vector<variant<int, unsigned char, string>> nueva_linea;
            for (const auto& caracter : *it_linea) {
                if (holds_alternative<int>(caracter)) {
                    nueva_linea.push_back(get<int>(caracter));
                }
                else if (holds_alternative<unsigned char>(caracter)) {
                    nueva_linea.push_back(get<unsigned char>(caracter));
                }
            }
            estiloDocMix.push_back(nueva_linea);

        }
    }



    // Procesar las incrustaciones
    procesarIncrustaciones(estiloDocMix);

    // Eliminar los inicios de párrafo (a,b,c,...,z) [ NO EJECUTAR ÉSTO SI QUIERES IDENTIFICAR LOS PÁRRAFOS POSTERIORMENTE!! ]
    eliminarIniciosParrafo(estiloDocMix);

    // Eliminar retornos de carro finales y los valores nulos
    estiloDocMix = procesarSaltosLinea(estiloDocMix);

    // Poner espacios duros HTML
    estiloDocMix = procesarEspaciosDuros(estiloDocMix);


    // Volcar en archivo
    volcarEnArchivo("nuevo.html", estiloDocMix);

	// Retornar el documento estilado
	return estiloDocMix;

    */

    // Paso 1: Nuevo almacén con soporte para etiquetas HTML
    vector<vector<variant<int, unsigned char, string>>> estiloDocMix;

    for (auto it_linea = docMix.begin(); it_linea != docMix.end(); ++it_linea) {
        if (esControl(*it_linea)) { // Si es una línea de control
            // Obtengo la última línea del vector modificado
            vector<variant<int, unsigned char, string>> linea_superior = estiloDocMix.back();
            estiloDocMix.pop_back();

            // La modifico agregándole las líneas HTML
            vector<variant<int, unsigned char, string>> linea_estilada;
            vector<variant<int, unsigned char>> linea_actual_control = *it_linea; // Línea actual

            int ultima_etiqueta = -1;
            int max_size = max(linea_superior.size(), linea_actual_control.size()); // Longitud máxima

            for (int indice = 0; indice < max_size; ++indice) {
                // Procesar elementos de la línea superior
                auto procesarElemento = [&](const auto& elemento) {
                    if (holds_alternative<int>(elemento)) {
                        linea_estilada.push_back(get<int>(elemento));
                    }
                    else if (holds_alternative<unsigned char>(elemento)) {
                        linea_estilada.push_back(get<unsigned char>(elemento));
                    }
                    else if (holds_alternative<string>(elemento)) {
                        linea_estilada.push_back(get<string>(elemento));
                    }
                    };

                if (indice < linea_actual_control.size()) { // Si hay un elemento en la línea inferior
                    const auto& caracter = linea_actual_control[indice];

                    if (holds_alternative<int>(caracter)) { // Si es un entero de control
                        int codigo_control = get<int>(caracter);
                        if (codigo_control == 176) { // °
                            ultima_etiqueta = 0;
                            if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                            linea_estilada.push_back(getIniMarca(ultima_etiqueta));
                        }
                        else if (codigo_control == 181) { // µ o '\'
                            ultima_etiqueta = 1;
                            if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                            linea_estilada.push_back(getIniMarca(ultima_etiqueta));
                        }
                        else if (codigo_control == 162 || codigo_control == 92) { // ¢
                            ultima_etiqueta = 2;
                            if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                            linea_estilada.push_back(getIniMarca(ultima_etiqueta));
                        }
                        else if (codigo_control == 159) { // Ÿ (cierre)
                            if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                            linea_estilada.push_back(getFinMarca(ultima_etiqueta));
                            ultima_etiqueta = -1; // Cierra etiqueta
                        }
                        else { // Otros códigos
                            if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                        }
                    }
                    else { // No es un entero, agregar directamente
                        if (indice < linea_superior.size()) procesarElemento(linea_superior[indice]);
                    }
                }
                else if (indice < linea_superior.size()) { // Si la línea inferior terminó, agregar superior
                    procesarElemento(linea_superior[indice]);
                }
            }

            // Cerrar etiquetas pendientes al final
            if (ultima_etiqueta != -1) {
                linea_estilada.push_back(getFinMarca(ultima_etiqueta));
            }

            // Agregar la línea estilada al documento
            estiloDocMix.push_back(linea_estilada);
        }
        else { // Si no es una línea de control (normal)
            // Crear una nueva línea en el formato adecuado para estiloDocMix
            vector<variant<int, unsigned char, string>> nueva_linea;
            for (const auto& caracter : *it_linea) {
                if (holds_alternative<int>(caracter)) {
                    nueva_linea.push_back(get<int>(caracter));
                }
                else if (holds_alternative<unsigned char>(caracter)) {
                    nueva_linea.push_back(get<unsigned char>(caracter));
                }
            }
            estiloDocMix.push_back(nueva_linea);
        }
    }

    // Procesar las incrustaciones y capturar el resultado
    estiloDocMix = procesarIncrustaciones(estiloDocMix);

    // Eliminar los inicios de párrafo (a,b,c,...,z)
    eliminarIniciosParrafo(estiloDocMix);

    // Eliminar retornos de carro finales y los valores nulos
    estiloDocMix = procesarSaltosLinea(estiloDocMix);

    // Poner espacios duros HTML
    estiloDocMix = procesarEspaciosDuros(estiloDocMix);

    // Volcar en archivo
    volcarEnArchivo("nuevo.html", estiloDocMix);

    // Retornar el documento estilado
    return estiloDocMix;


}


// Procesa las incristaciones del documento

vector<vector<variant<int, unsigned char, string>>> procesarIncrustaciones(vector<vector<variant<int, unsigned char, string>>>& documento) {
    
    /*
    vector<pair<int, string>> lineas_incrustacion = getLineasInc(documento);

    if (lineas_incrustacion.size() != 0) {

        cout << "Se han detectado " << lineas_incrustacion.size() << " incrustaciones para el documento actual" << endl;

        int lineas_insertadas = 0;  // llevo el conteo de las líneas insertadas en cada incrustación para el offset de las siguientes

        // Para cada incrustación detectada ...

        for (size_t i = 0; i < lineas_incrustacion.size(); ++i) {
            const auto& linea_inc = lineas_incrustacion[i];

            // Ajustar la línea de inserción en base a las líneas previamente insertadas
            int linea_incrustacion_correcta = linea_inc.first + lineas_insertadas;

            // Obtener el documento a incrustación (recursiva)
            vector<vector<variant<int, unsigned char, string>>> incrustacion = importarIncrustacion(getNombreInc(documento[linea_incrustacion_correcta]));

            // Insertar la incrustación en el documento original
            insertarIncrustacion(documento, incrustacion, linea_incrustacion_correcta);

            // Aplicar el offset de las líneas insertadas
            lineas_insertadas += incrustacion.size();
        }
    }
    else {
        cout << "No se han detectado incrustaciones para este archivo" << endl;
    }
    */

    vector<pair<int, string>> lineas_incrustacion = getLineasInc(documento);

    if (!lineas_incrustacion.empty()) {
        cout << "Se han detectado " << lineas_incrustacion.size() << " incrustaciones para el documento actual" << endl;

        int lineas_insertadas = 0;  // Contador de líneas insertadas

        for (const auto& linea_inc : lineas_incrustacion) {
            // Ajustar la línea de inserción
            int linea_incrustacion_correcta = linea_inc.first + lineas_insertadas;

            // Obtener el documento a incrustar (procesado)
            vector<vector<variant<int, unsigned char, string>>> incrustacion = importarIncrustacion(linea_inc.second);

            // Insertar la incrustación en el documento original
            documento.insert(documento.begin() + linea_incrustacion_correcta, incrustacion.begin(), incrustacion.end());

            // Actualizar el contador de líneas insertadas
            lineas_insertadas += incrustacion.size();
        }
    }
    else {
        cout << "No se han detectado incrustaciones para este archivo" << endl;
    }

    return documento;

}


// Importar la incrustación dado el nombre de un documento

vector<vector<variant<int, unsigned char, string>>> importarIncrustacion(string nombre_archivo) {

    /*
    vector<vector<variant<int, unsigned char, string>>> nuevo_documento;

    //cout << "Se intenta hacer la incrustacíón de: " << nombre_archivo << endl;

    // TODO: ------ POR HACER !! ------------ >>>

    // TODO: PROVISIONALMENTE, SE BUSCA SOLO EN LA PRIMERA PARTE DEL NOMBRE DEL DOCUMENTO (Ej: "pl202 plenos98.Secretaria")

    // Me que do con lo que hay antes del primer espacio (nombre en sí del archivo) [ÉSTO DEBERÍA VENIR YA RECORTADO DESDE ARRIBA...]
    size_t pos = nombre_archivo.find(' ');
    if (pos != string::npos) {
        nombre_archivo = nombre_archivo.substr(0, pos);
    }
    nombre_archivo += ".txt";
    cout << endl << "Nombre del archivo a incrustar: " << nombre_archivo << endl << endl;

    // Me traigo todo lo que hay y lo inserto en nuevo_documento






    // Leer el archivo incrustado y almacenarlo en ASCII
    vector<vector<unsigned char>> INCdocASCII = leerASCII(nombre_archivo);

    // Leer el archivo incrustado y obtener el binario
    vector<vector<bitset<8>>> INCdocBin = leerASCIIBinario(nombre_archivo);

    // Pasar de binario a decimal
    vector<vector<int>> INCdocDec = convBinarioDecimal(INCdocBin);

    // Crear el archivo en mixto (decimal + char)
    vector<vector<variant<int, unsigned char>>> docMix = crearDocMixto(INCdocASCII, INCdocDec);

    // Procesar el mixto y aplicar estilos
    procesarTextoMixto(docMix);


    // Convertir el vector mixto a un vector de variantes de int, unsigned char y string como nuevo_documento

    for (auto it_linea = docMix.begin(); it_linea != docMix.end(); ++it_linea) {
        
        vector<variant<int, unsigned char, string>> nueva_linea;
        
        for (auto it_caracter = it_linea->begin(); it_caracter != it_linea->end(); ++it_caracter) {
            if (holds_alternative<int>(*it_caracter)) {
                nueva_linea.push_back(get<int>(*it_caracter));
            }
            else if (holds_alternative<unsigned char>(*it_caracter)) {
                nueva_linea.push_back(get<unsigned char>(*it_caracter));
            }
        }
        nuevo_documento.push_back(nueva_linea);
    }

    // -------------------------------------- >>>
    return nuevo_documento;
    */


    vector<vector<variant<int, unsigned char, string>>> nuevo_documento;

    // TODO: PROVISIONALMENTE, SE BUSCA SOLO EN LA PRIMERA PARTE DEL NOMBRE DEL DOCUMENTO (Ej: "pl202 plenos98.Secretaria")
    // Me quedo con lo que hay antes del primer espacio (nombre en sí del archivo)
    size_t pos = nombre_archivo.find(' ');
    if (pos != string::npos) {
        nombre_archivo = nombre_archivo.substr(0, pos);
    }
    nombre_archivo += ".txt";
    cout << endl << "Nombre del archivo a incrustar: " << nombre_archivo << endl << endl;

    // Leer el archivo incrustado y almacenarlo en ASCII
    vector<vector<unsigned char>> INCdocASCII = leerASCII(nombre_archivo);

    // Leer el archivo incrustado y obtener el binario
    vector<vector<bitset<8>>> INCdocBin = leerASCIIBinario(nombre_archivo);

    // Pasar de binario a decimal
    vector<vector<int>> INCdocDec = convBinarioDecimal(INCdocBin);

    // Crear el archivo en mixto (decimal + char)
    vector<vector<variant<int, unsigned char>>> docMix = crearDocMixto(INCdocASCII, INCdocDec);

    // Procesar el mixto y aplicar estilos, capturando el resultado
    vector<vector<variant<int, unsigned char, string>>> estiloDocMix = procesarTextoMixto(docMix);

    // Utilizar el documento procesado como 'nuevo_documento'
    nuevo_documento = estiloDocMix;

    return nuevo_documento;


}


// Obtener líneas de incrustación
vector<pair<int, string>> getLineasInc(vector<vector<variant<int, unsigned char, string>>>& documento) {

    vector< pair<int, string> > salida;

    for (vector<vector<variant<int, unsigned char, string>>>::iterator it = documento.begin(); it != documento.end(); it++) {

        if (esIncrutacion(*it)) {

            pair<int, string> doc_inc;

            // almaceno la línea donde está
            doc_inc.first = distance(documento.begin(), it);

            // almaceno el nombre del documento al que apunta
            doc_inc.second = getNombreInc(*it);

            // agrego al vector de salida
            salida.push_back(doc_inc);
        }

    }
    return salida;
}

// Obtener el nombre del documento a incrustar en una línea

string getNombreInc(vector<variant<int, unsigned char>>& linea) {
    // Iterador para la posición inicial del punto
    auto pos_ini = linea.end();  // Inicializado al final del vector como valor inválido

    // Buscar el primer punto ('.')
    for (auto caracter = linea.begin(); caracter != linea.end(); ++caracter) {
        if (holds_alternative<unsigned char>(*caracter) && get<unsigned char>(*caracter) == '.') {
            pos_ini = caracter; // Guardar posición del punto
            break;
        }
    }

    // Verificar si encontramos el punto
    if (pos_ini == linea.end()) {
        return ""; // No se encontró el punto, retornar vacío
    }

    // Validar la secuencia ".im "
    if (distance(pos_ini, linea.end()) > 4 && // Asegurarse de que hay espacio suficiente
        holds_alternative<unsigned char>(*(pos_ini + 1)) && get<unsigned char>(*(pos_ini + 1)) == ' ' &&
        holds_alternative<unsigned char>(*(pos_ini + 2)) && get<unsigned char>(*(pos_ini + 2)) == 'i' &&
        holds_alternative<unsigned char>(*(pos_ini + 3)) && get<unsigned char>(*(pos_ini + 3)) == 'm' &&
        holds_alternative<unsigned char>(*(pos_ini + 4)) && get<unsigned char>(*(pos_ini + 4)) == ' ') {

        // Extraer el nombre entre paréntesis
        string nombre;
        bool leyendo = false;
        for (auto it = pos_ini + 5; it != linea.end(); ++it) {
            if (holds_alternative<unsigned char>(*it)) {
                char c = get<unsigned char>(*it);
                if (c == '(') {     // empiezo a leer el nombre desde aquí (no lo incluyo) '('
                    leyendo = true;
                    continue;
                }
                if (c == ')') {     // termino de leerlo justo aquí (no lo incluyo) ')'
                    break;
                }
                if (leyendo) {
                    nombre += c;
                }
            }
        }
        return nombre; // Retornar el nombre del documento
    }

    // Si no se cumple la secuencia ".im ", retornar vacío
    return "";
}

string getNombreInc(vector<variant<int, unsigned char, string>>& linea) {
    // Iterador para la posición inicial del punto
    auto pos_ini = linea.end();  // Inicializado al final del vector como valor inválido

    // Buscar el primer punto ('.')
    for (auto caracter = linea.begin(); caracter != linea.end(); ++caracter) {
        if (holds_alternative<unsigned char>(*caracter) && get<unsigned char>(*caracter) == '.') {
            pos_ini = caracter; // Guardar posición del punto
            break;
        }
    }

    // Verificar si encontramos el punto
    if (pos_ini == linea.end()) {
        return ""; // No se encontró el punto, retornar vacío
    }

    // Validar la secuencia ".im "
    if (distance(pos_ini, linea.end()) > 3 && // Asegurarse de que hay espacio suficiente
        //holds_alternative<unsigned char>(*(pos_ini + 1)) && get<unsigned char>(*(pos_ini + 1)) == ' ' &&
        holds_alternative<unsigned char>(*(pos_ini + 2)) && get<unsigned char>(*(pos_ini + 1)) == 'i' &&
        holds_alternative<unsigned char>(*(pos_ini + 3)) && get<unsigned char>(*(pos_ini + 2)) == 'm' &&
        holds_alternative<unsigned char>(*(pos_ini + 4)) && get<unsigned char>(*(pos_ini + 3)) == ' ') {

        // Extraer el nombre entre paréntesis
        string nombre;
        bool leyendo = false;
        for (auto it = pos_ini + 4; it != linea.end(); ++it) {
            if (holds_alternative<unsigned char>(*it)) {
                char c = get<unsigned char>(*it);
                if (c == '(') {     // empiezo a leer el nombre desde aquí (no lo incluyo) '('
                    leyendo = true;
                    continue;
                }
                if (c == ')') {     // termino de leerlo justo aquí (no lo incluyo) ')'
                    break;
                }
                if (leyendo) {
                    nombre += c;
                }
            }
        }
        return nombre; // Retornar el nombre del documento
    }

    // Si no se cumple la secuencia ".im ", retornar vacío
    return "";
}

// Insertar incrustación dentro del documento original

void insertarIncrustacion(vector<vector<variant<int, unsigned char, string>>>& original, vector<vector<variant<int, unsigned char, string>>>& nuevo, int linea_inc) {

    // compruebo que está dentro del rango del documto antes de nada
    if (linea_inc < 0 || linea_inc > original.size()) {
        cerr << "Error: Línea de inserción fuera de rango." << endl;
        return;
    }

    /*
        Original            Nuevo (pos. 1)

        [0] Original a      [0] Original a
        [1] Original b      [1] \ Nuevo 1 \
        [2] Original c      [2] \ Nuevo 2 \
                            [3] Original b
                            [4] Original c

    */

    // Insertar todas las líneas del vector nuevo desde una posición específica
    original.insert(original.begin() + linea_inc, nuevo.begin(), nuevo.end());
}

// Comprobar si es una línea de incrustación

bool esIncrutacion(vector<variant<int, unsigned char, string>>& linea) {

    // .im (doc_incrustado)

    if ((holds_alternative<unsigned char>(linea[0]) && get<unsigned char>(linea[0]) == '.') &&
        (holds_alternative<unsigned char>(linea[1]) && get<unsigned char>(linea[1]) == 'i') &&
        (holds_alternative<unsigned char>(linea[2]) && get<unsigned char>(linea[2]) == 'm')) {
        return true;
    }
    else {
        return false;
    }
}

// Comprobar si es una línea de incrustación

bool esIncrutacion(vector<variant<int, unsigned char>>& linea) {

    // .im (doc_incrustado)

    if ((holds_alternative<unsigned char>(linea[0]) && get<unsigned char>(linea[0]) == '.') &&
        (holds_alternative<unsigned char>(linea[1]) && get<unsigned char>(linea[1]) == 'i') &&
        (holds_alternative<unsigned char>(linea[2]) && get<unsigned char>(linea[2]) == 'm')) {
        return true;
    }
    else {
        return false;
    }
}

// Eliminar inicios de párrafo

void eliminarIniciosParrafo(vector<vector<variant<int, unsigned char, string>>>& estiloDocMix) {

    for (auto& linea : estiloDocMix) {

        // Comprobar si la línea comienza con un entero 0 seguido de un carácter 'a' - 'z'
        if (!linea.empty() &&
            holds_alternative<int>(linea[0]) && get<int>(linea[0]) == 0 &&
            linea.size() > 1 && holds_alternative<unsigned char>(linea[1])) {

            unsigned char caracter = get<unsigned char>(linea[1]);
            if (caracter >= 'a' && caracter <= 'z') {
                // Si cumple con la condición, eliminamos los dos primeros elementos (0 y el carácter)
                linea.erase(linea.begin(), linea.begin() + 2);
            }
        }
    }
}




// Inserta espacios duros en lugar de espacios " " y [0]

vector<vector<variant<int, unsigned char, string>>> procesarEspaciosDuros(const vector<vector<variant<int, unsigned char, string>>>& estiloDocMix) {

    vector<vector<variant<int, unsigned char, string>>> resultado = estiloDocMix;

    // Recorremos el vector principal
    for (auto& fila : resultado) {
        // Recorremos cada elemento de cada fila
        for (auto& celda : fila) {
            // Comprobamos el tipo de cada celda y realizamos el reemplazo
            if (holds_alternative<int>(celda)) {
                int valor = get<int>(celda);
                if (valor == 0) {
                    celda = "&nbsp;";  // Reemplazamos el 0 por un espacio duro
                }
            }
            else if (holds_alternative<unsigned char>(celda)) {
                unsigned char valor = get<unsigned char>(celda);
                if (valor == ' ') {
                    celda = "&nbsp;";  // Reemplazamos el espacio por un espacio duro
                }
            }
            // Si es un string, no se hace nada
        }
    }

    return resultado;
}


// Elimina retornos de carro e inicio de línea

vector<vector<variant<int, unsigned char, string>>> procesarSaltosLinea(const vector<vector<variant<int, unsigned char, string>>>& estiloDocMix) {

    vector<vector<variant<int, unsigned char, string>>> resultado;

    for (const auto& linea : estiloDocMix) {
        vector<variant<int, unsigned char, string>> nuevaLinea; // Línea limpia
        for (const auto& caracter : linea) {
            if (holds_alternative<int>(caracter)) {
                int valor = get<int>(caracter);
                if (valor == 10) { // Si es 10, agregar salto de línea HTML
                    nuevaLinea.push_back("<br>");
                }
                else if (valor != 0 && valor != 13) { // Eliminar 0 y 13
                    nuevaLinea.push_back(caracter);
                }
            }
            else if (holds_alternative<unsigned char>(caracter)) {
                unsigned char valor = get<unsigned char>(caracter);
                if (valor != 13 && valor != 10) { // Eliminar 13 y 10
                    nuevaLinea.push_back(caracter);
                }
            }
            else if (holds_alternative<string>(caracter)) {
                nuevaLinea.push_back(caracter); // Los strings se copian directamente
            }
        }
        resultado.push_back(nuevaLinea); // Agregar la línea procesada al resultado
    }

    return resultado; // Retornar el nuevo vector
}

// Insertar cabecera HTML

string insertarCabeceraHTML() {
    return R"(
        <html xmlns:v="urn:schemas-microsoft-com:vml"
              xmlns:o="urn:schemas-microsoft-com:office:office"
              xmlns:w="urn:schemas-microsoft-com:office:word"
              xmlns:m="http://schemas.microsoft.com/office/2004/12/omml"
              xmlns="http://www.w3.org/TR/REC-html40">
        <head>
        </head>
        <body lang="ES" style="tab-interval:35.4pt;word-wrap:break-word">
        <div style="font-size:10.0pt;font-family:&quot;Courier New&quot;">
        )";
}

// Insertar pie HTML
string insertarPieHTML() {
    return R"(
        </div>
        </body>
        </html>
        )";
}

// Vuelca el documento en un nuevo archivo

void volcarEnArchivo(const string& nombreArchivo, const vector<vector<variant<int, unsigned char, string>>>& estiloDocMix) {
    ofstream archivo(nombreArchivo); // Abrir o crear el archivo

    if (!archivo) {
        cerr << "No se pudo abrir el archivo " << nombreArchivo << " para escritura." << endl;
        return;
    }

    // agregar la cabecera
    archivo << insertarCabeceraHTML();

    for (const auto& linea : estiloDocMix) {
        for (const auto& caracter : linea) {
            if (holds_alternative<int>(caracter)) {
                archivo << "[" << get<int>(caracter) << "]";
            }
            else if (holds_alternative<unsigned char>(caracter)) {
                archivo << static_cast<char>(get<unsigned char>(caracter));
            }
            else if (holds_alternative<string>(caracter)) {
                archivo << get<string>(caracter);
            }
        }
        archivo << endl; // Agregar salto de línea al final de cada línea
    }

    // agregar el pie
    archivo << insertarPieHTML();


    archivo.close(); // Cerrar el archivo
    cout << "El contenido se ha guardado en '" << nombreArchivo << "'." << endl;
}

// Obtiene el inicio de marca HTML

string getIniMarca(int especial) {

    string marca = "";

    if (especial == 0) {        // '°'
        marca = "<u>";
    }
    else if (especial == 1) {   // 'µ' y '\'
        marca = "<b>";
    }
    else if (especial == 2) {   // ¢
        marca = "<u><b>";
    }
    else {
        cout << "ERROR: Interpretado de simbolo especial invalido (ini. marca) con codigo " << especial << endl;
    }
    return marca;
}

// Obtiene el fin de marca HTML

string getFinMarca(int especial) {

    string marca = "";

    if (especial == 0) {        // °
        marca = "</u>";
    }
    else if (especial == 1) {   // 'µ' y '\'
        marca = "</b>";
    }
    else if (especial == 2) {   // ¢
        marca = "</b></u>";
    }
    else {
        //cout << "ERROR: Interpretado de simbolo especial invalido (fin. marca) con codigo: " << especial << endl;
    }
    return marca;
}

// Comprueba si una línea mixta es un párrafo

bool esParrafo(vector<variant<int, unsigned char>>& linea) {

    // comprueba si el primer elemento es un int igual a 0 (NULL)
    if (holds_alternative<int>(linea[0]) && get<int>(linea[0]) == 0) {

        // comprueba si el segundo elemento es un char en minúsculas ('a' a 'z')
        if (holds_alternative<unsigned char>(linea[1])) {
            unsigned char c = get<unsigned char>(linea[1]);
            if (c >= 'a' && c <= 'z') {
                return true;
            }
        }
    }
    return false;
}

// Comprueba si una línea mixta es de control

bool esControl(vector<variant<int, unsigned char>>& linea) {

    // comprueba que tiene, al menos, dos elementos
    if (linea.size() < 2) {
        return false;
    }

    // Condición 1: Comienza con más de dos ints que sean 0 (NULLs)
    size_t zerosCount = 0;
    for (size_t i = 0; i < linea.size(); ++i) {
        if (holds_alternative<int>(linea[i]) && get<int>(linea[i]) == 0) {
            ++zerosCount;
        }
        else {
            break; // Deja de contar si encuentra un elemento distinto de int 0
        }
    }
    if (zerosCount > 2) {
        return true;
    }

    // Condición 2: Comienza con uno de los enteros específicos
    static const vector<int> controlInts = { 176, 181, 162, 123, 92, 159, 216 };
    if (holds_alternative<int>(linea[0])) {
        int firstInt = get<int>(linea[0]);
        for (int controlInt : controlInts) {
            if (firstInt == controlInt) {
                return true;
            }
        }
    }

    // Si no cumple ninguna de las condiciones, no es una línea de control
    return false;
}

// Comprueba si una línea de enteros es de control

bool esControl(vector<int>& linea) {

    // comprueba que tiene, al menos, dos elementos
    if (linea.size() < 2) {
        return false;
    }

    // Condición 1: Comienza con más de dos ints que sean 0 (NULLs)
    size_t zerosCount = 0;
    for (size_t i = 0; i < linea.size(); ++i) {
        if (linea[i] == 0) {
            ++zerosCount;
        }
        else {
            break; // Deja de contar si encuentra un elemento distinto de int 0
        }
    }
    if (zerosCount > 2) {
        return true;
    }

    // Condición 2: Comienza con uno de los enteros específicos
    static const vector<int> controlInts = { 176, 181, 162, 123, 92, 159, 216 };
    int firstInt = linea[0];
    for (int controlInt : controlInts) {
        if (firstInt == controlInt) {
            return true;
        }
    }


    // Si no cumple ninguna de las condiciones, no es una línea de control
    return false;

}

// --------------------> FUNCIONES AUXILIARES <------------------------- //


// Imprime por pantalla el documento binario

void imprimirBinario(vector< vector< bitset<8> > >& doc_binario) {

    for (const auto& linea : doc_binario) {
        for (const auto& byte : linea) {
            cout << byte << " ";
        }
        cout << endl;
    }
}

// Imprimir por pantalla el documento decimal

void imprimirDocumentoDecimal(const vector<vector<int>>& docDec) {

    for (const auto& linea : docDec) {
        for (const auto& num : linea) {
            cout << num << " ";
        }
        cout << endl;
    }
}

// Imprimir por pantalla el documento ASCII

void imprimirDocumentoASCII(const vector<vector<unsigned char>>& docASCII) {

    for (const auto& linea : docASCII) {
        for (const auto& num : linea) {
            cout << num << " ";
        }
        cout << endl;
    }
}

// Imprimir documento mixto

void imprimirDocumentoMixto(const vector<vector<variant<int, unsigned char>>>& docMix) {
    for (const auto& linea : docMix) {
        for (const auto& elem : linea) {
            if (holds_alternative<int>(elem)) {
                // Imprimir números con corchetes
                cout << "[" << get<int>(elem) << "]";
            }
            else if (holds_alternative<unsigned char>(elem)) {
                unsigned char c = get<unsigned char>(elem);
                if (c >= 32 && c <= 127) {
                    // Imprimir caracteres legibles directamente
                    cout << c;
                }
                else {
                    // Imprimir caracteres no legibles como números entre corchetes
                    cout << "[" << static_cast<int>(c) << "]";
                }
            }
        }
        cout << endl;  // Nueva línea al final de cada línea del documento
    }
}
