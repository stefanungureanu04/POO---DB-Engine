#include <iostream>
#include "pch.h"
#include "SocketLib.h"

SocketException::SocketException(const std::string& message, const int& code)
{
	m_Message = message;
	m_Code = code;
}

SocketException::~SocketException() {};

SocketException::SocketException(const SocketException& other)
{
    m_Message = other.m_Message;
    m_Code = other.m_Code;
}

const std::string& SocketException::getMessage() const
{
	return m_Message;
}

const int& SocketException::getCode() const
{
	return m_Code;
}

void SocketException::print() const
{
	std::cout << "Exception::SocketException: " + m_Message + " (error #" + std::to_string(m_Code) + ")";
}

std::ostream& operator<<(std::ostream& out, const SocketException& e)
{
	std::string stringify{};

	stringify += "Exception::SocketException: " + e.getMessage() + " (error #" + std::to_string(e.getCode()) + ")";

	out << stringify;

	return out;
}

std::ostream& operator<<(std::ostream& out, const SocketException* e)
{
	std::string stringify{};

	stringify += "Exception::SocketException: " + e->getMessage() + " (error #" + std::to_string(e->getCode()) + ")";

	out << stringify;

	return out;
}


#include <winsock2.h>
#include <ws2tcpip.h>

#include <cstring>   

#pragma comment(lib, "ws2_32.lib") 

Socket::Socket(Protocol protocol)
{
    this->protocol = protocol;
    this->sock = INVALID_SOCKET;
    this->isServer = false;

    this->initializeWinsock();

    int type = (protocol == Protocol::TCP) ? SOCK_STREAM : SOCK_DGRAM;

    this->sock = socket(AF_INET, type, IPPROTO_IP);

    if (this->sock == INVALID_SOCKET) {
        throw SocketException("Failed to create socket", WSAGetLastError());
    }
}

Socket::~Socket()
{
    this->closeSocket();
    this->cleanupWinsock();
}

const std::string Socket::type() const
{
    if (this->isServer == true) {
        return "SERVER";
    }
    return "CLIENT";
}

void Socket::initializeWinsock() const
{
    WSADATA wsaData;

    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0) {
        throw SocketException("WSAStartup failed", result);
    }
}

void Socket::cleanupWinsock() const
{
    WSACleanup();
}

const bool Socket::connectToServer(const std::string& ip_address, const int& port)
{
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, ip_address.c_str(), &serverAddr.sin_addr);

    return connect(this->sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) != SOCKET_ERROR;
}

void Socket::bindAndListen(const int& port)
{
    this->isServer = true;

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(this->sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        throw SocketException("Bind failed", WSAGetLastError());
    }

    if (this->protocol == Protocol::TCP && listen(sock, SOMAXCONN) == SOCKET_ERROR) {
        throw SocketException("Listen failed", WSAGetLastError());
    }
}

Socket* Socket::acceptConnection()
{
    SOCKET clientSock = accept(this->sock, nullptr, nullptr);

    if (clientSock == INVALID_SOCKET) {
        throw SocketException("Accept failed", WSAGetLastError());
    }

    Socket* client = new Socket(Protocol::TCP);
    client->sock = clientSock;

    return client;
}

bool Socket::sendData(const std::string& data) const
{
    int sent = send(this->sock, data.c_str(), static_cast<int>(data.size()), 0);
    return sent != SOCKET_ERROR;
}

const std::string Socket::receiveData(size_t bufferSize) const
{
    char* buffer = new char[bufferSize];
    memset(buffer, 0, bufferSize);
    int received = recv(this->sock, buffer, static_cast<int>(bufferSize) - 1, 0);

    std::string result;

    if (received > 0) {
        result.assign(buffer, received);
    }

    delete[] buffer;
    return result;
}

const std::string Socket::getIpAddress() const
{
    sockaddr_in addr;
    int addrLen = sizeof(addr);

    if (getsockname(sock, (sockaddr*)&addr, &addrLen) == SOCKET_ERROR) {
        throw SocketException("Failed to get IP address", 110);
    }

    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &addr.sin_addr, ipStr, sizeof(ipStr));

    return std::string(ipStr);
}

void Socket::closeSocket()
{
    if (this->sock != INVALID_SOCKET) {
        closesocket(this->sock);
        sock = INVALID_SOCKET;
    }
}