#ifndef NETWORK_MESSAGE_HPP
#define NETWORK_MESSAGE_HPP

#include <functional>
#include <string>
#include <type_traits>
#include "zmq.hpp"
#include "networking.hpp"
#include "time_utility.hpp"

namespace net
{

enum class NetworkCommand : std::uint8_t
{
    PUT_COMMAND,
    GET_COMMAND,
    DELETE_COMMAND
};

enum class NetworkResponse : std::uint8_t
{
    KEY_ADDED,
    KEY_VALUE,
    KEY_DELETED,
    KEY_DOES_NOT_EXIST,
    KEY_ALREADY_EXIST
};

class ISerializable
{
protected:
    virtual std::ostream &write(std::ostream &stream) const = 0;
    virtual std::istream &read(std::istream &stream) = 0;
};

class SendMessage
{
    public:
    TimeStamp time_stamp;
    std::uint16_t id;
    std::uint8_t data_type;

    SendMessage() {}
    SendMessage(std::uint16_t msg_id, NetworkCommand command)
        : time_stamp(TimeStamp()),
          id(msg_id),
          data_type(static_cast<std::underlying_type<NetworkCommand>::type>(command)) {}

protected:
    std::ostream &write(std::ostream &stream) const
    {
        if (stream.write((const char *)&time_stamp.value, sizeof(std::uint64_t)))
            if (stream.write((const char *)&id, sizeof(std::uint16_t)))
                return stream.write((const char *)&data_type, sizeof(std::uint8_t));
        return stream;
    }

    std::istream &read(std::istream &stream)
    {
        if (stream.read((char *)&time_stamp.value, sizeof(std::uint64_t)))
            if (stream.read((char *)&id, sizeof(std::uint16_t)))
                return stream.read((char *)&data_type, sizeof(std::uint8_t));
        return stream;
    }

public:
    friend std::ostream &operator<<(std::ostream &stream, const SendMessage& e) {
        return e.write(stream);
    }

    friend std::istream &operator>>(std::istream &stream, SendMessage& e) {
        return e.read(stream);
    }
};

class ResponseMessage
{
    public:
    TimeStamp time_stamp;
    std::uint16_t id;
    std::uint8_t data_type;

    ResponseMessage() {}
    ResponseMessage(std::uint16_t msg_id, NetworkResponse response)
        : time_stamp(TimeStamp()),
          id(msg_id),
          data_type(static_cast<std::underlying_type<NetworkResponse>::type>(response)) {}

protected:
    std::ostream &write(std::ostream &stream) const
    {
        if (stream.write((const char *)&time_stamp.value, sizeof(std::uint64_t)))
            if (stream.write((const char *)&id, sizeof(std::uint16_t)))
                return stream.write((const char *)&data_type, sizeof(std::uint8_t));
        return stream;
    }

    std::istream &read(std::istream &stream)
    {
        if (stream.read((char *)&time_stamp.value, sizeof(std::uint64_t)))
            if (stream.read((char *)&id, sizeof(std::uint16_t)))
                return stream.read((char *)&data_type, sizeof(std::uint8_t));
        return stream;
    }

public:
    friend std::ostream &operator<<(std::ostream &stream, const ResponseMessage &e) {
        return e.write(stream);
    }

    friend std::istream &operator>>(std::istream &stream, ResponseMessage &e) {
        return e.read(stream);
    }
};

struct BaseCommandData
{
protected:
    NetworkCommand type_;

public:
    BaseCommandData(NetworkCommand type) : type_(type) {}
    virtual ~BaseCommandData() {}
    NetworkCommand type() const { return type_; }
};

struct BaseResponseData
{
protected:
    NetworkResponse type_;

public:
    BaseResponseData(NetworkResponse type) : type_(type) {}
    virtual ~BaseResponseData() {}
    NetworkResponse type() const { return type_; }
};

struct PutCommand : public BaseCommandData, public ISerializable
{
    std::string key;
    std::string value;

    PutCommand() : BaseCommandData(NetworkCommand::PUT_COMMAND) {}
    PutCommand(const std::string &key, const std::string &value) : BaseCommandData(NetworkCommand::PUT_COMMAND), key(key), value(value) {}

protected:
    std::ostream &write(std::ostream &stream) const
    {
        std::uint16_t len = (std::uint16_t)key.size();
        if (stream.write((const char *)&len, sizeof(std::uint16_t)))
            if (stream.write((const char *)&key[0], len))
            {
                len = (std::uint16_t)value.size();
                if (stream.write((const char *)&len, sizeof(std::uint16_t)))
                    return stream.write((const char *)&value[0], len);
            }
        return stream;
    }

