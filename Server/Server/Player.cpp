#include "stdafx.h"
#include "Player.h"
#include "Terrain_Manager.h"
#include "Sector_Manager.h"
#include "Monster_Base.h"
#include "Timer_Manager.h"
#include "Object_Manager.h"

Player::Player(const SOCKET & _socket, const Type_ID& index)
{
	packet_start = player_exover.IObuf;
	recv_start = player_exover.IObuf;

	socket = _socket;
	id = index;

	die = false;

	pred_move_time = std::chrono::high_resolution_clock::now();

	Type_POS px, py;
	
	while (true) {
		px = (float)(rand() % (int)(WORLD_MAP_SIZE_WIDTH));
		py = (float)(rand() % (int)(WORLD_MAP_SIZE_HEIGHT));

		if (true == TrnMgr->Can_Create_Object(px, py))
			break;
	}

	target_x = x = px; // 459.f;
	target_y = y = py; // 204.f;

	vel = 3.f;
	Hp = 100000;
	str = 3;

	WSAReceive();
}

Player::Player(const Player& _player)
{
	packet_start = player_exover.IObuf;
	recv_start = player_exover.IObuf;

	socket = _player.socket;
	id = _player.id;

	die = false;

	pred_move_time = std::chrono::high_resolution_clock::now();

	target_x = x = _player.x;
	target_y = y = _player.y;

	vel = _player.vel;
	int _hp = _player.Hp;
	Hp = _hp;
	str = _player.str;

	WSAReceive();
}

Player::~Player()
{}

void Player::Get_Objects__Update_Near_set
	(std::vector<std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>>& vec_objects, std::vector<LF::shared_ptr<Player>*>& vec_p_sp_players)
{
	std::vector<Sector_Base*> vec_near_sector{ SctMgr->Get_near_sector(x, y) };
	
	Player* p_player;
	Object* p_object;

	for (Sector_Base* const sector : vec_near_sector) {

		sector->R_lock();

		for (const Type_ID& sector_obj_id : *sector) {

			if (sector_obj_id < MAX_PLAYER) {
				LF::shared_ptr<Player>* p_sp_player{ ObjMgr->Get_Player(sector_obj_id) };
				if (p_sp_player == nullptr)		continue;

				p_player = p_sp_player->get();
				if (p_player == nullptr) {
					delete p_sp_player;			continue;
				}

				if (false == is_near(p_player)) {
					delete p_sp_player;			continue;
				}

				vec_p_sp_players.emplace_back(p_sp_player);
				vec_objects.emplace_back(std::make_pair(p_player->id, RMOT_PLAYER_SIGHT_IN));
			}

			else {
				p_object = ObjMgr->Get_Object(sector_obj_id);
				if (p_object == nullptr)	continue;

				if (false == is_near(p_object)) continue;

				vec_objects.emplace_back(std::make_pair(p_object->id, RMOT_OBJECT_SIGHT_IN));
			}
		}

		sector->R_unlock();
	}

	std::vector<std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>>::iterator iter;
	near_set.R_lock();

	for (const Type_ID& near_obj_id : near_set) {

		if (near_obj_id < MAX_PLAYER) {

			LF::shared_ptr<Player>* p_sp_player{ ObjMgr->Get_Player(near_obj_id) };
			if (p_sp_player == nullptr) {
				vec_objects.emplace_back(std::make_pair(near_obj_id, RMOT_UNEXIST));
				delete p_sp_player;		 continue;
			}

			p_player = p_sp_player->get();
			if (p_player == nullptr) {
				vec_objects.emplace_back(std::make_pair(near_obj_id, RMOT_UNEXIST));
				delete p_sp_player;		 continue;
			}

			for (iter = vec_objects.begin(); iter != vec_objects.end(); ++iter) {
				if (iter->first != near_obj_id)	continue;
				
				iter->second = RMOT_PLAYER_NEAR;
				break;
			}

			if (iter == vec_objects.end()) 
				vec_objects.emplace_back(std::make_pair(near_obj_id, RMOT_PLAYER_SIGHT_OUT));

			vec_p_sp_players.emplace_back(p_sp_player);
		}

		else {
			p_object = ObjMgr->Get_Object(near_obj_id);
			if (p_object == nullptr)	continue;

			for (iter = vec_objects.begin(); iter != vec_objects.end(); ++iter) {
				if (iter->first != near_obj_id)	continue;

				iter->second = RMOT_OBJECT_NEAR;
				break;
			}

			if (iter == vec_objects.end())
				vec_objects.emplace_back(std::make_pair(near_obj_id, RMOT_OBJECT_SIGHT_OUT));
		}
	}

	near_set.R_unlock();

	for (const std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>& vec_element : vec_objects) {
	
		if (vec_element.second == RMOT_UNEXIST) continue;

		if (vec_element.second == RMOT_PLAYER_SIGHT_IN) {
			p_player = reinterpret_cast<Player*>(ObjMgr->Get_Object(vec_element.first));
			p_player->near_set.insert(id);
		}
		
		else if (vec_element.second == RMOT_OBJECT_SIGHT_IN || vec_element.second == RMOT_OBJECT_NEAR) {
			Monster_Base* p_monster{ reinterpret_cast<Monster_Base*>(ObjMgr->Get_Object(vec_element.first)) };

			if (true == p_monster->Awake()) {
				//printf("awake\n");
				TmrMgr->PQCS_Monster_Awake(p_monster->id);
			}
		}
	}

	near_set.Update(vec_objects);
}

