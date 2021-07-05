#pragma once

#include "Object.h"


class Sector_Manager;
class Monster_Base;
class Send_Manager;
class Object_Manager;
class Timer_Manager;

class Player : public Object
{
public:
	Player_Expand_Overlapped	player_exover;

private:
	SOCKET	socket;

	CoolTime time__recv_move;
	CoolTime time__recv_normal_attack;
	CoolTime time__recv_spell;

	Near_set near_set;	

public:
	char*	packet_start;
	char*	recv_start;
		
public:
	Player() = delete;
	Player(const SOCKET&, const Type_ID&);
	Player(const Player&);

	virtual ~Player() override final;
	

private:
	friend class Object_Manager;
	friend class Send_Manager;

	virtual bool is_near(Object* const) override final;
	bool Update_position();
	bool TEST_Update_position();

public:
	bool Is_Time_Recv_Move_packet();
	bool Is_Time_Recv_Move_Target_packet();
	bool Is_Time_Recv_Normal_Attack();
	bool Is_Time_Recv_SPELL();
	bool TEST_Move_packet();
	
	void Update_Near_set_Sight_in(std::vector<Object*>&, std::vector<LF::shared_ptr<Player>>&);
	void Get_Objects__Update_Near_set(std::vector<std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>>&, std::vector<LF::shared_ptr<Player>>&);

public:
	SOCKET	Get_Socket() const;

	bool try_insert__near_set(const Type_ID&);
	
	void Insert_Near_set(const Type_ID&);

	void WSAReceive();
	void WSAReceive(CHAR*, ULONG);

	virtual bool Is_Player() override final;
	virtual Type_POS Get_Size() const override final;
	RESULT_PLAYER_MOVE Move();
	RESULT_PLAYER_MOVE TEST_Move(std::vector<std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>>&, std::vector<LF::shared_ptr<Player>>&);
	virtual std::chrono::milliseconds Get_respwan_time() const override final;

	Type_Damage Normal_Attack(Object* const);
	Type_Damage Spell(Object* const);

	void Respwan();
};