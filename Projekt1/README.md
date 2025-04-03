# Problem Jedzących Filozofów (ang. Dining Philosophers Problem)

## Opis problemu
Problem Jedzących Filozofów to klasyczny problem synchronizacji w informatyce. Grupa filozofów siedzi przy okrągłym stole, a między każdym z nich znajduje się widelec. Każdy filozof na przemian myśli i je. Aby jeść, filozof musi podnieść dwa widelce – jeden z lewej i jeden z prawej strony. Problem polega na tym, aby zapewnić, że:
- **Nie dochodzi do zakleszczenia (ang. deadlock)**, czyli sytuacji, w której żaden filozof nie może jeść.
- **Nie dochodzi do zagłodzenia (ang. starvation)**, czyli sytuacji, w której jeden z filozofów nigdy nie dostaje szansy na jedzenie.

## Wymagania
- Kompilator obsługujący standard C++11 (np. g++)
- System operacyjny z obsługą wątków (Linux, macOS, Windows z WSL/Mingw)

## Instrukcje uruchomienia projektu
1. Pobierz repozytorium lub sklonuj je na swój komputer.
2. Skompiluj projekt za pomocą następującego polecenia:
    ```sh
    g++ -std=c++11 -o dpp main.cpp -lpthread
    ```
3. Uruchom skompilowany program, podając liczbę filozofów i liczbę powtórzeń jako argumenty:
    ```sh
    ./dpp <liczba_filozofów> <liczba_powtórzeń>
    ```
    Na przykład:
    ```sh
    ./dpp 5 4
    ```
    Powyższe polecenie uruchomi symulację dla 5 filozofów, w której każdy zje 4 razy.

## Wątki
Program wykorzystuje wielowątkowość, gdzie każdy filozof jest reprezentowany przez osobny wątek. Wątek filozofa wykonuje następujące czynności w pętli:
1. **Myślenie**: Filozof spędza losowy czas na myśleniu.
2. **Podnoszenie widelców**: Filozof próbuje podnieść dwa widelce – jeden z lewej i jeden z prawej strony.
3. **Jedzenie**: Po podniesieniu obu widelców filozof je przez losowy czas.
4. **Odkładanie widelców**: Po jedzeniu filozof odkłada oba widelce, aby inni mogli z nich skorzystać.

## Sekcje krytyczne i ich rozwiązanie
### Sekcje krytyczne
1. **Dostęp do widelców**: Tylko jeden filozof może korzystać z danego widelca w danym momencie.
2. **Wypisywanie na konsolę**: Wiele wątków może próbować jednocześnie wypisywać dane na konsolę, co prowadzi do nieczytelnych wyników.

### Rozwiązanie
1. **Dostęp do widelców**:
   - Każdy widelec jest chroniony przez własny semafor binarny (`Semaphore`).
   - Filozof musi zdobyć semafory dla obu widelców, zanim zacznie jeść.
2. **Wypisywanie na konsolę**:
   - Synchronizacja wypisywania na konsolę jest realizowana za pomocą semafora binarnego (`consoleSemaphore`), który zapewnia, że tylko jeden wątek w danym momencie ma dostęp do konsoli.

## Kod odpowiedzialny za sekcje krytyczne
### Podnoszenie widelców
```cpp
void pickUpForks(int id) {
    int leftFork = id; // Indeks lewego widelca
    int rightFork = (id + 1) % numPhilosophers; // Indeks prawego widelca

    // Czekaj na dostępność lewego widelca
    forkSemaphores[leftFork].wait();
    // Czekaj na dostępność prawego widelca
    forkSemaphores[rightFork].wait();

    updateState(id, "Picked up forks"); // Aktualizacja stanu filozofa
}
```

### Odkładanie widelców
```cpp
void putDownForks(int id) {
    int leftFork = id; // Indeks lewego widelca
    int rightFork = (id + 1) % numPhilosophers; // Indeks prawego widelca

    // Zwolnij lewy widelec
    forkSemaphores[leftFork].signal();
    // Zwolnij prawy widelec
    forkSemaphores[rightFork].signal();

    updateState(id, "Put down forks"); // Aktualizacja stanu filozofa
}
```

### Wypisywanie na konsolę
```cpp
void printStates() {
    consoleSemaphore.wait(); // Zablokowanie semafora
    for (int i = 0; i < numPhilosophers; ++i) {
        cout << "Philosopher " << i << ": " << states[i] << endl;
    }
    cout << "---------------------------------" << endl; // Separator dla czytelności
    consoleSemaphore.signal(); // Zwolnienie semafora
}
```

## Wyjście programu
Poniżej przedstawiono przykładowe wyjście programu dla 5 filozofów:
```
Philosopher 0: Thinking
Philosopher 1: Thinking
Philosopher 2: Thinking
Philosopher 3: Thinking
Philosopher 4: Thinking
---------------------------------
Philosopher 0: Picked up forks
Philosopher 1: Thinking
Philosopher 2: Thinking
Philosopher 3: Thinking
Philosopher 4: Thinking
---------------------------------
Philosopher 0: Eating
Philosopher 1: Thinking
Philosopher 2: Thinking
Philosopher 3: Thinking
Philosopher 4: Thinking
---------------------------------
```

## Uwagi
- Program wykorzystuje losowe opóźnienia, aby symulować myślenie i jedzenie filozofów.
- Synchronizacja za pomocą semaforów zapewnia brak zakleszczenia (deadlock) i zagłodzenia (starvation).
- Wydruk na konsolę jest czytelny dzięki synchronizacji za pomocą semafora binarnego.
