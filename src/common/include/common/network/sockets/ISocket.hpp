/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ISocket
*/

#ifndef ISOCKET_HPP_
#define ISOCKET_HPP_

#include <cstdint>
#include "../../common/protocol/Packet.hpp"


class ISocket {
    public:
        ISocket();
        ~ISocket();

        virtual void bind(uint16_t port) = 0;

        virtual bool send(const common::protocol::Packet& packet) = 0;

        virtual bool receive(common::protocol::Packet& packet) = 0;

    protected:
    private:
};

#endif /* !ISOCKET_HPP_ */