bool Player::is_near(Object * const other_obj)
{
	if (PLAYER_NEAR_WIDTH < x - other_obj->x)	return false;
	if (PLAYER_NEAR_WIDTH < other_obj->x - x)	return false;
	if (PLAYER_NEAR_HEIGHT < y - other_obj->y)	return false;
	if (PLAYER_NEAR_HEIGHT < other_obj->y - y)	return false;
	if (other_obj == this)						return false;
	return true;
}

bool Player::Update_position()
{
	using namespace std;
	using namespace chrono;
	
	high_resolution_clock::time_point curr_time{ high_resolution_clock::now() };
	duration<float, milli> time{ duration_cast<milliseconds>(curr_time - pred_move_time) };
	pred_move_time = curr_time;

	CSP_MOVE* p_move_packet{ reinterpret_cast<CSP_MOVE*>(packet_start) };

	float length_packet_x{ p_move_packet->x - x };
	float length_packet_y{ p_move_packet->y - y };
	float length_packet{ hypotf(length_packet_x, length_packet_y) };

	float length_rate{ vel * 0.001f * TIME_MOVE_INTERPOLATION };

	if (length_packet < length_rate) {
		x = p_move_packet->x;
		y = p_move_packet->y;
		return true;
	}

	float length_target_x{ target_x - x };
	float length_target_y{ target_y - y };
	float length_target{ hypotf(length_target_x, length_target_y) };

	length_rate = vel * 0.001f * time.count();

	if (length_rate < length_target) {
		x += length_rate * length_target_x / length_target;
		y += length_rate * length_target_y / length_target; 
	}

	else {
		x = target_x;
		y = target_y;
	}
		
	return false;
}

bool Player::TEST_Update_position()
{
	using namespace std;
	using namespace chrono;

	high_resolution_clock::time_point curr_time{ high_resolution_clock::now() };
	duration<float, milli> time{ duration_cast<milliseconds>(curr_time - pred_move_time) };
	pred_move_time = curr_time;

	float length_target_x{ target_x - x };
	float length_target_y{ target_y - y };
	float length_target{ hypotf(length_target_x, length_target_y) };

	float length_rate = vel * 0.001f * time.count();

	if (length_rate < length_target) {
		x += length_rate * length_target_x / length_target;
		y += length_rate * length_target_y / length_target;
		return false;
	}

	else {
		x = target_x;
		y = target_y;

		int rand_x{ (rand() % 20) - 10 };
		int rand_y{ (rand() % 14) - 7 };

		target_x += (float)rand_x;
		target_y += (float)rand_y;
		return true;
	}
}

void Player::Update_Near_set_Sight_in(std::vector<Object*>& vec_near_objects, std::vector<LF::shared_ptr<Player>*>& vec_near_player )
{
	std::vector<Sector_Base*> vec_near_sector{ SctMgr->Get_near_sector(x, y) };
	
	Player* p_player;
	Object* p_object;

	for (Sector_Base* const sector : vec_near_sector) {

		sector->R_lock();

		for (const Type_ID& sector_obj_id : *sector) {
			
			if (sector_obj_id < MAX_PLAYER) {
				LF::shared_ptr<Player>* p_sp_player{ ObjMgr->Get_Player(sector_obj_id) };
				if (p_sp_player == nullptr)	continue;

				p_player = p_sp_player->get();
				if (p_player == nullptr) {
					delete p_sp_player;		continue;
				}

				if (false == is_near(p_player)) {
					delete p_sp_player;		continue;
				}

				vec_near_player.emplace_back(p_sp_player);
				vec_near_objects.emplace_back(p_player);
			}

			else {
				p_object = ObjMgr->Get_Object(sector_obj_id);
				if (p_object == nullptr)	continue;

				if (false == is_near(p_object)) continue;

				vec_near_objects.emplace_back(p_object);
			}
		}
		
		sector->R_unlock();
	}
	
	for (Object* const near_object : vec_near_objects) {
		if (true == near_object->Is_Player()){
			Player* p_player{ reinterpret_cast<Player*>(near_object) };
			p_player->near_set.insert(id);
		}
		/*else {
			Monster_Base* p_monster{ reinterpret_cast<Monster_Base*>(p_object) };
			if (true == p_monster->Awake()) TmrMgr->PQCS_Monster_Awake(p_monster->id);
		}*/
	}

	near_set.insert(vec_near_objects);
}