    std::istream &read(std::istream &stream)
    {
        std::uint16_t len;
        if (stream.read((char *)&len, sizeof(std::uint16_t)))
        {
            key.resize(len);
            if (stream.read((char *)&key[0], len))
            {
                if (stream.read((char *)&len, sizeof(std::uint16_t)))
                {
                    value.resize(len);
                    return stream.read((char *)&value[0], len);
                }
            }
        }
        return stream;
    }

public:
    friend std::ostream &operator<<(std::ostream &stream, const PutCommand &e)
    {
        return e.write(stream);
    }

    friend std::istream &operator>>(std::istream &stream, PutCommand &e)
    {
        return e.read(stream);
    }
};

struct GetCommand : public BaseCommandData, public ISerializable
{
    std::string key;

    GetCommand() : BaseCommandData(NetworkCommand::GET_COMMAND) {}
    GetCommand(const std::string &key) : BaseCommandData(NetworkCommand::GET_COMMAND), key(key) {}

protected:
    std::ostream &write(std::ostream &stream) const
    {
        std::uint16_t key_len = (std::uint16_t)key.size();
        if (stream.write((const char *)&key_len, sizeof(std::uint16_t)))
            return (stream.write((const char *)&key[0], key_len));
        return stream;
    }

    std::istream &read(std::istream &stream)
    {
        std::uint16_t len;
        if (stream.read((char *)&len, sizeof(std::uint16_t)))
        {
            key.resize(len);
            return stream.read((char *)&key[0], len);
        }
        return stream;
    }

public:
    friend std::ostream &operator<<(std::ostream &stream, const GetCommand &e)
    {
        return e.write(stream);
    }

    friend std::istream &operator>>(std::istream &stream, GetCommand &e)
    {
        return e.read(stream);
    }
};

struct DeleteCommand : public BaseCommandData, public ISerializable
{
    std::string key;

    DeleteCommand() : BaseCommandData(NetworkCommand::DELETE_COMMAND) {}
    DeleteCommand(const std::string &key) : BaseCommandData(NetworkCommand::DELETE_COMMAND), key(key) {}

protected:
    std::ostream &write(std::ostream &stream) const
    {
        std::uint16_t len = (std::uint16_t)key.size();
        if (stream.write((const char *)&len, sizeof(std::uint16_t)))
            return (stream.write((const char *)&key[0], len));
        return stream;
    }

    std::istream &read(std::istream &stream)
    {
        std::uint16_t len;
        if (stream.read((char *)&len, sizeof(std::uint16_t)))
        {
            key.resize(len);
            return stream.read((char *)&key[0], len);
        }
        return stream;
    }

public:
    friend std::ostream &operator<<(std::ostream &stream, const DeleteCommand &e)
    {
        return e.write(stream);
    }

    friend std::istream &operator>>(std::istream &stream, DeleteCommand &e)
    {
        return e.read(stream);
    }
};

struct KeyAddedResponseData : public BaseResponseData, public ISerializable
{
    std::string key;
    std::string value;

    KeyAddedResponseData() : BaseResponseData(NetworkResponse::KEY_ADDED) {}
    KeyAddedResponseData(const std::string &key, const std::string &value) : BaseResponseData(NetworkResponse::KEY_ADDED), key(key), value(value) {}

protected:
    std::ostream &write(std::ostream &stream) const
    {
         std::uint16_t len = (std::uint16_t)key.size();
        if (stream.write((const char *)&len, sizeof(std::uint16_t)))
            if (stream.write((const char *)&key[0], len))
            {
                len = (std::uint16_t)value.size();
                if (stream.write((const char *)&len, sizeof(std::uint16_t)))
                    return stream.write((const char *)&value[0], len);
            }
        return stream;
    }

    std::istream &read(std::istream &stream)
    {
        std::uint16_t len;
        if (stream.read((char *)&len, sizeof(std::uint16_t)))
        {
            key.resize(len);
            if (stream.read((char *)&key[0], len))
            {
                if (stream.read((char *)&len, sizeof(std::uint16_t)))
                {
                    value.resize(len);
                    return stream.read((char *)&value[0], len);
                }
            }
        }
        return stream;
    }
public:
    friend std::ostream &operator<<(std::ostream &stream, const KeyAddedResponseData &e)
    {
        return e.write(stream);
    }

    friend std::istream &operator>>(std::istream &stream, KeyAddedResponseData &e)
    {
        return e.read(stream);
    }
};

struct KeyValueResponseData : public BaseResponseData, public ISerializable
{
    std::string key;
    std::string value;

