#include <iostream>
#include <thread>
#include <mutex>
#include <vector>


using namespace std;

binary_semaphore clientMutex(0);         // Mutex pour signifier qu'un client est prêt à parler
binary_semaphore auditorMutex(0);        // Mutex pour signifier qu'un auditeur est prêt à parler
binary_semaphore clientDoneMutex(0);     // Mutex pour signifier qu'un client a fini de parler
binary_semaphore auditorDoneMutex(0);    // Mutex pour signifier qu'un auditeur a fini de parler

mutex clientInCallMutex;                        // Mutex pour ne pas avoir deux clients en même temps qui parle
mutex clientWaitingMutex;                       // Mutex pour ne pas avoir deux clients en même temps qui modifie ou consulte le nombre de clients en attente

const unsigned MAX_WAITING_CUSTOMERS = 5;       // Nombre maximum de clients en attente
unsigned waitingCustomers = 0;                  // Nombre de clients en attente

/**
 * Fonction qui représente le thread de l'auditeur
 * @return [[noreturn]] Ne retourne jamais car while(true)
 */
[[noreturn]] void auditorFct() {
    while (true) {
        cout << "Auditor: is waiting for client" << endl;
        clientMutex.acquire();

        cout << "Auditor: is notifying that he is ready to talk" << endl;
        auditorMutex.release();

        cout << "Auditor: is talking" << endl;
        this_thread::sleep_for(chrono::seconds(2));

        cout << "Auditor: is waiting for clientMutex to finish" << endl;
        clientDoneMutex.acquire();

        cout << "Auditor: is notifying that he is ready to finish" << endl;
        auditorDoneMutex.release();
    }
}

/**
 * Fonction qui représente le thread d'un client
 * @param name Nom du client
 */
void clientFct(const string& name) {
    cout << name + ": is check if he can wait" << endl;
    clientWaitingMutex.lock();
    if (waitingCustomers >= MAX_WAITING_CUSTOMERS) {
        cout << name + ": is leaving because queue is full" << endl;
        clientWaitingMutex.unlock();
        return;
    }
    ++waitingCustomers;
    clientWaitingMutex.unlock();
    cout << name + ": is waiting in queue" << endl;

    clientInCallMutex.lock();

    cout << name + ": is notifying that he is ready to talk" << endl;
    clientMutex.release();

    cout << name + ": is waiting for auditor" << endl;
    auditorMutex.acquire();

    cout << name + ": is talking" << endl;
    this_thread::sleep_for(chrono::seconds(2));

    cout << name + ": is notifying that he is done" << endl;
    clientDoneMutex.release();

    cout << name + ": is waiting for auditor to finish" << endl;
    auditorDoneMutex.acquire();

    clientWaitingMutex.lock();
    --waitingCustomers;
    cout << name + ": is leaving because he finished" << endl;
    clientWaitingMutex.unlock();

    clientInCallMutex.unlock();
}

int main() {
    vector<thread> clientThreads; // Vecteur de threads pour les clients

    thread auditorThread(auditorFct); // Thread de l'auditeur

    // Création des threads des clients
    for (unsigned i = 0; i < 10; ++i) {
        clientThreads.emplace_back(clientFct, "Client " + to_string(i));
    }

    // Join des threads
    auditorThread.join();
    for (thread &thread : clientThreads) {
        thread.join();
    }

    return 0;
}
