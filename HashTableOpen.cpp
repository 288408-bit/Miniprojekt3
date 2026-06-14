#include "HashTableOpen.hpp"
#include <iostream>

HashTableOpen::HashTableOpen(int init_capacity) : capacity(init_capacity), size(0), used(0) {
    if (capacity < 1) capacity = 1;
    table = new Entry[capacity];
    for (int i = 0; i < capacity; i++) {
        table[i].is_occupied = false;
        table[i].is_deleted = false;
    }
}

HashTableOpen::~HashTableOpen() { delete[] table; }

int HashTableOpen::hash_function(int key) const {
    int hash = key % capacity;
    return hash < 0 ? hash + capacity : hash;
}

void HashTableOpen::rehash() {
    int old_capacity = capacity;
    Entry* old_table = table;

    capacity *= 2;
    table = new Entry[capacity];
    for (int i = 0; i < capacity; i++) {
        table[i].is_occupied = false;
        table[i].is_deleted = false;
    }
    // Ponowne wstawienie tylko aktywnych elementow. Nagrobki znikaja,
    // dzieki czemu rehash jednoczesnie porzadkuje tablice (size == used).
    size = 0;
    used = 0;

    for (int i = 0; i < old_capacity; i++) {
        if (old_table[i].is_occupied && !old_table[i].is_deleted) {
            insert(old_table[i].key, old_table[i].value);
        }
    }
    delete[] old_table;
}

void HashTableOpen::insert(int key, int value) {
    // Wspolczynnik wypelnienia liczony od slotow zajetych I nagrobkow, aby
    // nagromadzone nagrobki same w sobie wymusily rehash i nie pozwolily
    // sekwencjom probkowania degenerowac sie do dlugich przebiegow.
    if ((float)used / capacity >= MAX_LOAD_FACTOR) rehash();

    int index = hash_function(key);
    int start_index = index;
    int first_free = -1;   // pierwszy napotkany nagrobek mozliwy do ponownego uzycia

    // Probkowanie liniowe az do napotkania slotu PUSTEGO lub pelnego cyklu.
    while (table[index].is_occupied) {
        if (!table[index].is_deleted && table[index].key == key) {
            table[index].value = value;   // klucz juz istnieje: aktualizacja wartosci
            return;
        }
        if (table[index].is_deleted && first_free == -1) {
            first_free = index;
        }
        index = (index + 1) % capacity;
        if (index == start_index) break;   // przebyto cala tablice
    }

    int target;
    if (!table[index].is_occupied) {
        // Napotkano slot PUSTY -> klucza na pewno nie ma w tablicy.
        // Jezeli po drodze byl nagrobek, wstawiamy w nim (oszczedzamy slot).
        target = (first_free != -1) ? first_free : index;
    } else {
        // Pelny cykl bez slotu pustego.
        if (first_free == -1) { rehash(); insert(key, value); return; }
        target = first_free;
    }

    bool was_empty = !table[target].is_occupied;
    table[target].key = key;
    table[target].value = value;
    table[target].is_occupied = true;
    table[target].is_deleted = false;
    if (was_empty) used++;   // nagrobek byl juz wliczony do used
    size++;
}

void HashTableOpen::remove(int key) {
    int index = hash_function(key);
    int start_index = index;

    while (table[index].is_occupied) {
        if (!table[index].is_deleted && table[index].key == key) {
            table[index].is_deleted = true;   // usuwanie leniwe: pozostawiamy nagrobek
            size--;
            return;
        }
        index = (index + 1) % capacity;
        if (index == start_index) break;
    }
}

bool HashTableOpen::find(int key, int& out_value) const {
    int index = hash_function(key);
    int start_index = index;

    while (table[index].is_occupied) {
        if (!table[index].is_deleted && table[index].key == key) {
            out_value = table[index].value;
            return true;
        }
        index = (index + 1) % capacity;
        if (index == start_index) break;
    }
    return false;
}

int HashTableOpen::return_size() const { return size; }

void HashTableOpen::clear() {
    for (int i = 0; i < capacity; i++) {
        table[i].is_occupied = false;
        table[i].is_deleted = false;
    }
    size = 0;
    used = 0;
}

void HashTableOpen::display() const {
    std::cout << "  Rozmiar: " << size << ", Pojemnosc: " << capacity
              << " (adresowanie otwarte, probkowanie liniowe)\n";
    int printed = 0;
    for (int i = 0; i < capacity && printed < 10; i++) {
        if (table[i].is_occupied && !table[i].is_deleted) {
            std::cout << "    Slot [" << i << "]: {" << table[i].key << ":" << table[i].value << "}\n";
            printed++;
        }
    }
    if (size > 10) std::cout << "    ...\n";
}
