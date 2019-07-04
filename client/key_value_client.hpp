#ifndef KEY_VALUE_CLIENT_HPP_
#define KEY_VALUE_CLIENT_HPP_

#include <iostream>
#include <limits>
#include <memory>
#include <csignal>
#include <random>
#include <string>
#include <atomic>
#include "cpp_helpers/networking.hpp"
#include "cpp_helpers/network_message.hpp"

class KeyValueClient
{
private:
    std::string identity_;
    std::unique_ptr<net::Client> socket;
    std::atomic_bool stop_thread;
    std::thread thread;

public:
    KeyValueClient(const std::string &identity, const std::string &ip_str, std::uint16_t port);
    ~KeyValueClient();

    const std::string &identity() { return identity_; }
    void stop();

    template <typename T>
    inline void send(std::uint16_t id, T msg_data);
};

template <typename T>
inline void KeyValueClient::send(std::uint16_t id, T msg_data)
{
    static_assert(std::is_base_of<net::ISerializable, T>::value,
                  "msg_data must be driven from net::ISerializable.");
    std::stringstream stream;
    if (stream << net::SendMessage(id, msg_data.type()) << msg_data)
    {
        socket->send(stream.str());
    }
    else
    {
        std::cout << "Stream is invalide" << std::endl;
    }
}

#endif //!KEY_VALUE_CLIENT_HPP_
