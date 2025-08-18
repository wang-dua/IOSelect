#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winSock2.h>
#include <WS2tcpip.h>
#include <cstdio>
#include <vector>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2), &wsaData);

	//listen socket, IPV4, TCP
	SOCKET listenSock = socket(AF_INET, SOCK_STREAM, 0);

	//listenSock 绑定本地IP Port
	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY; //0000不需要htons, 监听本机所有IP
	addr.sin_port = htons(8080);
	int b = bind(listenSock, (sockaddr*) &addr, sizeof(addr));
	listen(listenSock, SOMAXCONN); 

	std::cout << "[Server] Listening on 8080...\n";

	//vector 存储已连接的客户端Socket, accept返回的Socket
	std::vector<SOCKET> clients;

	while (true)
	{
		fd_set read_fds; //可读fd
		FD_ZERO(&read_fds); //标志位置零
		FD_SET(listenSock, &read_fds); //将listenSock 添加到可读fd, 读取listenSock是否有新连接到达

		for (SOCKET c : clients)
		{
			FD_SET(c, &read_fds);
		} //将已连接的客户端所有socket 都添加到read_fds

		 //socket是默认可写的, select只是监控作用, 建立与多个客户端的连接
		//	没有write_fds 服务端也可以向客户端发送信息
		int ret = select(0, &read_fds, nullptr, nullptr, nullptr);
		if (ret == SOCKET_ERROR) break;

		//检测新客户端的连接 (listenSock 可读代表有新连接), 将新连接的客户端socket添加到clients
		if (FD_ISSET(listenSock, &read_fds))
		{
			SOCKET client = accept(listenSock, nullptr, nullptr);
			clients.push_back(client);
			std::cout << "New Client connected.\n";
		}

		//检查clients: 已连接的客户端是否有信息可读
		for (size_t i = 0; i < clients.size();)
		{
			SOCKET c = clients[i];
			if (FD_ISSET(c, &read_fds)) //客户端可读
			{
				/*
				 * 服务端接受消息, 并回显
				 * 当客户端使用close关闭连接时, 也可读, 消息长度 n == 0, 服务器可以关闭该连接并移除client
				 */
				char buf[512];
				int n = recv(c, buf, sizeof(buf) - 1, 0);
				if (n > 0)
				{
					buf[n] = 0;
					std::cout << buf << "\n";
					send(c, buf, n, 0);
				}
				else
				{
					closesocket(c);
					clients.erase(clients.begin() + i);
					continue;
				}
			}
			i++; //客户端没有发送消息, 或是处理完消息, 到下一个客户端
		}

	}

	closesocket(listenSock);
	WSACleanup();
}