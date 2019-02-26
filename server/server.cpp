// server.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#pragma comment(lib,"ws2_32.lib")


BOOL read_memory(const Packet& packet)
{
	char *buf = new char[packet.data.read_memory.size];

	HANDLE hProcess;

	ReadProcessMemory(hProcess, (LPCVOID)packet.data.read_memory.address,buf, packet.data.read_memory.size,NULL);

	memcpy(packet.data.completed.result.buffer, buf, packet.data.read_memory.size);

	delete[]buf;
}


uint64_t handle_incoming_packet(const Packet& packet)
{
	switch (packet.header.type)
	{
	case PacketType::packet_read_memory:
		read_memory(packet);
		break;
	default:
		break;
	}

	return uint64_t();
}

// Send completion packet.
bool complete_request(const SOCKET client_connection, const uint64_t result)
{
	Packet packet{ };

	packet.header.packet_pwd = packet_pwd;
	packet.header.type = PacketType::packet_completed;
	//packet.data.completed.result = result;

	return send(client_connection, (char*)&packet, sizeof(packet), 0) != SOCKET_ERROR;
}

static void NTAPI connection_thread(void* connection_socket)
{
	const auto client_connection = SOCKET(ULONG_PTR(connection_socket));
	printf("新连接。。。\n");
	Packet packet{ };
	while (true)
	{
		const auto result = recv(client_connection, (char *)&packet, sizeof(packet), 0);
		if (result <= 0)
			break;
		if (result < sizeof(PacketHeader))
			continue;
		if (packet.header.packet_pwd != packet_pwd)
			continue;
		if (!complete_request(client_connection, handle_incoming_packet(packet)))
			break;
	}
	printf("Connection closed.\n");
	closesocket(client_connection);
}

int main()
{
	WSADATA wsaData;
	SOCKET sockServer;
	SOCKADDR_IN addrServer;
	SOCKET sockClient;
	SOCKADDR_IN addrClient;

	WSAStartup(MAKEWORD(2, 2), &wsaData);//socket库绑定

	sockServer = socket(AF_INET, SOCK_STREAM, 0);//创建套接字的描述符

	// 地址/端口 相关设置
	addrServer.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//INADDR_ANY表示任何IP
	addrServer.sin_family = AF_INET;//域设置
	addrServer.sin_port = htons(6000);//绑定端口6000

	bind(sockServer, (SOCKADDR*)&addrServer, sizeof(SOCKADDR));//绑定sokect

	int listen_socket  = listen(sockServer, 5);//创建监听套接字 （服务端用）5为等待连接数目

	int len;

	while (true)
	{
		sockClient = accept(sockServer, (SOCKADDR*)&addrClient, &len);

		CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)connection_thread, (void*)sockClient,0,NULL));
	}
	closesocket(listen_socket);
}


