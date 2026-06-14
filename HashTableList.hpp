#pragma once
#include "IDictionary.hpp"

// Wariant 1: tablica mieszajaca z adresowaniem zamknietym,
// w ktorej kolizje rozwiazywane sa metoda lancuchowa.
// Kazdy kubelek jest glowa wlasnej (samodzielnie zaimplementowanej)
// listy jednokierunkowej przechowujacej pary klucz-wartosc.
class HashTableList : public IDictionary {
private:
    // Wezel wlasnej listy jednokierunkowej.
    struct Node {
        int key;
        int value;
        Node* next;
    };

    Node** table;   // dynamiczna tablica wskaznikow na glowy list (kubelkow)
    int capacity;   // liczba kubelkow
    int size;       // liczba przechowywanych par klucz-wartosc

    // Wspolczynnik wypelnienia, po przekroczeniu ktorego nastepuje rehash.
    // Dla metody lancuchowej 0.75 to klasyczna, dobrze zbalansowana wartosc:
    // utrzymuje srednia dlugosc lancucha ponizej 1, czyli operacje O(1) srednio.
    const float MAX_LOAD_FACTOR = 0.75f;

    int hash_function(int key) const;   // odwzorowanie klucza na indeks kubelka
    void rehash();                      // podwojenie pojemnosci i ponowne rozmieszczenie

public:
    HashTableList(int init_capacity = 16);
    ~HashTableList() override;

    void insert(int key, int value) override;
    void remove(int key) override;
    bool find(int key, int& out_value) const override;
    int return_size() const override;
    void display() const override;
    void clear() override;
};
