# Wymagania projektowe dla VRP-EA-optimization

## 1. Wprowadzenie
Głównym celem projektu **VRP-EA-optimization** jest implementacja wybranego algorytmu heurystycznego do rozwiązania problemu optymalizacji dyskretnej oraz porównanie jakości uzyskanych rozwiązań z wynikami metody referencyjnej.

## 2. Cel projektu
- **Implementacja algorytmu heurystycznego:**  
  Zaimplementowanie algorytmu ewolucyjnego (z możliwością włączenia elementów optymalizacji lokalnej) do rozwiązania problemu VRP z utrudnieniem ograniczonej ładowności pojazdów.
  
- **Porównanie metod:**  
  Porównanie jakości uzyskanych rozwiązań z rezultatami metody odniesienia, np. za pomocą Google OR-Tools lub innego wybranego algorytmu.

- **Raport końcowy:**  
  Sporządzenie szczegółowego raportu w szablonie LaTeX, opisującego:
  - Charakterystykę problemu oraz wybraną metodę jego rozwiązania,
  - Przeprowadzone eksperymenty i badania,
  - Dokonaną analizę porównawczą wyników,
  - Wnioski i rekomendacje dotyczące dalszych badań lub modyfikacji.

## 3. Opis problemu
Problem optymalizacji, który zostanie rozwiązany w projekcie to **Vehicle Routing Problem (VRP)** z następującymi utrudnieniami:
- **Ograniczona ładowność pojazdów:** Każdy pojazd ma określony limit ładowności, który nie może być przekroczony.
- (Opcjonalnie) **Okienka czasowe:** Możliwość dodania ograniczeń czasowych, w których klienci mogą być obsłużeni.

## 4. Wymagania funkcjonalne
- **Import danych:**  
  - Wczytywanie danych wejściowych (np. z plików CSV lub JSON) zawierających informacje o klientach (współrzędne, zapotrzebowanie) oraz o pojazdach (pojemność, dostępność).

- **Inicjalizacja populacji:**  
  - Generowanie początkowej populacji rozwiązań przy użyciu metod losowych lub heurystycznych.

- **Operatory ewolucyjne:**  
  - Implementacja operatorów selekcji (np. turniejowej lub ruletkowej), krzyżowania (np. OX, PMX) oraz mutacji (np. swap, inversion).
  - **Utrudnienie:** Włączenie elementów optymalizacji lokalnej w ramach operacji ewolucyjnych.

- **Funkcja celu:**  
  - Ocena jakości rozwiązań na podstawie minimalizacji całkowitego kosztu operacyjnego (np. suma przebytych odległości) przy uwzględnieniu ograniczeń (ładowność, ewentualne okienka czasowe).

- **Raportowanie wyników:**  
  - Wyświetlanie wyników w konsoli oraz zapisywanie wyników do plików (CSV, tekstowych).

- **Interfejs użytkownika:**  
  - Minimalny interfejs (np. wiersz poleceń) umożliwiający konfigurację parametrów algorytmu i wybór zbioru danych.

## 5. Wymagania niefunkcjonalne
- **Wydajność:**  
  - Implementacja powinna być zoptymalizowana pod kątem szybkości działania, szczególnie przy dużych instancjach problemu.
  
- **Modularność i czytelność kodu:**  
  - Kod powinien być podzielony na moduły (np. osobne pliki dla algorytmu ewolucyjnego, logiki VRP, obsługi danych wejściowych) ułatwiające dalszy rozwój i utrzymanie.

- **Testowalność:**  
  - Wdrożenie testów jednostkowych (np. z użyciem GoogleTest) dla kluczowych funkcji systemu.

- **Dokumentacja:**  
  - Kompletna dokumentacja kodu oraz architektury projektu, ułatwiająca zrozumienie i rozwój systemu.
  
- **Skalowalność:**  
  - System powinien umożliwiać łatwe rozszerzenie o nowe funkcjonalności lub utrudnienia.

## 6. Metody odniesienia
- **Oprogramowanie specjalistyczne:**  
  - Wykorzystanie Google OR-Tools jako punktu odniesienia dla porównania wyników.

- **Alternatywne algorytmy:**  
  - Możliwość porównania wyników z innymi algorytmami heurystycznymi lub dokładnymi, ustalonymi z prowadzącym.

## 7. Raport końcowy
Finalny raport, sporządzony w LaTeX, powinien zawierać:
- Szczegółowy opis problemu optymalizacji i wybranej metody rozwiązania.
- Przegląd literatury oraz uzasadnienie wyboru metody.
- Opis implementacji, eksperymentów oraz przeprowadzonych badań.
- Analizę porównawczą wyników uzyskanych przez zaproponowany algorytm i metodę odniesienia.
- Wnioski oraz propozycje dalszych usprawnień.

## 8. Technologie i narzędzia
- **Język programowania:** C++ (standard C++17)
- **System budowania:** CMake
- **Testy jednostkowe:** GoogleTest
- **Narzędzia CI/CD:** Opcjonalnie (np. GitHub Actions lub GitLab CI)
- **Środowisko raportowe:** LaTeX
