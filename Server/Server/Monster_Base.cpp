#include "stdafx.h"
#include "Monster_Base.h"
#include "Sector_Manager.h"
#include "Player.h"
#include "Timer_Manager.h"
#include "Terrain_Manager.h"
#include "Object_Manager.h"

void Monster_Base::Get_Near_Player
	(std::vector<std::pair<Player*, RESULT_MOVE_OBJECT_TYPE>>& vec_players, std::vector<LF::shared_ptr<Player>>& vec_sp_players)
{
	std::vector<Sector_Base*> vec_near_sector{ SctMgr->Get_near_sector(x, y) };
	Player* p_player;

	for (Sector_Base* const sector : vec_near_sector) {

		sector->R_lock();

		for (const Type_ID& sector_object_id : *sector) {

			if (MAX_PLAYER <= sector_object_id)	continue;

			LF::shared_ptr<Player>* p_sp_near_player{ &(ObjMgr->Get_Player(sector_object_id)) };
			if (p_sp_near_player == nullptr)	continue;

			p_player = p_sp_near_player->get();
			if (p_player == nullptr) 			continue;
			
			vec_sp_players.emplace_back(*p_sp_near_player);

			if (false == is_near(p_player))		continue;

			vec_players.emplace_back(std::make_pair(p_player, RMOT_PLAYER_NEAR));
		}

		sector->R_unlock();
	}
}

bool Monster_Base::Is_Sleep__Update_Move
	(std::vector<std::pair<Player*, RESULT_MOVE_OBJECT_TYPE>>& vec_objects)
{
	if (true == vec_objects.empty()) {
		Sleep();	return false;
	}

	else Awake();

	for (std::pair<Player*, RESULT_MOVE_OBJECT_TYPE>& vec_element : vec_objects) {
		Player* p_player{ reinterpret_cast<Player*>(vec_element.first) };

		if (true == p_player->try_insert__near_set(id))
			vec_element.second = RMOT_PLAYER_SIGHT_IN;
	}

	return true;
}

bool Monster_Base::is_near(Object* const other_obj)
{
	if (MONSTER_PLAYER_NEAR_WIDTH < x - other_obj->x)	return false;
	if (MONSTER_PLAYER_NEAR_WIDTH < other_obj->x - x)	return false;
	if (MONSTER_PLAYER_NEAR_HEIGHT < y - other_obj->y)	return false;
	if (MONSTER_PLAYER_NEAR_HEIGHT < other_obj->y - y)	return false;

	return true;
}

bool Monster_Base::is_near(Player* const p_player)
{
	Object* p_object{reinterpret_cast<Object*>(p_player)};
	return is_near(p_object);
}

void Monster_Base::Set_Update_time
	(std::chrono::high_resolution_clock::time_point current_time)
{
	pred_move_time = current_time;
}

void Monster_Base::Update_position
	(std::chrono::high_resolution_clock::time_point current_time)
{
	using namespace std;
	using namespace chrono;

	duration<float, milli> time{ duration_cast<milliseconds>(current_time - pred_move_time) };
	pred_move_time = current_time;

	float lenght_x{ target_x - x };
	float lenght_y{ target_y - y };

	float length{ hypotf(lenght_x, lenght_y) };

	float length_rate{ vel * 0.001f * time.count() };

	if (length < length_rate) {
		x = target_x;
		y = target_y;
	}

	else {
		x += length_rate * lenght_x / length;
		y += length_rate * lenght_y / length;
	}
}

bool Monster_Base::Sleep()
{
	bool _sleep = sleep;
	if (true == _sleep)	return false;

	if (false == CAS_bool(&sleep, false, true))	return false;

	//printf("sleep\n");
	target_x = init_x;
	target_y = init_y;

	return true;
}

bool Monster_Base::Is_sleep()
{
	bool _sleep = sleep;

	return _sleep;
}

bool Monster_Base::Can_Hurt()
{
	if (true == init_moving)	return false;
	if (true == die)			return false;
	return true;
}

bool Monster_Base::Is_Player()
{
	return false;
}

void Monster_Base::Set_move_init_position()
{
	target_x = init_x;
	target_y = init_y;
	init_moving = true;
}

std::chrono::milliseconds Monster_Base::Get_respwan_time() const
{
	return TIME_MONSTER_RESPWAN;
}

void Monster_Base::Respwan()
{
	die = false;
	sleep = true;

	init_moving = false;

	x = target_x = init_x;
	y = target_y = init_y;

	Hp = Get_init_hp();

	pred_move_time = std::chrono::high_resolution_clock::now();
}

Monster_Base::Monster_Base()
{
	die = false;
	sleep = true;

	init_moving = false;

	pred_move_time = std::chrono::high_resolution_clock::now();
}

bool Monster_Base::Awake()
{
	bool _sleep = sleep;
	if (false == _sleep)	return false;

	if (true == Is_die())	return false;

	return CAS_bool(&sleep, true, false);
}

