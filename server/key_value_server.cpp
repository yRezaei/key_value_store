#include "key_value_server.hpp"
#include "cpp_helpers/network_message.hpp"

KeyValueServer::KeyValueServer(std::uint16_t port)
    : key_value_size(0),
      socket(std::make_unique<net::Server>(net::TCP, port))
{
}

KeyValueServer::~KeyValueServer()
{
}

void KeyValueServer::handle_requests()
{
    std::string identity;
    std::stringstream stream;
    if (socket->receive(identity, stream))
    {
        if (net::SendMessage header; stream >> header)
        {
            switch (header.data_type)
            {
            case static_cast<std::underlying_type<net::NetworkCommand>::type>(net::NetworkCommand::PUT_COMMAND):
            {
                if (net::PutCommand cmd; stream >> cmd)
                {
                    std::stringstream stream;
                    if (key_value_store.find(cmd.key) == key_value_store.end())
                    {
                        key_value_store[cmd.key] = cmd.value;
                        std::cout << "A request for adding an item '" << cmd.key << ": " << cmd.value << "' was received." << std::endl;
                        auto response_data = net::KeyAddedResponseData(cmd.key, cmd.value);
                        stream << net::ResponseMessage(header.id, response_data.type()) << response_data;
                        socket->send(identity, stream.str());
                        print_key_value();
                    }
                    else
                    {
                        std::cout << "A request for adding an already existing item '" << cmd.key << ": " << cmd.value << "' was received." << std::endl;
                        auto response_data = net::KeyAlreadyExistResponseData(cmd.key);
                        stream << net::ResponseMessage(header.id, response_data.type()) << response_data;
                        socket->send(identity, stream.str());
                    }
                }
                break;
            }
            case static_cast<std::underlying_type<net::NetworkCommand>::type>(net::NetworkCommand::GET_COMMAND):
            {
                if (net::GetCommand cmd; stream >> cmd)
                {
                    std::stringstream stream;
                    if (key_value_store.find(cmd.key) != key_value_store.end())
                    {
                        std::cout << "A request for value of the key '" << cmd.key << "' was received." << std::endl;
                        auto response_data = net::KeyValueResponseData(cmd.key, key_value_store[cmd.key]);
                        stream << net::ResponseMessage(header.id, response_data.type()) << response_data;
                        socket->send(identity, stream.str());
                        print_key_value();
                    }
                    else
                    {
                        std::cout << "A request for value of an unknown key '" << cmd.key << "' was received." << std::endl;
                        auto response_data = net::KeyNotExistResponseData(cmd.key);
                        stream << net::ResponseMessage(header.id, response_data.type()) << response_data;
                        socket->send(identity, stream.str());
                    }
                }
                break;
            }
            case static_cast<std::underlying_type<net::NetworkCommand>::type>(net::NetworkCommand::DELETE_COMMAND):
            {
                if (net::DeleteCommand cmd; stream >> cmd)
                {
                    std::stringstream stream;
                    if (key_value_store.find(cmd.key) != key_value_store.end())
                    {
                        key_value_store.erase(cmd.key);
                        std::cout << "A request for removing an item with key '" << cmd.key << "' was received." << std::endl;
                        auto response_data = net::KeyDeletedResponseData(cmd.key);
                        stream << net::ResponseMessage(header.id, response_data.type()) << response_data;
                        socket->send(identity, stream.str());
                        print_key_value();
                    }
                    else
                    {
                        std::cout << "A request for removing an unknown item with key '" << cmd.key << "' was received." << std::endl;
                        auto response_data = net::KeyNotExistResponseData(cmd.key);
                        stream << net::ResponseMessage(header.id, response_data.type()) << response_data;
                        socket->send(identity, stream.str());
                    }
                }
                break;
            }

            default:
                break;
            }
        }
    }
}

double KeyValueServer::item_size(const std::pair<std::string, std::string> &item)
{
    return (item.first.size() / 1024.0 / 1024.0) + (item.second.size() / 1024.0 / 1024.0);
}


void KeyValueServer::print_key_value()
{
    std::cout << "Store status:" << '\n';
    for(auto&[key, value] : key_value_store)
    {
        std::cout << "\tItem: " << key << ": " << value << '\n';
    }
    std::cout << std::endl;
}