    KeyValueResponseData() : BaseResponseData(NetworkResponse::KEY_VALUE) {}
    KeyValueResponseData(const std::string &key, const std::string &value) : BaseResponseData(NetworkResponse::KEY_VALUE), key(key), value(value) {}

protected:
    std::ostream &write(std::ostream &stream) const
    {
         std::uint16_t len = (std::uint16_t)key.size();
        if (stream.write((const char *)&len, sizeof(std::uint16_t)))
            if (stream.write((const char *)&key[0], len))
            {
                len = (std::uint16_t)value.size();
                if (stream.write((const char *)&len, sizeof(std::uint16_t)))
                    return stream.write((const char *)&value[0], len);
            }
        return stream;
    }

    std::istream &read(std::istream &stream)
    {
        std::uint16_t len;
        if (stream.read((char *)&len, sizeof(std::uint16_t)))
        {
            key.resize(len);
            if (stream.read((char *)&key[0], len))
            {
                if (stream.read((char *)&len, sizeof(std::uint16_t)))
                {
                    value.resize(len);
                    return stream.read((char *)&value[0], len);
                }
            }
        }
        return stream;
    }

public:
    friend std::ostream &operator<<(std::ostream &stream, const KeyValueResponseData &e)
    {
        return e.write(stream);
    }

    friend std::istream &operator>>(std::istream &stream, KeyValueResponseData &e)
    {
        return e.read(stream);
    }
};

struct KeyDeletedResponseData : public BaseResponseData, public ISerializable
{
    std::string key;

    KeyDeletedResponseData() : BaseResponseData(NetworkResponse::KEY_DELETED) {}
    KeyDeletedResponseData(const std::string &key) : BaseResponseData(NetworkResponse::KEY_DELETED), key(key) {}

protected:
    std::ostream &write(std::ostream &stream) const
    {
        std::uint16_t len = (std::uint16_t)key.size();
        if (stream.write((const char *)&len, sizeof(std::uint16_t)))
            return (stream.write((const char *)&key[0], len));
        return stream;
    }

    std::istream &read(std::istream &stream)
    {
        std::uint16_t len;
        if (stream.read((char *)&len, sizeof(std::uint16_t)))
        {
            key.resize(len);
            return stream.read((char *)&key[0], len);
        }
        return stream;
    }

public:
    friend std::ostream &operator<<(std::ostream &stream, const KeyDeletedResponseData &e)
    {
        return e.write(stream);
    }

    friend std::istream &operator>>(std::istream &stream, KeyDeletedResponseData &e)
    {
        return e.read(stream);
    }
};

struct KeyNotExistResponseData : public BaseResponseData, public ISerializable
{
    std::string key;

    KeyNotExistResponseData() : BaseResponseData(NetworkResponse::KEY_DOES_NOT_EXIST) {}
    KeyNotExistResponseData(const std::string &key) : BaseResponseData(NetworkResponse::KEY_DOES_NOT_EXIST), key(key) {}

protected:
    std::ostream &write(std::ostream &stream) const
    {
        std::uint16_t len = (std::uint16_t)key.size();
        if (stream.write((const char *)&len, sizeof(std::uint16_t)))
            return (stream.write((const char *)&key[0], len));
        return stream;
    }

    std::istream &read(std::istream &stream)
    {
        std::uint16_t len;
        if (stream.read((char *)&len, sizeof(std::uint16_t)))
        {
            key.resize(len);
            return stream.read((char *)&key[0], len);
        }
        return stream;
    }

public:
    friend std::ostream &operator<<(std::ostream &stream, const KeyNotExistResponseData &e)
    {
        return e.write(stream);
    }

    friend std::istream &operator>>(std::istream &stream, KeyNotExistResponseData &e)
    {
        return e.read(stream);
    }
};

struct KeyAlreadyExistResponseData : public BaseResponseData, public ISerializable
{
    std::string key;

    KeyAlreadyExistResponseData() : BaseResponseData(NetworkResponse::KEY_ALREADY_EXIST) {}
    KeyAlreadyExistResponseData(const std::string &key) : BaseResponseData(NetworkResponse::KEY_ALREADY_EXIST), key(key) {}

protected:
    std::ostream &write(std::ostream &stream) const
    {
        std::uint16_t len = (std::uint16_t)key.size();
        if (stream.write((const char *)&len, sizeof(std::uint16_t)))
            return (stream.write((const char *)&key[0], len));
        return stream;
    }

    std::istream &read(std::istream &stream)
    {
        std::uint16_t len;
        if (stream.read((char *)&len, sizeof(std::uint16_t)))
        {
            key.resize(len);
            return stream.read((char *)&key[0], len);
        }
        return stream;
    }

public:
    friend std::ostream &operator<<(std::ostream &stream, const KeyAlreadyExistResponseData &e)
    {
        return e.write(stream);
    }

    friend std::istream &operator>>(std::istream &stream, KeyAlreadyExistResponseData &e)
    {
        return e.read(stream);
    }
};

} // namespace net

#endif // !NETWORK_MESSAGE_HPP
