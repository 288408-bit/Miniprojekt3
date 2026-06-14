#include "HashTableList.hpp"
#include <iostream>

HashTableList::HashTableList(int init_capacity) : capacity(init_capacity), size(0) {
    if (capacity < 1) capacity = 1;
    table = new Node*[capacity];
    for (int i = 0; i < capacity; i++) table[i] = nullptr;
}

HashTableList::~HashTableList() {
    clear();
    delete[] table;
}

int HashTableList::hash_function(int key) const {
    // Funkcja modulo. Zabezpieczenie reszty ujemnej gwarantuje poprawny indeks
    // takze dla ujemnych kluczy (4-bajtowa liczba calkowita ze znakiem).
    int hash = key % capacity;
    return hash < 0 ? hash + capacity : hash;
}

void HashTableList::rehash() {
    // Zwiekszenie pojemnosci dwukrotnie i ponowne rozmieszczenie wszystkich
    // elementow zgodnie z nowa pojemnoscia (zmienia sie wynik hash_function).
    int old_capacity = capacity;
    Node** old_table = table;

    capacity *= 2;
    table = new Node*[capacity];
    for (int i = 0; i < capacity; i++) table[i] = nullptr;
    size = 0;

    for (int i = 0; i < old_capacity; i++) {
        Node* current = old_table[i];
        while (current) {
            insert(current->key, current->value);
            Node* temp = current;
            current = current->next;
            delete temp;
        }
    }
    delete[] old_table;
}

void HashTableList::insert(int key, int value) {
    if ((float)size / capacity >= MAX_LOAD_FACTOR) rehash();

    int index = hash_function(key);
    Node* current = table[index];

    // Przeszukanie lancucha danego kubelka w poszukiwaniu istniejacego klucza.
    while (current) {
        if (current->key == key) {
            current->value = value;   // klucz juz istnieje: aktualizacja wartosci
            return;
        }
        current = current->next;
    }

    // Nowy klucz: dolaczenie na poczatek lancucha (operacja w czasie stalym).
    Node* new_node = new Node{key, value, table[index]};
    table[index] = new_node;
    size++;
}

void HashTableList::remove(int key) {
    int index = hash_function(key);
    Node* current = table[index];
    Node* prev = nullptr;

    while (current && current->key != key) {
        prev = current;
        current = current->next;
    }

    if (!current) return;   // brak elementu o podanym kluczu

    if (!prev) table[index] = current->next;   // usuwany element jest glowa lancucha
    else prev->next = current->next;

    delete current;
    size--;
}

bool HashTableList::find(int key, int& out_value) const {
    int index = hash_function(key);
    Node* current = table[index];
    while (current) {
        if (current->key == key) {
            out_value = current->value;
            return true;
        }
        current = current->next;
    }
    return false;
}

int HashTableList::return_size() const { return size; }

void HashTableList::clear() {
    for (int i = 0; i < capacity; i++) {
        Node* current = table[i];
        while (current) {
            Node* temp = current;
            current = current->next;
            delete temp;
        }
        table[i] = nullptr;
    }
    size = 0;
}

void HashTableList::display() const {
    std::cout << "  Rozmiar: " << size << ", Pojemnosc: " << capacity
              << " (kubelki z lista jednokierunkowa)\n";
    int printed = 0;
    for (int i = 0; i < capacity && printed < 10; i++) {
        if (table[i]) {
            std::cout << "    Kubelek [" << i << "]: ";
            Node* current = table[i];
            while (current) {
                std::cout << "{" << current->key << ":" << current->value << "} -> ";
                current = current->next;
            }
            std::cout << "nullptr\n";
            printed++;
        }
    }
    if (size > 10) std::cout << "    ...\n";
}
