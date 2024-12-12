// Implementación de la clase BloomFilter

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>

using namespace std;
// Definimos long long como ll
typedef long long ll;

class BloomFilter {
    vector<bool> M;
    vector<ll> primes;
    ll k;
    ll m;
    ll BigPrime = 1e9 + 7;

    //Constructor
    public:
    BloomFilter(ll m_, ll k_, vector<ll>& prim_buffer) {
        // Inicializar el vector M con n ceros
        M = vector<bool>(m_, false);
        // Inicializar k y m
        this->k = k_;
        this->m = m_;
        // Inicializar el vector de primos y copiar los primeros k primos
        primes.resize(k);
        copy_n(prim_buffer.begin(), k, primes.begin());
    }


    //Inpirado en GeeksforGeeks, Polynomial Rolling Hash Function

    // Método para calcular el hash de un string
    // s: string a calcular el hash
    // p: primo a utilizar
    // return: hash del string s
    ll hash (const string& s, ll p){
        ll hash_so_far = 0;
        ll p_pow = 1;
        const ll n = s.length();
        for (ll i = 0; i < n; ++i) {
            ll char_value = s[i] >= 'a' && s[i] <= 'z' ? (s[i] - 'a' + 1) :
                        s[i] >= 'A' && s[i] <= 'Z' ? (s[i] - 'A' + 27) :
                        s[i] >= '0' && s[i] <= '9' ? (s[i] - '0' + 53) :
                        static_cast<ll>(static_cast<unsigned char>(s[i]) + 100);
            hash_so_far = (hash_so_far + char_value * p_pow) % BigPrime;
            if (hash_so_far < 0) {
                hash_so_far += BigPrime;
            }
            p_pow = (p_pow * p) % BigPrime;
        }
        return hash_so_far;
    }

    // Método para agregar un string al filtro de Bloom
    // s: string a agregar
    void add(const string& s){
        for (ll i = 0; i < k; ++i) {
            ll h = hash(s, primes[i]);
            M[h % m] = true;
        }
    }

    // Método para buscar un string en el filtro de Bloom
    // s: string a buscar
    // return: true si el string está en el filtro, false en caso contrario
    bool lookup(const string& s){
        for (ll i = 0; i < k; ++i) {
            ll h = hash(s, primes[i]);
            if (h < 0) {
                return false;
            }
            if (!M[h % m]) {
                return false;
            }
        }
        return true;
    }
};