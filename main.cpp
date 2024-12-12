#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
using namespace std;
using namespace chrono;

#include "bloom.h"
#include "busquedaSecuencial.h"

// Valores de N y p
vector<int> N_values = {1024, 4096, 16384, 65536};
vector<double> p_values = {0, 0.25, 0.5, 0.75, 1.0};
// Numeros primos
vector<ll> primos = {1020361, 1020379, 1020389 ,1020401,
                     1020407, 1020413, 1020419, 1020431,
                     1020451, 1020457, 1020491, 1020517,
                     1020529, 1020541, 1020557, 1020583,
                     1020589, 1020599, 1020619, 1020631};

struct Combination {
    int N;
    double p;
};

vector<Combination> generateCombinations(const vector<int>& N_values,
                                              const vector<double>& p_values) {
    vector<Combination> combinations;
    for (int N : N_values) {
        for (double p : p_values) {
            combinations.push_back({N, p});
        }
    }
    return combinations;
}

// Función para crear la data del filtro de bloom
vector<string> bloomData(int N, vector<string>& babys) {
    vector<string> data;
    random_shuffle(babys.begin(), babys.end());
    // Elegimos los primeros N elementos de babys
    for (int i = 0; i < N; i++) {
        data.push_back(babys[i]);
    }
    return data;
}

// Función para generar conjuntos de datos en proporción P de elementos que están en el filtro
// y 1-P de elementos que no están en el filtro
vector<string> generateData(int n, double P, vector<string>& babys, vector<string>& films) {
    vector<string> data;
    // Los elementos de films seran randomizados 
    random_shuffle(films.begin(), films.end());
    // Elegimos n * P elementos de babys y n * (1 - P) elementos de films
    int numBabys = static_cast<int>(n * P);
    int numFilms = n - numBabys;

    for (int i = 0; i < numBabys; i++) {
        data.push_back(babys[i % babys.size()]); // Reusar nombres de babys si es necesario
    }
    for (int i = 0; i < numFilms; i++) {
        data.push_back(films[i % films.size()]); // Reusar nombres de films si es necesario
    }

    // Randomizamos el vector de datos
    random_shuffle(data.begin(), data.end());
    return data;
}


int main() {
    cout << "------------------------- INICIO -------------------------" << endl;
    
    // archivo para guardar resultados
    ofstream archivo;
    cout << "Creando archivo de resultados" << endl;
    archivo.open("resultados.txt", fstream::out);
    archivo << "Resultados de la ejecución" << endl;
    archivo << endl;
    archivo.close();

    // Creamos las combinaciones de N y p
    vector<Combination> combinations = generateCombinations(N_values, p_values);
    // Leemos los CSV de Popular-Baby-Names-Final.csv y Film-Names.csv
    vector<string> babys = readCSV("Popular-Baby-Names-Final.csv");
    vector<string> films = readCSV("Film-Names.csv");

    // En un ciclo, para cada combinación de N y p, generamos un conjunto de datos
    // con proporción p de elementos en babys y 1-p de elementos en films
    
    for(const Combination& comb : combinations){
        cout << "Test para N: " << comb.N << " p: " << comb.p << endl;
        // Creamos un conjunto 
        vector<string> babyBloom = bloomData(comb.N, babys);
        // Creamos los conjuntos de datos
        vector<string> data = generateData(comb.N, comb.p, babyBloom, films);
        // Elegir m y k en funcion de N
        ll m = comb.N * 10; // se elige m para tener 7 funciones de hashing
        ll k = ceil((m / comb.N) * log(2));
        // Inicializar filtro de bloom
        BloomFilter bloom(m, k, primos);

        // Calculamos la probabilidad de falsos positivos. Probabilidad esperada teorica: 0.00819372 
        double P = pow(1 - exp(-static_cast<double>(k) * static_cast<double>(comb.N) / static_cast<double>(m)), static_cast<double>(k));
        
        // Agregamos los elementos de babybloom al filtro
        for (const string& s : babyBloom) {
            bloom.add(s);
        }


        // ------------------- Tests -------------------
        double negativoBloom = 0;
        double falsoPositivo = 0;
        vector<double> times_Sec(comb.N);
        vector<double> times_Bloom(comb.N);

        cout << "------------------------- BUSQUEDA SECUENCIAL -------------------------" << endl;
        auto begin_sec = high_resolution_clock::now();
        for(const string& palabra : data) {
            searchCSV(babyBloom,palabra);
        }
        auto end_sec = high_resolution_clock::now();

        // Calcula el tiempo transcurrido y lo almacena en el vector
        double time_sec = duration_cast<nanoseconds>(end_sec - begin_sec).count() / 1e9;

        cout << "------------------------- FILTRO DE BLOOM -------------------------" << endl;
        auto begin_bloom = high_resolution_clock::now();
        for(const string& palabra : data) {
            if(bloom.lookup(palabra)){
                if(!searchCSV(babyBloom,palabra)){
                    falsoPositivo++;
                }
            } else {
                negativoBloom++;
            }

        }
        auto end_bloom = high_resolution_clock::now();
        // Calcula el tiempo transcurrido y lo almacena en el vector
	double time_bloom = duration_cast<nanoseconds>(end_bloom - begin_bloom).count() / 1e9;
        // Calcula el porcentaje de error
        double pctError = negativoBloom+falsoPositivo > 0 ? falsoPositivo / (negativoBloom+falsoPositivo) : 0;

	// Guarda los resultados en un archivo 
	archivo.open("resultados.txt", fstream::app);

        // (|N|, p)
	archivo << "(|N|: " << comb.N << ", p: " << comb.p << ")" << endl;
	archivo << "Tiempo promedio busqueda secuencial para: " << time_sec << endl;
	archivo << "Tiempo promedio filtro de Bloom: " << time_bloom << endl;
        archivo << "Porcentaje falsos positivos: " << pctError << endl;
	archivo << endl;

		archivo.close();
    
    }
    cout << "------------------------- FIN -------------------------" << endl;

    return 0;
}

// how to compile: g++ -std=c++11 main.cpp -o mainPenguin
