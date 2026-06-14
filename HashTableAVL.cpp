#include "HashTableAVL.hpp"
#include <iostream>

HashTableAVL::HashTableAVL(int init_capacity) : capacity(init_capacity), size(0) {
    if (capacity < 1) capacity = 1;
    table = new AVLNode*[capacity];
    for (int i = 0; i < capacity; i++) table[i] = nullptr;
}

HashTableAVL::~HashTableAVL() {
    clear();
    delete[] table;
}

int HashTableAVL::hash_function(int key) const {
    int hash = key % capacity;
    return hash < 0 ? hash + capacity : hash;
}

int HashTableAVL::get_height(AVLNode* node) const { return node ? node->height : 0; }

int HashTableAVL::get_balance(AVLNode* node) const {
    return node ? get_height(node->left) - get_height(node->right) : 0;
}

void HashTableAVL::update_height(AVLNode* node) {
    if (node) {
        int hl = get_height(node->left);
        int hr = get_height(node->right);
        node->height = (hl > hr ? hl : hr) + 1;
    }
}

// Rotacja w prawo wokol wezla y (przywraca rownowage po przeciazeniu lewej strony).
HashTableAVL::AVLNode* HashTableAVL::rotate_right(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;
    x->right = y;
    y->left = T2;
    update_height(y);
    update_height(x);
    return x;
}

// Rotacja w lewo wokol wezla x (przywraca rownowage po przeciazeniu prawej strony).
HashTableAVL::AVLNode* HashTableAVL::rotate_left(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;
    y->left = x;
    x->right = T2;
    update_height(x);
    update_height(y);
    return y;
}

// Rekurencyjne wstawienie do drzewa AVL z przywroceniem rownowagi w drodze powrotnej.
HashTableAVL::AVLNode* HashTableAVL::insert_node(AVLNode* node, int key, int value, bool& is_new) {
    if (!node) {
        is_new = true;
        return new AVLNode{key, value, 1, nullptr, nullptr};
    }
    if (key < node->key) node->left = insert_node(node->left, key, value, is_new);
    else if (key > node->key) node->right = insert_node(node->right, key, value, is_new);
    else { node->value = value; return node; }   // klucz istnieje: aktualizacja wartosci

    update_height(node);
    int balance = get_balance(node);

    // Cztery przypadki niezrownowazenia drzewa AVL:
    if (balance > 1 && key < node->left->key) return rotate_right(node);            // LL
    if (balance < -1 && key > node->right->key) return rotate_left(node);           // RR
    if (balance > 1 && key > node->left->key) {                                     // LR
        node->left = rotate_left(node->left);
        return rotate_right(node);
    }
    if (balance < -1 && key < node->right->key) {                                   // RL
        node->right = rotate_right(node->right);
        return rotate_left(node);
    }
    return node;
}

HashTableAVL::AVLNode* HashTableAVL::min_value_node(AVLNode* node) const {
    AVLNode* current = node;
    while (current->left != nullptr) current = current->left;
    return current;
}

// Rekurencyjne usuniecie z drzewa AVL z przywroceniem rownowagi w drodze powrotnej.
HashTableAVL::AVLNode* HashTableAVL::remove_node(AVLNode* root, int key, bool& was_removed) {
    if (!root) return root;

    if (key < root->key) root->left = remove_node(root->left, key, was_removed);
    else if (key > root->key) root->right = remove_node(root->right, key, was_removed);
    else {
        was_removed = true;
        if (!root->left || !root->right) {
            // Wezel z co najwyzej jednym dzieckiem.
            AVLNode* temp = root->left ? root->left : root->right;
            if (!temp) { temp = root; root = nullptr; }   // brak dzieci
            else *root = *temp;                            // jedno dziecko
            delete temp;
        } else {
            // Wezel z dwojgiem dzieci: zastapienie nastepnikiem (min prawego poddrzewa).
            AVLNode* temp = min_value_node(root->right);
            root->key = temp->key;
            root->value = temp->value;
            root->right = remove_node(root->right, temp->key, was_removed);
        }
    }

    if (!root) return root;

    update_height(root);
    int balance = get_balance(root);

    // Przywrocenie rownowagi (przypadki analogiczne jak przy wstawianiu).
    if (balance > 1 && get_balance(root->left) >= 0) return rotate_right(root);
    if (balance > 1 && get_balance(root->left) < 0) {
        root->left = rotate_left(root->left);
        return rotate_right(root);
    }
    if (balance < -1 && get_balance(root->right) <= 0) return rotate_left(root);
    if (balance < -1 && get_balance(root->right) > 0) {
        root->right = rotate_right(root->right);
        return rotate_left(root);
    }
    return root;
}

void HashTableAVL::destroy_tree(AVLNode* node) {
    if (node) {
        destroy_tree(node->left);
        destroy_tree(node->right);
        delete node;
    }
}

// Przejscie drzewa (in-order) i wstawienie kazdej pary do nowej tablicy podczas rehashu.
void HashTableAVL::traverse_and_insert(AVLNode* node, HashTableAVL* new_hash) {
    if (!node) return;
    traverse_and_insert(node->left, new_hash);
    new_hash->insert(node->key, node->value);
    traverse_and_insert(node->right, new_hash);
}

void HashTableAVL::rehash() {
    int old_capacity = capacity;
    AVLNode** old_table = table;

    capacity *= 2;
    table = new AVLNode*[capacity];
    for (int i = 0; i < capacity; i++) table[i] = nullptr;
    size = 0;

    for (int i = 0; i < old_capacity; i++) {
        traverse_and_insert(old_table[i], this);
        destroy_tree(old_table[i]);
    }
    delete[] old_table;
}

void HashTableAVL::insert(int key, int value) {
    if ((float)size / capacity >= MAX_LOAD_FACTOR) rehash();
    int index = hash_function(key);
    bool is_new = false;
    table[index] = insert_node(table[index], key, value, is_new);
    if (is_new) size++;
}

void HashTableAVL::remove(int key) {
    int index = hash_function(key);
    bool was_removed = false;
    table[index] = remove_node(table[index], key, was_removed);
    if (was_removed) size--;
}

bool HashTableAVL::find(int key, int& out_value) const {
    // Wyszukiwanie binarne w obrebie drzewa AVL danego kubelka.
    int index = hash_function(key);
    AVLNode* current = table[index];
    while (current) {
        if (current->key == key) { out_value = current->value; return true; }
        else if (key < current->key) current = current->left;
        else current = current->right;
    }
    return false;
}

int HashTableAVL::return_size() const { return size; }

void HashTableAVL::clear() {
    for (int i = 0; i < capacity; i++) {
        destroy_tree(table[i]);
        table[i] = nullptr;
    }
    size = 0;
}

void HashTableAVL::display() const {
    std::cout << "  Rozmiar: " << size << ", Pojemnosc: " << capacity
              << " (kubelki z drzewami AVL)\n";
}
