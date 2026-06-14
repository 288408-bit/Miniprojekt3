Miniprojekt 3
=====================================================

Zaimplementowane warianty:
  1. Tablica mieszajaca z metoda lancuchowa (wlasna lista jednokierunkowa)
  2. Tablica mieszajaca z drzewami AVL w kubelkach (adresowanie zamkniete)
  3. Tablica mieszajaca z adresowaniem otwartym (probkowanie liniowe)

Badane operacje: insert(), remove().

Kompilacja (CMake):
  mkdir build
  cd build
  cmake -DCMAKE_BUILD_TYPE=Release ..
  make
  ./projekt

Obsluga:
  Menu glowne pozwala wybrac konkretna strukture (opcje 1-3) i wykonac na niej
  operacje (dodaj/usun/znajdz/wczytaj z pliku/utworz losowo/wyswietl/wyczysc),
  albo uruchomic komplet badan numerycznych (opcja 4).

Badania:
  Opcja 4 mierzy sredni czas operacji insert i remove w funkcji rozmiaru
  struktury dla 8 rozmiarow (5000 ... 100000). Dla kazdego rozmiaru badanie
  powtarzane jest dla 10 roznych, ustalonych (powtarzalnych) ziaren generatora,
  a wynik jest usredniany (przyblizenie przypadku sredniego). Pojedyncza operacja
  mierzona jest na osobnej kopii struktury o ustalonym rozmiarze, dzieki czemu
  rozmiar nie zmienia sie w trakcie pomiaru. Wszystkie trzy warianty badane sa
  na dokladnie tych samych danych wejsciowych.

  Wyniki zapisywane sa do plikow CSV (separator ';'):
    results/insert_times.csv
    results/remove_times.csv
  Czas podawany jest w nanosekundach [ns].

