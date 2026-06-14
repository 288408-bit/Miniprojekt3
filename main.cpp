#include <iostream>
#include <string>
#include <fstream>
#include <random>

#include "IDictionary.hpp"
#include "HashTableList.hpp"
#include "HashTableAVL.hpp"
#include "HashTableOpen.hpp"
#include "Benchmark.hpp"

using namespace std;

// Wczytuje pary (klucz, wartosc) z pliku tekstowego, zachowujac kolejnosc z pliku.
// Przed wczytaniem nowych danych usuwa poprzednia zawartosc struktury.
void load_from_file(IDictionary& dict, const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "  [Blad] Nie udalo sie otworzyc pliku: " << filename << "\n";
        return;
    }
    dict.clear();
    int key, value;
    int count = 0;
    while (file >> key >> value) {
        dict.insert(key, value);
        count++;
    }
    cout << "  Zaladowano " << count << " elementow z pliku.\n";
}

// Wypelnia strukture losowo wygenerowanymi parami (klucz, wartosc).
void generate_random(IDictionary& dict) {
    int n;
    cout << "Podaj wielkosc struktury do wygenerowania: ";
    cin >> n;

    dict.clear();
    mt19937 gen(random_device{}());
    uniform_int_distribution<int> dist(1, 1000000);

    for (int i = 0; i < n; i++) {
        dict.insert(dist(gen), dist(gen));
    }
    cout << "  Wygenerowano " << n << " losowych elementow.\n";
}

// Menu operacyjne wspolne dla kazdego wariantu slownika (drugi poziom menu).
void structure_menu(IDictionary& dict, const string& name) {
    int choice;
    do {
        cout << "\n--- " << name << " ---\n"
             << "  1. Zbuduj z pliku\n"
             << "  2. Dodaj element (insert)\n"
             << "  3. Usun element (remove)\n"
             << "  4. Znajdz element (find)\n"
             << "  5. Utworz losowo\n"
             << "  6. Rozmiar (size)\n"
             << "  7. Wyswietl (display)\n"
             << "  8. Wyczysc (clear)\n"
             << "  0. Wroc\n"
             << "Wybor: ";
        cin >> choice;

        if (choice == 1) {
            string path;
            cout << "Podaj nazwe pliku: "; cin >> path;
            load_from_file(dict, path);
        } else if (choice == 2) {
            int key, val;
            cout << "Klucz: "; cin >> key;
            cout << "Wartosc: "; cin >> val;
            dict.insert(key, val);
            cout << "  Dodano.\n";
        } else if (choice == 3) {
            int key; cout << "Klucz do usuniecia: "; cin >> key;
            dict.remove(key);
            cout << "  Wykonano operacje usuniecia.\n";
        } else if (choice == 4) {
            int key, val; cout << "Klucz do znalezienia: "; cin >> key;
            if (dict.find(key, val)) cout << "  Znaleziono wartosc: " << val << "\n";
            else cout << "  Brak klucza w slowniku.\n";
        } else if (choice == 5) {
            generate_random(dict);
        } else if (choice == 6) {
            cout << "  Rozmiar: " << dict.return_size() << "\n";
        } else if (choice == 7) {
            dict.display();
        } else if (choice == 8) {
            dict.clear(); cout << "  Wyczyszczono strukture.\n";
        }
    } while (choice != 0);
}

int main() {
    HashTableList dictList;
    HashTableAVL dictAVL;
    HashTableOpen dictOpen;

    int choice;
    do {
        cout << "\n===== MINIPROJEKT 3 - TABLICE MIESZAJACE =====\n"
             << "  1. Tablica mieszajaca (metoda lancuchowa)\n"
             << "  2. Tablica mieszajaca (drzewa AVL)\n"
             << "  3. Tablica mieszajaca (adresowanie otwarte)\n"
             << "  4. Uruchom badania numeryczne (benchmark)\n"
             << "  0. Wyjscie\nWybor: ";
        cin >> choice;

        if (choice == 1) structure_menu(dictList, "Tablica mieszajaca z lista");
        else if (choice == 2) structure_menu(dictAVL, "Tablica mieszajaca z AVL");
        else if (choice == 3) structure_menu(dictOpen, "Tablica z adresowaniem otwartym");
        else if (choice == 4) run_measurements();
    } while (choice != 0);

    return 0;
}
