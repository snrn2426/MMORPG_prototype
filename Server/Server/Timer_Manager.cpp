#include "stdafx.h"
#include "Timer_Manager.h"
#include "Event.h"
#include "Object_Manager.h"
#include "Object.h"
#include "Player.h"
#include "Monster_Base.h"

Timer_Manager* Timer_Manager::TmrMgr = nullptr;
Object_Manager* Timer_Manager::ObjMgr = nullptr;
HANDLE* Timer_Manager::iocp = nullptr;

Timer_Manager::Timer_Manager(HANDLE* const _iocp, Object_Manager* const _ObjMgr)
{
	iocp = _iocp;
	ObjMgr = _ObjMgr;
}

Timer_Manager* Timer_Manager::Create_Time_Manager(HANDLE* const _iocp, Object_Manager* const _ObjMgr)
{
	if (TmrMgr == nullptr) {
		TmrMgr = new Timer_Manager(_iocp, _ObjMgr);
		printf("[Time_Manager] - Create\n");
	}
	return TmrMgr;
}

void Timer_Manager::Process()
{
	printf("[Timer Manager] - Running Process thread \n");

	std::priority_queue<EVENT> local_priority_queue;

	EVENT new_event;

	while (true) {
		new_event = Pop(local_priority_queue);
		switch (new_event.type) {
			
			case EV_MONSTER_MOVE:		Process_Monster_Move(new_event.detail);			break;
			case EV_MONSTER_MOVE_START:	Process_Monster_Move_Start(new_event.detail);	break;
			case EV_RESPWAN:			Process_Respwan(new_event.detail);			break;
		}
		delete new_event.detail;
	}
}


void Timer_Manager::Process_Monster_Move(void* const event_detail)
{
	Detail_Monster_Move* detail{ reinterpret_cast<Detail_Monster_Move*>(event_detail) };
	Type_ID id{ detail->id };

	Object* p_object{ ObjMgr->Get_Object(id) };

	if (nullptr == p_object)			return;

	Monster_Base* p_monster{ reinterpret_cast<Monster_Base*>(p_object) };

	ZeroMemory(&p_monster->move_exover.over, sizeof(WSAOVERLAPPED));
	p_monster->move_exover.type = EX_MONSTER_MOVE;
	PostQueuedCompletionStatus(*iocp, 1, id, &p_monster->move_exover.over);
}

void Timer_Manager::Process_Monster_Move_Start(void * const event_detail)
{
	Detail_Monster_Move* detail{ reinterpret_cast<Detail_Monster_Move*>(event_detail) };
	Type_ID id{ detail->id };

	Object* p_object{ ObjMgr->Get_Object(id) };

	if (nullptr == p_object)			return;

	Monster_Base* p_monster{ reinterpret_cast<Monster_Base*>(p_object) };

	ZeroMemory(&p_monster->move_exover.over, sizeof(WSAOVERLAPPED));
	p_monster->move_exover.type = EX_MONSTER_MOVE_START;
	PostQueuedCompletionStatus(*iocp, 1, id, &p_monster->move_exover.over);
}

void Timer_Manager::Process_Respwan(void* const event_detail)
{
	Detail_Monster_Move* detail{ reinterpret_cast<Detail_Monster_Move*>(event_detail) };
	Type_ID id{ detail->id };

	if (id < MAX_PLAYER) {
		Object* p_object{ ObjMgr->Get_Object(id) };

		if (nullptr == p_object)	return;

		Expand_Overlapped* exover = new Expand_Overlapped;

		ZeroMemory(&exover->over, sizeof(WSAOVERLAPPED));
		exover->type = EX_RESPWAN_PLAYER;
		PostQueuedCompletionStatus(*iocp, 1, id, &exover->over);
	}
	else {
		Expand_Overlapped* exover = new Expand_Overlapped;

		ZeroMemory(&exover->over, sizeof(WSAOVERLAPPED));
		exover->type = EX_RESPWAN_MONSTER;
		PostQueuedCompletionStatus(*iocp, 1, id, &exover->over);
	}
}



void Timer_Manager::PQCS_Monster_Awake(const Type_ID& id)
{
	Object* p_object{ ObjMgr->Get_Object(id) };
	if (nullptr == p_object)	return;

	Expand_Overlapped* exover = new Expand_Overlapped;

	ZeroMemory(&exover->over, sizeof(WSAOVERLAPPED));
	exover->type = EX_MONSTER_AWAKE;
	PostQueuedCompletionStatus(*iocp, 1, id, &exover->over);
}


EVENT Timer_Manager::Pop(std::priority_queue<EVENT>& local_queue)
{
	EVENT ret_ev;

	while (true) {

		if (false == local_queue.empty()) {

			const EVENT& ev = local_queue.top();

			if (ev.process_time < std::chrono::high_resolution_clock::now()) {
				ret_ev = ev;
				local_queue.pop();
				return ret_ev;
			}
		}
		
		if (true == concurrent_queue.try_pop(ret_ev)){

			if (std::chrono::high_resolution_clock::now() < ret_ev.process_time) {
				local_queue.push(ret_ev);
				continue;
			}
			
			else {
				return ret_ev;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}


void Timer_Manager::Push_Event_Monster_Move(const Monster_Base* const p_monster)
{
	using namespace std::chrono;
	Detail_Monster_Move* detail_ptr{ new Detail_Monster_Move(p_monster->id) };
	
	concurrent_queue.push(EVENT(high_resolution_clock::now() + S_TIME_UPDATE_OBJECT_MOVE, EV_MONSTER_MOVE, detail_ptr));
}

void Timer_Manager::Push_Event_Monster_Move_Start(const Monster_Base* const p_monster)
{
	using namespace std::chrono;
	Detail_Monster_Move* detail_ptr{ new Detail_Monster_Move(p_monster->id) };
	
	concurrent_queue.push(EVENT(high_resolution_clock::now() + p_monster->Get_Next_Move_Cooltime(), EV_MONSTER_MOVE_START, detail_ptr));
	
}
