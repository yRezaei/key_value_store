#ifndef KEY_VALUE_SERVER_HPP_
#define KEY_VALUE_SERVER_HPP_

#include <iostream>
#include <memory>
#include <csignal>
#include <unordered_map>
#include "cpp_helpers/networking.hpp"

class KeyValueServer
{
private:
    double key_value_size;
    std::unordered_map<std::string, std::string> key_value_store;
    std::unique_ptr<net::Server> socket;

public:
    KeyValueServer(std::uint16_t port);
    ~KeyValueServer();

    void handle_requests();

private:
    double item_size(const std::pair<std::string, std::string> &item);
    void print_key_value();
};

#endif // !KEY_VALUE_SERVER_HPP_
