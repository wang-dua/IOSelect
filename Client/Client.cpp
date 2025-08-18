#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winSock2.h>
#include <WS2tcpip.h>
#include <cstdio>
#include <vector>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")


/**
 * 
 * @return 客户端需要只需要一个通信套接字
 */
int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0); //IPV4, TCP

	sockaddr_in srv{};
	srv.sin_family = AF_INET;
	srv.sin_port = htons(8080);
	srv.sin_addr.s_addr = inet_addr("192.168.0.178");

	if (connect(sock, (sockaddr*) &srv, sizeof(srv)) == SOCKET_ERROR)
	{
		std::cout << "Connect failed\n";
		return -1;
	}
	std::cout << "Connected to server. Type messages.\n";

	char buf[512];
	while (true)
	{
		std::string msg;
		std::getline(std::cin, msg);
		if (msg == "quit") break;

		send(sock, msg.c_str(), msg.size(), 0);
		//如果不发送消息, send会阻塞, 导致不能recv服务端的消息

		int n = recv(sock, buf, sizeof(buf) - 1, 0);
		if (n > 0)
		{
			buf[n] = '\0';
			std::cout << "Server respond: " << buf << "\n";
		}
		else
		{
			break;
		}
	}
	closesocket(sock);
	WSACleanup();

}