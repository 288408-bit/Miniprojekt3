#include "Benchmark.hpp"
#include "HashTableList.hpp"
#include "HashTableAVL.hpp"
#include "HashTableOpen.hpp"
#include "utility.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <random>
#include <cstdlib>         // system()
#include <unordered_set>   // wykorzystywany WYLACZNIE do przygotowania
                            // unikalnych danych testowych, nie jest czescia
                            // implementacji zadnej z badanych struktur danych

using namespace std;

// ------------------------- Parametry badania -------------------------------
// 8 punktow pomiarowych (rozmiarow struktury) zgodnie z wytycznymi (>= 8, >= 5000).
static const int SIZES[] = {5000, 10000, 20000, 30000, 40000, 60000, 80000, 100000};
static const int NUM_SIZES = sizeof(SIZES) / sizeof(SIZES[0]);

// Liczba roznych ziaren (instancji danych) na jeden rozmiar problemu.
// Wynik usredniamy po wszystkich ziarnach -> przyblizenie przypadku sredniego.
static const int NUM_SEEDS = 10;

// Liczba kopii struktury (i jednoczesnie liczba mierzonych operacji) na jedno
// ziarno. Pojedyncza operacja jest mierzona na osobnej kopii o ustalonym
// rozmiarze, dzieki czemu w trakcie pomiaru rozmiar struktury sie NIE zmienia.
static int repetitions = 100;

// Ustalone (powtarzalne) ziarno bazowe. Te same dane wejsciowe mozna
// odtworzyc przy kazdym uruchomieniu programu, a wszystkie warianty struktur
// sa badane dokladnie na tych samych danych.
static const unsigned int BASE_SEED = 123456u;

// Zakresy losowania kluczy. Klucze "do wstawienia" pochodza z zakresu rozlacznego
// z kluczami bazowymi, dzieki czemu wstawiany element na pewno nie istnieje
// jeszcze w strukturze (badamy czysty koszt wstawienia nowego klucza).
static const int KEY_MIN = 1;
static const int KEY_MAX = 1000000000;
static const int INS_MIN = 1000000001;
static const int INS_MAX = 2000000000;

// Generuje dokladnie 'count' UNIKALNYCH kluczy z zakresu [lo, hi].
// Gwarancja unikalnosci sprawia, ze rozmiar struktury po wypelnieniu jest
// rowny deklarowanemu N (brak duplikatow nadpisujacych istniejace klucze),
// a wiec os OX wykresu odpowiada rzeczywistej liczbie elementow.
static int* generate_unique_keys(int count, mt19937& gen, int lo, int hi) {
    int* keys = new int[count];
    unordered_set<int> seen;
    seen.reserve(count * 2);
    uniform_int_distribution<int> dist(lo, hi);
    int filled = 0;
    while (filled < count) {
        int k = dist(gen);
        if (seen.insert(k).second) keys[filled++] = k;
    }
    return keys;
}

// Mierzy laczny czas (w ns) operacji insert oraz remove dla pojedynczego
// ziarna, na zadanej strukturze typu DictType, korzystajac z PRZEKAZANYCH,
// wspolnych danych (tych samych dla wszystkich wariantow struktur).
template <typename DictType>
static void measure_on_data(int data_size,
                            const int* base_keys,
                            const int* keys_to_insert,
                            const int* keys_to_remove,
                            long long& out_insert_ns,
                            long long& out_remove_ns) {
    // Krok 1: przygotowanie kopii testowych PRZED pomiarem czasu.
    // Kazda kopia wypelniana jest tym samym zestawem kluczy bazowych.
    DictType** copies = new DictType*[repetitions];
    for (int i = 0; i < repetitions; i++) {
        copies[i] = new DictType(data_size);   // pre-alokacja przyspiesza setup
        for (int j = 0; j < data_size; j++) {
            copies[i]->insert(base_keys[j], base_keys[j]);
        }
    }

    // Krok 2: pomiar operacji INSERT (po jednej, nowej parze na kazda kopie).
    auto start = chrono::steady_clock::now();
    for (int i = 0; i < repetitions; i++) {
        copies[i]->insert(keys_to_insert[i], 123);
    }
    auto end = chrono::steady_clock::now();
    out_insert_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    // Krok 3: pomiar operacji REMOVE (po jednym, gwarantowanie istniejacym
    // kluczu na kazda kopie; kolejnosc kluczy jest losowa).
    start = chrono::steady_clock::now();
    for (int i = 0; i < repetitions; i++) {
        copies[i]->remove(keys_to_remove[i]);
    }
    end = chrono::steady_clock::now();
    out_remove_ns = chrono::duration_cast<chrono::nanoseconds>(end - start).count();

    for (int i = 0; i < repetitions; i++) delete copies[i];
    delete[] copies;
}

