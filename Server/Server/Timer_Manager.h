#pragma once

#include <queue>
#include <tbb/concurrent_priority_queue.h>

struct EVENT;
class Object_Manager;
class Send_Manager;
class Object;
class Player;
class Monster_Base;

class Timer_Manager
{
private:
	tbb::concurrent_priority_queue<EVENT> concurrent_queue;

private:
	friend class Send_Manager;

public:
	static Timer_Manager* TmrMgr;
	static Object_Manager* ObjMgr;
	static HANDLE* iocp;

	Timer_Manager() = delete;
	Timer_Manager(HANDLE* const, Object_Manager* const);

public:
	static Timer_Manager* Create_Time_Manager(HANDLE* const, Object_Manager* const);
	
private:
	void Process_Monster_Move(void* const);
	void Process_Monster_Move_Start(void* const);
	void Process_Respwan(void* const);

public:
	void PQCS_Monster_Awake(const Type_ID&);

public:
	void Process();

	EVENT Pop(std::priority_queue<EVENT>&);


	void Push_Event_Monster_Move(const Monster_Base* const);
	void Push_Event_Monster_Move_Start(const Monster_Base* const);
};
