#pragma once
#include "Object.h"

class Login_Manager;
class Object_Manager;
class Sector_Manager;
class Terrain_Manager;
class Timer_Manager;
class Send_Manager;

class Server
{
private:
	WSADATA				wsadata;
	SOCKET				listensocket, acceptsocket;
	SOCKADDR_IN			serveraddr;
	Player_Expand_Overlapped	accept_exover;

	HANDLE iocp;

	Login_Manager*		LgnMgr;
	Object_Manager*		ObjMgr;
	Sector_Manager*		SctMgr;
	Terrain_Manager*	TrnMgr;
	Timer_Manager*		TmrMgr;
	Send_Manager*		SndMgr;

	std::vector<std::thread> process_threads;

private:
	using Duration_Time = const std::chrono::duration<float, std::milli>&;

public:
	Server();
	~Server();

private:
	void error_display(const char* const, int);

	void Do_Connect();
	void Do_Acceptex();
	void Do_Disconnect(Player* const);
	void Do_Assemble_packet(Player* const, const DWORD&);

	void Create_thread__Server_Process();
	void Create_thread__Timer_Process();
	void Wait_thread__Process();

	void Process();
	void Process_Accept();
	void Process_Receive(const Type_ID&, DWORD);
	void Process_Send(const Expand_Overlapped* const);
	void Process__Packet(Player* const);
	void Process__Packet_Move(Player* const);
	void Process__Packet_Move_Target(Player* const);
	void Process__Packet_TEST_Move(Player* const);
	void Process__Packet_Normal_Attack(Player* const);
	void Process__Packet_Spell(Player* const);
	void Process_Monster_Move_Start(const Type_ID&);
	void Process_Monster_Move(const Type_ID&);
	void Process_Player_Respwan(const Type_ID&);
	void Process_Monster_Respwan(const Type_ID&);

public:
	void Running();

};

