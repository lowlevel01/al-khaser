#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>


#pragma comment(lib, "ws2_32.lib")

int main() {

    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return 1;
    }

    // create socket
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Socket creation failed.\n";
        WSACleanup();
        return 1;
    }
    
    addrinfo* result = nullptr;
    addrinfo hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    if (getaddrinfo("ip-api.com", "80", &hints, &result) != 0) {
        std::cerr << "getaddrinfo failed.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    
    if (connect(sock, result->ai_addr, static_cast<int>(result->ai_addrlen)) == SOCKET_ERROR) {
        std::cerr << "Connection failed.\n";
        freeaddrinfo(result);
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    std::string request = "GET /json/?fields=hosting HTTP/1.1\r\n";
    request += "Host: ip-api.com\r\n";
    request += "Connection: close\r\n\r\n";

    if (send(sock, request.c_str(), static_cast<int>(request.length()), 0) == SOCKET_ERROR) {
        std::cerr << "Send failed.\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    // Receive response
    const int bufferSize = 512;
    char buffer[bufferSize];
    int bytesReceived;
    std::string response;

    do {
        bytesReceived = recv(sock, buffer, bufferSize - 1, 0);
        if (bytesReceived > 0) {
            buffer[bytesReceived] = '\0';
            response += buffer;
        }
    } while (bytesReceived > 0);

    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Receive failed.\n";
    }
    else {
        if (response.find("true") != std::string::npos ){
            std::cout << "Sandbox Detected" << std::endl;
        }
        else {
            std::cout << "Safe" << std::endl;
        }
    }
	
    // Clean up
    closesocket(sock);
    WSACleanup();

    return 0;
}
