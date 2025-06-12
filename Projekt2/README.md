# SO2_2 - Wielowątkowy serwer czatu

## Opis projektu

Projekt składa się z dwóch aplikacji: serwera i klienta, które umożliwiają komunikację tekstową wielu użytkowników przez sieć lokalną (TCP/IP). Każdy klient po podłączeniu się do serwera podaje swoją nazwę użytkownika, a następnie może wysyłać wiadomości widoczne dla wszystkich innych użytkowników.

---

## Instrukcje uruchomienia projektów

### Linux

1. **Kompilacja:**
    ```sh
    g++ server.cpp -o server -pthread
    g++ client.cpp -o client -pthread
    ```

2. **Uruchomienie serwera:**
    ```sh
    ./server
    ```

3. **Uruchomienie klienta (w osobnych terminalach):**
    ```sh
    ./client
    ```

---

## Opis problemu/ów

- **Brak wsparcia dla Windows:** Kod korzysta z nagłówków POSIX (`<sys/socket.h>`, `<netinet/in.h>`, `<unistd.h>`), które nie są dostępne na Windows. Do uruchomienia na Windowsie wymagane są zmiany na WinSock.
- **Zarządzanie wieloma klientami:** Serwer musi obsługiwać wielu klientów jednocześnie, zapewniając bezpieczeństwo współdzielonych zasobów (lista klientów).
- **Synchronizacja dostępu:** Współdzielona lista klientów wymaga synchronizacji przy dodawaniu, usuwaniu i rozsyłaniu wiadomości.

---

## Wątki i ich rola

- **Wątek główny serwera:** Nasłuchuje na porcie 8080 i akceptuje nowe połączenia.
- **Wątki obsługi klientów:** Dla każdego nowego klienta serwer tworzy osobny wątek (`std::thread(clientSession, client_fd).detach();`), który:
    - Odbiera nazwę użytkownika.
    - Obsługuje odbiór i rozsyłanie wiadomości od tego klienta.
    - Informuje o dołączeniu/wyjściu użytkownika.

- **Wątek odbioru wiadomości w kliencie:** Klient uruchamia osobny wątek do odbierania wiadomości z serwera, aby użytkownik mógł jednocześnie pisać i odbierać czat.

---

## Sekcje krytyczne i ich rozwiązanie

- **Sekcja krytyczna:** Współdzielona lista połączonych klientów (`connectedClients`).
    - **Zagrożenie:** Jednoczesny dostęp wielu wątków (dodawanie, usuwanie, rozsyłanie wiadomości) może prowadzić do błędów i wyścigów.
    - **Rozwiązanie:** Zastosowanie mutexa (`std::mutex clientsLock`) oraz `std::lock_guard<std::mutex>` przy każdej operacji na liście klientów:
        - Dodawanie klienta do listy.
        - Usuwanie klienta z listy.
        - Rozsyłanie wiadomości do wszystkich klientów.

### Kod odpowiedzialny za sekcje krytyczne

```cpp
// Protecting access to the shared clients list
std::vector<ConnectedClient> connectedClients;
std::mutex clientsLock;

// Sending a message to all clients except the sender
void sendToAll(const std::string& msg, int exclude_fd) {
    std::lock_guard<std::mutex> guard(clientsLock);
    for (auto& client : connectedClients) {
        if (client.socket_fd != exclude_fd) {
            send(client.socket_fd, msg.c_str(), msg.size(), 0);
        }
    }
}

// Adding a client to the list
{
    std::lock_guard<std::mutex> guard(clientsLock);
    connectedClients.push_back({client_fd, userName});
}

// Removing a client from the list
{
    std::lock_guard<std::mutex> guard(clientsLock);
    connectedClients.erase(
        std::remove_if(
            connectedClients.begin(),
            connectedClients.end(),
            [client_fd](const ConnectedClient& c) { return c.socket_fd == client_fd; }),
        connectedClients.end()
    );
}
```

---

## Przykład działania

```
$ ./server
Server running on port 8080

$ ./client
Please enter your username: imię_1
>> imię_1 has connected.
imię_1: Cześć wszystkim!

$ ./client
Please enter your username: imię_2
>> imię_2 has connected.
imię_1: Cześć wszystkim!
imię_2: Hej imię_1!
```

---

## Zależności

- Kompilator C++ (np. g++)
- System UNIX/Linux (wymagane nagłówki POSIX: `<unistd.h>`, `<arpa/inet.h>`, `<sys/socket.h>`, `<netinet/in.h>`)
- Wątkowanie (`-pthread`)

---

## Autorzy

Adrian Nurkowski (272859)  
Maciej Myśków (272794)