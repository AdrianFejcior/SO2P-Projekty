#include <iostream>
#include <thread>
#include <vector>
#include <atomic>
#include <chrono>

using namespace std;

// Custom binary semaphore
class Semaphore {
private:
    atomic<bool> flag; // Flag indicating resource availability

public:
    Semaphore() : flag(true) {}

    void wait() {
        while (!flag.exchange(false)) {
            this_thread::yield(); // Yield to other threads
        }
    }

    void signal() {
        flag.store(true);
    }
};

class DiningPhilosophers {
private:
    int numPhilosophers; // Number of philosophers
    int repetitions; // Number of repetitions (how many times each philosopher eats)
    vector<Semaphore> forkSemaphores; // Semaphores for each fork
    vector<string> states; // States of philosophers (e.g., Thinking, Eating)
    Semaphore consoleSemaphore; // Semaphore for synchronizing console output

    // Function executed by each philosopher thread
    void philosopher(int id) {
        for (int i = 0; i < repetitions; ++i) {
            think(id); // Simulate thinking
            pickUpForks(id); // Picking up forks
            eat(id); // Simulate eating
            putDownForks(id); // Putting down forks
        }
    }

    // Simulate philosopher thinking
    void think(int id) {
        updateState(id, "Thinking"); // Update philosopher's state
        this_thread::sleep_for(chrono::milliseconds(1000 + rand() % 2000)); // Random delay
    }

    // Philosopher picks up forks (left and right)
    void pickUpForks(int id) {
        int leftFork = id; // Index of the left fork
        int rightFork = (id + 1) % numPhilosophers; // Index of the right fork

        // Wait for the left fork to become available
        forkSemaphores[leftFork].wait();
        // Wait for the right fork to become available
        forkSemaphores[rightFork].wait();

        updateState(id, "Picked up forks"); // Update philosopher's state
    }

    // Simulate philosopher eating
    void eat(int id) {
        updateState(id, "Eating"); // Update philosopher's state
        this_thread::sleep_for(chrono::milliseconds(1000 + rand() % 2000)); // Random delay
    }

    // Philosopher puts down forks (left and right)
    void putDownForks(int id) {
        int leftFork = id; // Index of the left fork
        int rightFork = (id + 1) % numPhilosophers; // Index of the right fork

        // Release the left fork
        forkSemaphores[leftFork].signal();
        // Release the right fork
        forkSemaphores[rightFork].signal();

        updateState(id, "Put down forks"); // Update philosopher's state
    }

    // Update philosopher's state and print all states
    void updateState(int id, const string& state) {
        states[id] = state; // Update philosopher's state
        printStates(); // Print the current states of all philosophers
    }

    // Print the states of all philosophers
    void printStates() {
        consoleSemaphore.wait(); // Lock the semaphore
        for (int i = 0; i < numPhilosophers; ++i) {
            cout << "Philosopher " << i << ": " << states[i] << endl;
        }
        cout << "---------------------------------" << endl; // Separator for readability
        consoleSemaphore.signal(); // Release the semaphore
    }

public:
    // Constructor initializing the dining philosophers problem
    DiningPhilosophers(int numPhilosophers, int repetitions)
        : numPhilosophers(numPhilosophers), repetitions(repetitions),
          forkSemaphores(numPhilosophers), states(numPhilosophers, "Thinking") {}

    // Start the simulation
    void start() {
        vector<thread> philosophers;

        // Create threads for each philosopher
        for (int i = 0; i < numPhilosophers; ++i) {
            philosophers.emplace_back(&DiningPhilosophers::philosopher, this, i);
        }

        // Wait for all threads to finish
        for (auto& philosopher : philosophers) {
            philosopher.join();
        }
    }
};

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <number_of_philosophers> <repetitions>" << endl;
        return 1;
    }

    int numPhilosophers = stoi(argv[1]);
    int repetitions = stoi(argv[2]);

    if (numPhilosophers < 2 || repetitions < 1) {
        cerr << "Number of philosophers must be >= 2 and repetitions >= 1." << endl;
        return 1;
    }

    // Create an instance of the DiningPhilosophers class and start the simulation
    DiningPhilosophers dp(numPhilosophers, repetitions);
    dp.start();

    return 0;
}