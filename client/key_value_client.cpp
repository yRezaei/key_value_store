#include "key_value_client.hpp"

KeyValueClient::KeyValueClient(const std::string &identity, const std::string &ip_str, std::uint16_t port)
    : identity_(identity),
      socket(std::make_unique<net::Client>(identity_, net::TCP, net::IP(ip_str), port)),
      stop_thread(false)

{
    thread = std::thread([&]() {
        std::stringstream incoming_msg;
        while (stop_thread == false)
        {
            if (net::ResponseMessage header; socket->receive(incoming_msg) && incoming_msg >> header)
            {
                switch (header.data_type)
                {
                    case static_cast<std::underlying_type<net::NetworkResponse>::type>(net::NetworkResponse::KEY_ADDED):
                    {
                        net::KeyAddedResponseData response;
                        incoming_msg >> response;
                        std::cout << "The item '" << response.key << ": " << response.value << "' was successfully added to the store." << std::endl;
                        break;
                    }
                    case static_cast<std::underlying_type<net::NetworkResponse>::type>(net::NetworkResponse::KEY_VALUE):
                    {
                        net::KeyValueResponseData response;
                        incoming_msg >> response;
                        std::cout << "The value of the key '" << response.key << "' is '" << response.value << "'" << std::endl;
                        break;
                    }
                    case static_cast<std::underlying_type<net::NetworkResponse>::type>(net::NetworkResponse::KEY_DELETED):
                    {
                        net::KeyDeletedResponseData response;
                        incoming_msg >> response;
                        std::cout << "The item with key '" << response.key << "' was successfully removed from the store." << std::endl;
                        break;
                    }
                    case static_cast<std::underlying_type<net::NetworkResponse>::type>(net::NetworkResponse::KEY_DOES_NOT_EXIST):
                    {
                        net::KeyNotExistResponseData response;
                        incoming_msg >> response;
                        std::cout << "The item with key '" << response.key << "' does NOT exist in the store." << std::endl;
                        break;
                    }
                    case static_cast<std::underlying_type<net::NetworkResponse>::type>(net::NetworkResponse::KEY_ALREADY_EXIST):
                    {
                        net::KeyNotExistResponseData response;
                        incoming_msg >> response;
                        std::cout << "AN item with key '" << response.key << "' already exist in the store." << std::endl;
                        break;
                    }
                    default:
                        break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(33));
        }
    });
}

KeyValueClient::~KeyValueClient()
{
    stop();
}

void KeyValueClient::stop()
{
    stop_thread = true;
    if (thread.joinable())
        thread.join();
}