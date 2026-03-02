#include "client.h"
#include "server.h"

#include <csignal>
#include <thread>

void sigint_handler(int sig) {
    StopClient();
    StopServer();
}

int main() {
    signal(SIGINT, sigint_handler);
    {
        std::thread server(RunServer);
        std::thread client(RunClient);

        client.join();
        server.join();
    }

    return 0;
}
