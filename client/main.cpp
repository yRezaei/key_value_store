#include <iostream>
#include <string>
#include "key_value_client.hpp"

namespace
{
volatile std::sig_atomic_t gSignalStatus;
}
void signal_handler(int signal);
const std::string get_unique_name();
std::uint16_t unique_id();


int main(int argc, char *argv[])
{
    std::signal(SIGINT, signal_handler);
    std::string line;
    try
    {
        if (argc != 3)
        {
            std::cerr << "Usage: client <Ip> <port>\n";
            return 1;
        }

        KeyValueClient key_value_client(get_unique_name(), std::string(argv[1]), std::stoi(argv[2]));
        //KeyValueClient key_value_client(get_unique_name(), "127.0.0.1", 45678);

        std::cout << key_value_client.identity() << " started ..." << std::endl;
        while (gSignalStatus == 0)
        {
            if (std::getline(std::cin, line))
            {
                auto tokkens = str::split(line, " ");
                if(tokkens.size() >= 2)
                {
                    if(tokkens[0] == "put" && tokkens.size() == 3)
                    {
                        key_value_client.send(unique_id(), net::PutCommand(tokkens[1], tokkens[2]) );
                        std::cout << "sending put command: " << tokkens[1] << ": " << tokkens[2] << std::endl;
                    }
                    else if(tokkens[0] == "get" && tokkens.size() == 2)
                    {
                        key_value_client.send(unique_id(), net::GetCommand(tokkens[1]));
                        std::cout << "sending get command: " << tokkens[1] << std::endl;
                    }
                    else if(tokkens[0] == "delete" && tokkens.size() == 2)
                    {
                        key_value_client.send(unique_id(), net::DeleteCommand(tokkens[1]));
                        std::cout << "sending delete command: " << tokkens[1] << std::endl;
                    }
                }
            }
        }
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}

void signal_handler(int signal)
{
    gSignalStatus = signal;
}

const std::string get_unique_name()
{
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<> distr(1, 1000000);
    return str::format("client_{}", std::to_string(distr(eng)));
}


std::uint16_t unique_id()
{
    static std::uint16_t unique_id_ = 0;
    return unique_id_++;
}