#pragma once

// Abstrakcyjny typ danych "slownik" (ADT).
// Klasa czysto wirtualna pelniaca role wspolnego interfejsu dla wszystkich
// wariantow tablicy mieszajacej. Dzieki niej kod badawczy (Benchmark) oraz menu
// operuja na strukturach przez jeden wspolny typ, niezaleznie od ich implementacji.
class IDictionary {
public:
    // Wstawia pare (klucz, wartosc). Jezeli klucz juz istnieje, aktualizuje wartosc.
    virtual void insert(int key, int value) = 0;

    // Usuwa pare powiazana z podanym kluczem (jezeli istnieje).
    virtual void remove(int key) = 0;

    // Wyszukuje klucz. Zwraca true i ustawia out_value, gdy klucz zostal znaleziony.
    virtual bool find(int key, int& out_value) const = 0;

    // Zwraca liczbe aktualnie przechowywanych par klucz-wartosc.
    virtual int return_size() const = 0;

    // Wypisuje skrocona informacje o stanie struktury (na potrzeby menu).
    virtual void display() const = 0;

    // Usuwa wszystkie elementy, przywracajac strukture do stanu pustego.
    virtual void clear() = 0;

    virtual ~IDictionary() {}
};
