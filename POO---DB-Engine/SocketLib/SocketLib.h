#pragma once

#ifdef SOCKETLIBRARY_EXPORTS
    #define SOCKETLIB_API __declspec(dllexport)
#else
    #define SOCKETLIB_API __declspec(dllimport)
#endif

#include <string>
#include <ostream>

class SOCKETLIB_API SocketException {
private:
	std::string m_Message;
	int m_Code;

public:
	SocketException(const std::string& message, const int& code);
    SocketException(const SocketException& other);
    virtual ~SocketException();
	const std::string& getMessage() const;
	const int& getCode() const;
	void print() const;
	friend std::ostream& operator << (std::ostream& out, const SocketException& e);
	friend std::ostream& operator << (std::ostream& out, const SocketException* e);
};

#include <WinSock2.h>

class SOCKETLIB_API Socket {
public:
    enum class Protocol { TCP, UDP };

private:
    SOCKET sock;
    Protocol protocol;
    bool isServer;

public:
    Socket(Protocol protocol = Protocol::TCP);
    virtual ~Socket();
    const std::string type() const;
    const bool connectToServer(const std::string& ip_address, const int& port);
    void bindAndListen(const int& port = 12345);
    Socket* acceptConnection();
    bool sendData(const std::string& data) const;
    const std::string receiveData(size_t bufferSize = 1024) const;
    const std::string getIpAddress() const;
    void closeSocket();

private:
    void initializeWinsock() const;
    void cleanupWinsock() const;
};
