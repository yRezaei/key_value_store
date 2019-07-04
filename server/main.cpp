#include <iostream>
#include <memory>
#include <csignal>
#include "key_value_server.hpp"

namespace
{
volatile std::sig_atomic_t gSignalStatus;
}

void signal_handler(int signal)
{
    gSignalStatus = signal;
}

int main(int argc, char *argv[])
{
    std::signal(SIGINT, signal_handler);

    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        KeyValueServer key_value_server(std::stoi(argv[1]));

        std::cout << "Server started ..." << std::endl;
        while (gSignalStatus == 0)
        {
            key_value_server.handle_requests();
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}