#pragma once
#include "IDictionary.hpp"

// Wariant 3: tablica mieszajaca z adresowaniem otwartym i probkowaniem liniowym.
// Wszystkie elementy przechowywane sa bezposrednio w jednej tablicy; w razie
// kolizji szukany jest kolejny wolny slot (indeks + 1, modulo pojemnosc).
// Usuwanie realizowane jest metoda leniwa (tzw. "nagrobek" / tombstone),
// aby nie przerywac sekwencji probkowania innych elementow.
class HashTableOpen : public IDictionary {
private:
    // Pojedynczy slot tablicy. Mozliwe stany:
    //  - PUSTY:   is_occupied == false             (slot nigdy nieuzywany; konczy probkowanie)
    //  - ZAJETY:  is_occupied && !is_deleted        (slot przechowuje aktywny element)
    //  - NAGROBEK:is_occupied && is_deleted         (element usuniety; probkowanie idzie dalej)
    struct Entry {
        int key;
        int value;
        bool is_occupied;
        bool is_deleted;
    };

    Entry* table;
    int capacity;   // liczba slotow
    int size;       // liczba aktywnych elementow (bez nagrobkow)
    int used;       // liczba slotow zajetych lub bedacych nagrobkiem

    // Dla adresowania otwartego 0.7 to praktyczna granica, powyzej ktorej
    // zjawisko grupowania (primary clustering) silnie wydluza sekwencje
    // probkowania i degraduje zlozonosc operacji.
    const float MAX_LOAD_FACTOR = 0.7f;

    int hash_function(int key) const;
    void rehash();

public:
    HashTableOpen(int init_capacity = 16);
    ~HashTableOpen() override;

    void insert(int key, int value) override;
    void remove(int key) override;
    bool find(int key, int& out_value) const override;
    int return_size() const override;
    void display() const override;
    void clear() override;
};
