#pragma warning(disable:4996)

#include <iostream>
#include <atomic>
#include <array>
#include <mutex>
#include <thread>

#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

#include "..\..\protocol.h"

constexpr size_t MAX_BUFFER{ 1024 };
constexpr size_t MIN_BUFFER{ 256 };
constexpr size_t MAX_PACKET_SIZE{ 256 };

constexpr size_t TEST_NUMBER{ 900 };

enum EXOVER_TYPE { EX_RECV, EX_SEND };

struct EXOVER {
	WSAOVERLAPPED over;
	EXOVER_TYPE	  type;

	WSABUF	wsabuf;
	char	IObuf[MAX_BUFFER];
};

class Dummy_Player
{
public:
	SOCKET	sock;
	EXOVER	exover;
	unsigned char packet_buf[MAX_PACKET_SIZE];
	char*	packet_start;
	char*	recv_start;

	bool	connect;

	Dummy_Player() {
		packet_start = exover.IObuf;
		recv_start = exover.IObuf;
		sock = NULL;
		connect = false;
	}

	void WSAReceive(CHAR* start_ptr, ULONG size)
	{
		ZeroMemory(&exover.over, sizeof(WSAOVERLAPPED));
		exover.type = EX_RECV;
		exover.wsabuf.buf = start_ptr;
		exover.wsabuf.len = size;

		DWORD flag = 0;
		WSARecv(sock, &exover.wsabuf, 1, NULL, &flag, &exover.over, NULL);
	}
};

int try_connect_num;
int connecting_dummy_num;
std::mutex connecting_dummy_num_mutex;
HANDLE iocp;
std::array<Dummy_Player,TEST_NUMBER> dummy_players;

void SendPacket(int index, void *packet)
{
	int packet_size = reinterpret_cast<unsigned char *>(packet)[0];
	int packet_type = reinterpret_cast<unsigned char *>(packet)[1];

	EXOVER *exover = new EXOVER;
	exover->type = EX_SEND;

	memcpy(exover->IObuf, packet, packet_size);
	ZeroMemory(&exover->over, sizeof(exover->over));
	exover->wsabuf.buf = reinterpret_cast<CHAR *>(exover->IObuf);
	exover->wsabuf.len = packet_size;

	int Result = WSASend(dummy_players[index].sock, &exover->wsabuf, 1, NULL, 0, &exover->over, NULL);
	
	if (0 != Result) {
		int err_no = WSAGetLastError();
		if (WSA_IO_PENDING != err_no)	printf("Send Error[%d] - %d \n", err_no, index);
	}
}

void Process__Packet(Dummy_Player& player)
{
	const char packet_type{ player.packet_start[1] };

	switch (packet_type)
	{
	case SCPT_LOGIN_SUCCESS: {
		connecting_dummy_num_mutex.lock();
		connecting_dummy_num++;
		connecting_dummy_num_mutex.unlock();
		player.connect = true;
	}
							 break;

	default:	break;
	}
}

void DisconnectClient(int id)
{
	closesocket(dummy_players[id].sock);
	dummy_players[id].connect = false;
	std::cout << "Client [" << id << "] Disconnected!\n" << std::endl;
}

void Try_Connect_Dummy() {
	if (TEST_NUMBER <= try_connect_num) return;
	if (dummy_players[try_connect_num].connect == true)	return;

	dummy_players[try_connect_num].sock = WSASocketW(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(dummy_players[try_connect_num].sock, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);
	if (0 != Result) {
		printf("Connect Error[%d] - %d \n", GetLastError() , try_connect_num);
	}

	ZeroMemory(&dummy_players[try_connect_num].exover, sizeof(EXOVER));
	dummy_players[try_connect_num].exover.type = EX_RECV;
	dummy_players[try_connect_num].exover.wsabuf.buf = reinterpret_cast<CHAR *>(dummy_players[try_connect_num].exover.IObuf);
	dummy_players[try_connect_num].exover.wsabuf.len = sizeof(dummy_players[try_connect_num].exover.IObuf);

	DWORD recv_flag = 0;
	CreateIoCompletionPort(reinterpret_cast<HANDLE>(dummy_players[try_connect_num].sock), iocp, try_connect_num, 0);
	
	int ret = WSARecv(dummy_players[try_connect_num].sock, &dummy_players[try_connect_num].exover.wsabuf, 1, NULL, &recv_flag, &dummy_players[try_connect_num].exover.over, NULL);
	if (SOCKET_ERROR == ret) {
		int err_no = WSAGetLastError();
		if (err_no != WSA_IO_PENDING)	printf("Recv Error[%d] - %d \n", err_no, try_connect_num);
	}

	dummy_players[try_connect_num].connect = true;
	printf("connecting : %d \n", try_connect_num);
	try_connect_num++;
}

void Thread_Func__Recv() {
	printf("Working Recv thread \n");
	while (true) {
		DWORD byte;
		ULONGLONG key64;
		PULONG_PTR keyptr{ (PULONG_PTR)&key64 };
		EXOVER *over;
		BOOL ret = GetQueuedCompletionStatus(iocp, &byte, keyptr, reinterpret_cast<LPWSAOVERLAPPED *>(&over), INFINITE);

		int id = static_cast<int>(key64);

		if (FALSE == ret || 0 == byte) {
			DisconnectClient(id);
			continue;
		}

		if (EX_RECV == over->type) {

			Dummy_Player& player{ dummy_players[id] };

			unsigned char packet_size{ (unsigned char)player.packet_start[0] };
			char* next_recv_ptr{ player.recv_start + byte };

			while (packet_size <= next_recv_ptr - player.packet_start) {

				Process__Packet(player);

				player.packet_start += packet_size;

				if (player.packet_start < next_recv_ptr)
					packet_size = player.packet_start[0];
				else
					break;
			}

			int left_data{ (int)(next_recv_ptr - player.packet_start) };

			if ((MAX_BUFFER - (next_recv_ptr - dummy_players[id].exover.IObuf) < MIN_BUFFER)) {
				memcpy(player.exover.IObuf, player.packet_start, left_data);
				player.packet_start = player.exover.IObuf;
				next_recv_ptr = player.packet_start + left_data;
			}

			player.recv_start = next_recv_ptr;

			player.WSAReceive(reinterpret_cast<CHAR*>(next_recv_ptr), (ULONG)(MAX_BUFFER - (next_recv_ptr - player.exover.IObuf)));
		}

		else if (EX_SEND == over->type) {
			delete over;
		}

		else {
			std::cout << "Unknown GQCS event!\n";
		}
	}
}

void Thread_Func__Send() {
	try_connect_num = 0;

	while (true) {
		Sleep(500);	// CS_TIME_SEND_PLAYER_MOVE

		Try_Connect_Dummy();

		for (int index = 0; index < try_connect_num; ++index) {
			if (false == dummy_players[index].connect)	continue;

			TEST_MOVE_TARGET my_packet;
			my_packet.size = sizeof(my_packet);
			my_packet.type = T_MOVE_TARGET;

			SendPacket(index, &my_packet);
		}
	}
}

void main() {
	connecting_dummy_num = 0;

	WSADATA				wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, NULL, 0);

	std::thread thread_recv{ Thread_Func__Recv };

	Thread_Func__Send();

	thread_recv.join();
}