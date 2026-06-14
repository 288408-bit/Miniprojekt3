#pragma once
#include "IDictionary.hpp"

// Wariant 2: tablica mieszajaca z adresowaniem zamknietym, w ktorej kazdy kubelek
// jest korzeniem samodzielnie zaimplementowanego, samobalansujacego drzewa AVL.
// W odroznieniu od metody lancuchowej, koszt wyszukania/wstawienia/usuniecia
// w obrebie kubelka wynosi O(log k) zamiast O(k), gdzie k to liczba kolizji.
// Dzieki temu struktura znosi wysokie wspolczynniki wypelnienia bez gwaltownej
// degradacji zlozonosci.
class HashTableAVL : public IDictionary {
private:
    // Wezel drzewa AVL przechowywanego w pojedynczym kubelku.
    struct AVLNode {
        int key;
        int value;
        int height;     // wysokosc poddrzewa (potrzebna do balansowania)
        AVLNode* left;
        AVLNode* right;
    };

    AVLNode** table;   // tablica wskaznikow na korzenie drzew AVL (kubelkow)
    int capacity;      // liczba kubelkow
    int size;          // liczba przechowywanych par klucz-wartosc

    // Wyzszy dopuszczalny wspolczynnik wypelnienia niz w metodzie lancuchowej.
    // Logarytmiczny koszt operacji w drzewie AVL pozwala bezpiecznie utrzymywac
    // srednio kilka elementow na kubelek, oszczedzajac pamiec (mniej kubelkow).
    const float MAX_LOAD_FACTOR = 3.0f;

    int hash_function(int key) const;
    void rehash();

    // Funkcje pomocnicze drzewa AVL.
    int get_height(AVLNode* node) const;
    int get_balance(AVLNode* node) const;
    void update_height(AVLNode* node);
    AVLNode* rotate_right(AVLNode* y);
    AVLNode* rotate_left(AVLNode* x);
    AVLNode* insert_node(AVLNode* node, int key, int value, bool& is_new);
    AVLNode* min_value_node(AVLNode* node) const;
    AVLNode* remove_node(AVLNode* root, int key, bool& was_removed);
    void destroy_tree(AVLNode* node);
    void traverse_and_insert(AVLNode* node, HashTableAVL* new_hash);

public:
    HashTableAVL(int init_capacity = 16);
    ~HashTableAVL() override;

    void insert(int key, int value) override;
    void remove(int key) override;
    bool find(int key, int& out_value) const override;
    int return_size() const override;
    void display() const override;
    void clear() override;
};
