#pragma once
#include <random>

// Wlasna implementacja tasowania (algorytm Fishera-Yatesa).
// Wykorzystywana w czesci badawczej do losowego wyboru kolejnosci usuwanych
// (istniejacych) kluczy, tak aby usuwanie nie odbywalo sie zawsze w tej samej
// kolejnosci, w jakiej elementy byly wstawiane.
template <typename T>
void custom_shuffle(T* array, int size, std::mt19937& gen) {
    for (int i = size - 1; i > 0; --i) {
        std::uniform_int_distribution<int> dist(0, i);
        int j = dist(gen);
        T temp = array[i];
        array[i] = array[j];
        array[j] = temp;
    }
}
