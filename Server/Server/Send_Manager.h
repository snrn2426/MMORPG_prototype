#pragma once

class Sector_Manager;
class Object_Manager;
class Object;
class Player;
class Monster_Base;

class Send_Manager
{
private:
	std::array<SOCKET, MAX_PLAYER> sockets;

private:
	void Send_Packet(Player* const, void* const, bool);
	void Send_Login_failure(const SOCKET&);
	void Send_Login_success(Player* const);
	void Send_World_in_Player(Player* const, std::vector<Object*>&);

public:
	void Notify_Login_failure(const SOCKET&);
	void Notify_Login_success(Player* const, std::vector<Object*>&);
	void Notify_Player_Move(Player* const, const std::vector<std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>>&);
	void Notify_Player_Move_Modify(Player* const);
	void Notify_Player_Move_Incorrect(Player* const);
	void Notify_Player_Move_Target(Player* const, const std::vector<std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>>&);
	void Notify_Player_Normal_Attack(Player* const, Object* const, const Type_Damage&);
	void Notify_Player_Spell(Player* const, Object* const, const Type_Damage&);
	void Notify_Monster_Move(Monster_Base* const, const std::vector<std::pair<Object*, RESULT_MOVE_OBJECT_TYPE>>&);
	void Notify_Monster_Move_Target(Monster_Base* const, const std::vector<std::pair<Player*, RESULT_MOVE_OBJECT_TYPE>>&);
	void Notify_Monster_Attack(Monster_Base* const, const Type_ID&, const std::vector<std::pair<Player*, RESULT_MOVE_OBJECT_TYPE>>&, const Type_Damage&);
	
private:
	SCP_MOVE Create_Packet_Move(const Object* const);
	SCP_MOVE_TARGET Create_Packet_Move_Target(const Object* const);

	SCP_SIGHT_IN Create_Packet_Sight_in();
	SCP_SIGHT_IN Create_Packet_Sight_in(const Object* const);
	SCP_SIGHT_OUT Create_Packet_Sight_out();
	SCP_SIGHT_OUT Create_Packet_Sight_out(const Type_ID&);

	SCP_PLAYER_ATTACK Create_Packet_Normal_Attack(const Player* const, const Object* const, const Type_Damage&);
	SCP_PLAYER_ATTACK Create_Packet_Spell(const Player* const, const Object* const, const Type_Damage&);

	SCP_PLAYER_ATTACK_RESULT Create_Packet_Normal_Attack_Result(const Object* const, const Type_Damage&);
	SCP_PLAYER_ATTACK_RESULT Create_Packet_Spell_Result(const Object* const, const Type_Damage&);

	SCP_MONSTER_ATTACK Create_Packet_Monster_Attack(const Monster_Base* const, const Type_ID&, const Type_Damage&);

	void Change_Packet_data(SCP_SIGHT_IN&, const Object* const);
	void Change_Packet_data(SCP_SIGHT_OUT&, const Type_ID&);

private:

	static HANDLE* iocp;

	static Send_Manager* SndMgr;
	static Sector_Manager* SctMgr;
	static Object_Manager* ObjMgr;

	Send_Manager() = default;

public:
	static Send_Manager* Create_Send_Manager();
	void Include_Manager(HANDLE* const, Sector_Manager* const, Object_Manager* const);

};