// Zbiera wyniki dla jednego rozmiaru problemu, usredniajac po NUM_SEEDS ziarnach.
// Akumulatory indeksowane: 0 = lista lancuchowa, 1 = AVL, 2 = adresowanie otwarte.
static void run_size(int data_size,
                     long long ins_total[3], long long rem_total[3]) {
    for (int k = 0; k < 3; k++) { ins_total[k] = 0; rem_total[k] = 0; }

    for (int s = 0; s < NUM_SEEDS; s++) {
        // Ustalone ziarno -> powtarzalnosc i te same dane dla wszystkich struktur.
        mt19937 gen(BASE_SEED + (unsigned int)s);

        int* base_keys = generate_unique_keys(data_size, gen, KEY_MIN, KEY_MAX);

        // Klucze do wstawienia: rozlaczny zakres => na pewno nieobecne w strukturze.
        uniform_int_distribution<int> dist_ins(INS_MIN, INS_MAX);
        int* keys_to_insert = new int[repetitions];
        for (int i = 0; i < repetitions; i++) keys_to_insert[i] = dist_ins(gen);

        // Klucze do usuniecia: losowa kolejnosc istniejacych kluczy bazowych.
        int* shuffled = new int[data_size];
        for (int i = 0; i < data_size; i++) shuffled[i] = base_keys[i];
        custom_shuffle(shuffled, data_size, gen);
        int* keys_to_remove = new int[repetitions];
        for (int i = 0; i < repetitions; i++) keys_to_remove[i] = shuffled[i];

        long long ins_ns, rem_ns;

        measure_on_data<HashTableList>(data_size, base_keys, keys_to_insert, keys_to_remove, ins_ns, rem_ns);
        ins_total[0] += ins_ns; rem_total[0] += rem_ns;

        measure_on_data<HashTableAVL>(data_size, base_keys, keys_to_insert, keys_to_remove, ins_ns, rem_ns);
        ins_total[1] += ins_ns; rem_total[1] += rem_ns;

        measure_on_data<HashTableOpen>(data_size, base_keys, keys_to_insert, keys_to_remove, ins_ns, rem_ns);
        ins_total[2] += ins_ns; rem_total[2] += rem_ns;

        delete[] base_keys;
        delete[] keys_to_insert;
        delete[] shuffled;
        delete[] keys_to_remove;
    }
}

void run_measurements() {
    const char* names[3] = {"HashList", "HashAVL", "HashOpen"};

    // Utworzenie katalogu na wyniki (przenosnie: Windows oraz systemy uniksowe).
#ifdef _WIN32
    int rc = system("if not exist results mkdir results");
#else
    int rc = system("mkdir -p results");
#endif
    (void)rc;
    ofstream f_ins("results/insert_times.csv");
    ofstream f_rem("results/remove_times.csv");
    f_ins << "Struktura;Rozmiar;Czas[ns]\n";
    f_rem << "Struktura;Rozmiar;Czas[ns]\n";

    cout << "\nUsrednianie po " << NUM_SEEDS << " ziarnach, "
         << repetitions << " pomiarach na ziarno.\n";
    cout << string(64, '-') << "\n";
    cout << left << setw(20) << "Struktura"
         << right << setw(10) << "Rozmiar(N)"
         << setw(16) << "Insert[ns]"
         << setw(16) << "Remove[ns]" << "\n";
    cout << string(64, '-') << "\n";

    for (int si = 0; si < NUM_SIZES; si++) {
        int data_size = SIZES[si];

        long long ins_total[3], rem_total[3];
        run_size(data_size, ins_total, rem_total);

        long long denom = (long long)NUM_SEEDS * repetitions;

        // Zabezpieczenie przed "sztucznym zerem": gdyby usredniony czas wyszedl
        // zerowy (zbyt krotka operacja wzgledem rozdzielczosci zegara),
        // podwajamy liczbe pomiarow i powtarzamy badanie dla tego rozmiaru.
        bool zero_detected = false;
        for (int k = 0; k < 3; k++) {
            if (ins_total[k] / denom == 0 || rem_total[k] / denom == 0) zero_detected = true;
        }
        if (zero_detected) {
            cout << "  [info] Wykryto zerowy pomiar dla N=" << data_size
                 << " - zwiekszam liczbe powtorzen i ponawiam.\n";
            repetitions *= 2;
            run_size(data_size, ins_total, rem_total);
            denom = (long long)NUM_SEEDS * repetitions;
        }

        for (int k = 0; k < 3; k++) {
            long long ins_avg = ins_total[k] / denom;
            long long rem_avg = rem_total[k] / denom;

            cout << left << setw(20) << names[k]
                 << right << setw(10) << data_size
                 << setw(16) << ins_avg
                 << setw(16) << rem_avg << "\n";

            f_ins << names[k] << ";" << data_size << ";" << ins_avg << "\n";
            f_rem << names[k] << ";" << data_size << ";" << rem_avg << "\n";
        }
        cout << string(64, '-') << "\n";
    }

    f_ins.close();
    f_rem.close();
    cout << "Badania zakonczone. Wyniki zapisano w folderze results/\n";
}
