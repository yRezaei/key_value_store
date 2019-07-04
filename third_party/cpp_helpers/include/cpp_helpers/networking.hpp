#pragma once
#ifndef TARJA_NETWORKING_HPP
#define TARJA_NETWORKING_HPP

#include <array>
#include <thread>
#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <mutex>
#include "cpp_helpers/string_utility.hpp"
#include "cpp_helpers/singleton.hpp"
#include <zmq.hpp>

namespace net
{
using Port = std::uint16_t;

enum TransportProtocol : std::uint8_t
{
    TCP,
    UDP
};

struct IP
{
    std::array<std::uint16_t, 4> data;

    IP() : data({127, 0, 0, 1})
    {
    }

    /* IP(const std::string& interface_name, net::IP_Type ip_type) : data({ 127, 0, 0, 1 })
		{
			auto[succeed, address] = get_ip_address(interface_name, ip_type);
            if(succeed)
                reset(address);
            else
                throw std::runtime_error(str::format("No ip address available for such a network interface '{}'!!!", interface_name));
		} */

    IP(const std::array<std::uint16_t, 4> &ip) : data(ip)
    {
    }

    IP(std::uint16_t p1, std::uint16_t p2, std::uint16_t p3, std::uint16_t p4)
    {
        reset(p1, p2, p3, p4);
    }

    IP(const std::string &ip_str)
    {
        reset(ip_str);
    }

    void reset(const std::string &ip_str)
    {
        auto tokkens = str::split(ip_str, "., ", false);
        if (tokkens.size() == 4)
        {
            for (auto i = 0u; i < tokkens.size(); i++)
            {
                data[i] = (std::uint16_t)std::stoi(tokkens[i]);
                if (data[i] <= 255)
                    continue;
                else
                {
                    data[0] = 127;
                    data[1] = 0;
                    data[2] = 0;
                    data[3] = 1;
                    break;
                }
            }
        }
    }

    void reset(std::uint16_t p1, std::uint16_t p2, std::uint16_t p3, std::uint16_t p4)
    {
        if (p1 <= 255 || p2 <= 255 || p3 <= 255 || p4 <= 255)
        {
            data[0] = p1;
            data[1] = p2;
            data[2] = p3;
            data[3] = p4;
        }
    }

    void reset(const std::array<std::uint16_t, 4> ip)
    {
        for (auto &i : ip)
        {
            if (i > 255)
                return;
        }
        data = ip;
    }

    std::string to_string() const
    {
        return std::to_string(data[0]) + "." + std::to_string(data[1]) + "." + std::to_string(data[2]) + "." + std::to_string(data[3]);
    }

    static IP loopback()
    {
        return IP();
    }
};

class Context : public Singleton<Context>
{
private:
    friend class Singleton<Context>;

private:
    zmq::context_t *context_;
    std::once_flag onceFlag_;

public:
    Context()
    {
        std::call_once(onceFlag_, [&] { context_ = new zmq::context_t(1); });
    }

    ~Context()
    {
        context_->close();
    }

    zmq::socket_t *create_socket(int socket_type)
    {
        return new zmq::socket_t(*context_, socket_type);
    }
};

class Client
{
private:
    zmq::socket_t *socket_;
    TransportProtocol protocol_;
    IP ip_;
    Port port_;

public:
    Client(const std::string &identity, TransportProtocol protocol, const IP &ip, const Port &port) : socket_(nullptr), protocol_(protocol), ip_(ip), port_(port)
    {
        socket_ = Context::instance().create_socket(ZMQ_DEALER);
        if (socket_)
        {
            socket_->setsockopt(ZMQ_IDENTITY, identity.data(), identity.size());
            socket_->connect((protocol_ == TCP ? std::string("tcp") : std::string("udp")).append("://") + ip_.to_string() + ":" + std::to_string(port_));
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    ~Client()
    {
        socket_->disconnect((protocol_ == TCP ? std::string("tcp") : std::string("udp")).append("://") + ip_.to_string() + ":" + std::to_string(port_));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        socket_->close();
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    void send(std::string const &msg_str)
    {
        socket_->send(msg_str.data(), msg_str.size(), ZMQ_NOBLOCK);
    }

    void send(const void *data_ptr, size_t data_size)
    {
        socket_->send(data_ptr, data_size, ZMQ_NOBLOCK);
    }

    bool receive(std::stringstream &incoming_msg)
    {
        if (zmq::message_t msg; socket_->recv(&msg, ZMQ_NOBLOCK) && msg.size() > 0)
        {
            incoming_msg.str("");
            incoming_msg.write(static_cast<char *>(msg.data()), msg.size());
            return true;
        }
        return false;
    }
};

class Server
{
private:
    zmq::socket_t *socket_;
    TransportProtocol protocol_;
    Port port_;

public:
    Server(TransportProtocol protocol, const Port &port) : socket_(nullptr), protocol_(protocol), port_(port)
    {
        socket_ = Context::instance().create_socket(ZMQ_ROUTER);
        if (socket_)
        {
            std::string address = (protocol_ == TCP ? std::string("tcp") : std::string("udp")).append("://") + "*:" + std::to_string(port_);
            socket_->bind(address);
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }

    ~Server()
    {
        //socket_->unbind((protocol_ == TCP ? std::string("tcp") : std::string("udp")).append("://") + "*:" + std::to_string(port_));
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        socket_->close();
    }

    void send(std::string const &reciever_identity, std::string const &mes_str)
    {
        socket_->send(reciever_identity.data(), reciever_identity.size(), ZMQ_SNDMORE | ZMQ_NOBLOCK);
        socket_->send(mes_str.data(), mes_str.size(), ZMQ_NOBLOCK);
    }

    void send(const std::string &reciever_identity, const void *data_ptr, size_t data_size)
    {
        socket_->send(reciever_identity.data(), reciever_identity.size(), ZMQ_SNDMORE | ZMQ_NOBLOCK);
        socket_->send(data_ptr, data_size, ZMQ_NOBLOCK);
    }

    bool receive(std::string &reciever_identity, std::stringstream &incoming_msg)
    {
        if (zmq::message_t identity; socket_->recv(&identity, ZMQ_RCVMORE | ZMQ_NOBLOCK) && identity.size() > 0)
        {
            reciever_identity = std::string(static_cast<char *>(identity.data()), identity.size());
            if (zmq::message_t msg; socket_->recv(&msg, ZMQ_NOBLOCK) && msg.size() > 0)
            {
                incoming_msg.str("");
                incoming_msg.write(static_cast<char *>(msg.data()), msg.size());
                return true;
            }
        }
        return false;
    }
};
} // namespace net

#endif // TARJA_NETWORKING_HPP