void Player::WSAReceive()
{
	ZeroMemory(&player_exover.exover.over, sizeof(WSAOVERLAPPED));
	player_exover.exover.type = EX_RECEIVE;
	player_exover.wsabuf.buf = reinterpret_cast<CHAR*>(player_exover.IObuf);
	player_exover.wsabuf.len = MAX_BUFFER;

	DWORD flag = 0;
	WSARecv(socket, &player_exover.wsabuf, 1, NULL, &flag, &player_exover.exover.over, NULL);
}

void Player::WSAReceive(CHAR* start_ptr, ULONG size)
{
	ZeroMemory(&player_exover.exover.over, sizeof(WSAOVERLAPPED));
	player_exover.exover.type = EX_RECEIVE;
	player_exover.wsabuf.buf = start_ptr;
	player_exover.wsabuf.len = size;

	DWORD flag = 0;
	WSARecv(socket, &player_exover.wsabuf, 1, NULL, &flag, &player_exover.exover.over, NULL);
}

bool Player::Is_Time_Recv_Move_packet()
{
	return time__recv_move.Set_time(TIME_RECV_PLAYER_MOVE);
}

bool Player::Is_Time_Recv_Move_Target_packet()
{
	if (false == time__recv_move.Set_time(TIME_RECV_PLAYER_MOVE))	return false;
	
	CSP_MOVE_TARGET* packet{ reinterpret_cast<CSP_MOVE_TARGET*>(packet_start) };

	target_x = packet->target_x;
	target_y = packet->target_y;

	pred_move_time = std::chrono::high_resolution_clock::now();

	return true;
}

bool Player::Is_Time_Recv_Normal_Attack()
{
	return time__recv_normal_attack.Set_time(TIME_RECV_PLAYER_NORMAL_ATTACK);
}

bool Player::Is_Time_Recv_SPELL()
{
	return time__recv_spell.Set_time(TIME_RECV_PLAYER_SPELL);
}

bool Player::TEST_Move_packet()
{
	return time__recv_move.Set_time(TIME_RECV_PLAYER_MOVE);
}

bool Player::Is_Player()
{
	return true;
}

Type_POS Player::Get_Size() const
{
	return SIZE_PLAYER;
}

RESULT_PLAYER_MOVE Player::Move()
{
	Type_POS pred_x{ x }, pred_y{ y };
	RESULT_PLAYER_MOVE result;

	if (false == Update_position()) result = RPM_MODIFY;
	else							result = RPM_CORRECT;

	if (false == TrnMgr->Can_move(this)) {
		Reset_position(pred_x, pred_y);				
		return RPM_INCORRECT;
	}

	SctMgr->Update_Sector(this, pred_x, pred_y);
	
	return result;
}

RESULT_PLAYER_MOVE Player::TEST_Move
	(std::vector<std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>>& vec_objects, std::vector<LF::shared_ptr<Player>*>& vec_players)
{
	Type_POS pred_x{ x }, pred_y{ y };
	RESULT_PLAYER_MOVE result;

	if (true == TEST_Update_position()) result = RPM_MODIFY;
	else								result = RPM_CORRECT;

	if (false == TrnMgr->Can_move(this)) {
		Reset_position(pred_x, pred_y);
		return RPM_INCORRECT;
	}

	SctMgr->Update_Sector(this, pred_x, pred_y);

	Get_Objects__Update_Near_set(vec_objects, vec_players);

	return result;
}

std::chrono::milliseconds Player::Get_respwan_time() const
{
	return TIME_PLAYER_RESPWAN;
}

Type_Damage Player::Normal_Attack(Object* const p_monster)
{
	Type_POS distance{ sqrtf(pow(x - p_monster->x, 2) + pow(y - p_monster->y, 2)) };
	
	if (DISTANCE_ALLOW_PLAYER_NORMAL_ATTACK + Get_Size() + p_monster->Get_Size() < distance)	return -1;

	Type_Damage damage{ str };

	if (true == p_monster->Hurt(damage, id))	return damage;

	return -1;
}

Type_Damage Player::Spell(Object* const p_monster)
{
	Type_POS distance{ sqrtf(pow(x - p_monster->x, 2) + pow(y - p_monster->y, 2)) };
	
	if (DISTANCE_ALLOW_PLAYER_SPELL < distance) return -1;

	Type_Damage damage{ str * 2 };

	if (true == p_monster->Hurt(damage, id))	return damage;

	return 0;
}

void Player::Respwan()
{
	die = false;

	pred_move_time = std::chrono::high_resolution_clock::now();

	target_x = x;
	target_y = y;

	vel = 3.f;
	Hp = 100;
	str = 3;
}

SOCKET Player::Get_Socket() const
{
	return socket;
}


bool Player::try_insert__near_set(const Type_ID& id)
{
	return near_set.try_insert(id);
}

void Player::Insert_Near_set(const Type_ID& id)
{
	near_set.insert(id);
}
