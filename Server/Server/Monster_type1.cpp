#include "stdafx.h"
#include "Monster_type1.h"
#include "Terrain_Manager.h"
#include "Sector_Manager.h"
#include "Object_Manager.h"
#include "Timer_Manager.h"
#include "Player.h"

Monster_type1::Monster_type1(const Type_ID& _id, const Type_POS& px, const Type_POS& py)
{
	target_player_id = -1;

	id = _id;
	x = target_x = init_x = px;
	y = target_y = init_y = py;
	
	vel = 1.f;
	Hp = Get_init_hp();
	str = 5;
}

void Monster_type1::Set_Move_New_Target()
{
	int rand_x{ (rand() % 10) - 5 };
	int rand_y{ (rand() % 10) - 5 };

	target_x += (float)rand_x;
	target_y += (float)rand_y;
}

Player* Monster_type1::Get_exist_target_object(std::vector<LF::shared_ptr<Player>>& vec_sp_players)
{
	Type_ID curr_target_player_id{ target_player_id };
	if (curr_target_player_id == -1)	return nullptr;

	LF::shared_ptr<Player>* curr_p_sp_target_player{ &(ObjMgr->Get_Player(target_player_id)) };
	if (curr_p_sp_target_player == nullptr) {
		CAS_ID(&target_player_id, curr_target_player_id, -1);	return nullptr;
	}

	Player* p_player{ curr_p_sp_target_player->get()};
	if (p_player == nullptr) {
		delete curr_p_sp_target_player;
		CAS_ID(&target_player_id, curr_target_player_id, -1);	return nullptr;
	}

	if (true == p_player->Is_die()) {
		delete curr_p_sp_target_player;
		CAS_ID(&target_player_id, curr_target_player_id, -1);	return nullptr;
	}

	vec_sp_players.emplace_back(*curr_p_sp_target_player);

	return p_player;
}

bool Monster_type1::Can_Move()
{
	if (x != target_x || y != target_y) return true;

	if (true == init_moving) {
		target_player_id = -1;
		init_moving = false;
	}

	return false;
}

void Monster_type1::Awake__Set_Move_target()
{
	int rand_x{ (rand() % 10) - 5 };
	int rand_y{ (rand() % 10) - 5 };

	target_x += (float)rand_x;
	target_y += (float)rand_y;

	pred_move_time = std::chrono::high_resolution_clock::now();
}

bool Monster_type1::Is_far_from_init_position()
{
	if (DISTANCE_MONSTER_TYPE1_FAR_FROM_INIT_X < x - init_x)		return true;
	if (DISTANCE_MONSTER_TYPE1_FAR_FROM_INIT_Y < init_x - x)		return true;
	if (DISTANCE_MONSTER_TYPE1_FAR_FROM_INIT_X < y - init_y)	return true;
	if (DISTANCE_MONSTER_TYPE1_FAR_FROM_INIT_Y < init_y - y)	return true;

	return false;
}

RESULT_MONSTER_MOVE Monster_type1::Move_or_Attack
	(std::vector<std::pair<Player*, RESULT_MOVE_OBJECT_TYPE>>& vec_objects, std::vector<LF::shared_ptr<Player>>& vec_sp_players, Type_ID& target_playe_id, Type_Damage& damage)
{
	using namespace std;
	using namespace chrono;

	high_resolution_clock::time_point curr_time{ high_resolution_clock::now() };
	
	if (false == init_moving) {
		Player* p_target_player{ Get_exist_target_object(vec_sp_players) };

		if (nullptr != p_target_player) {
			RESULT_MONSTER_MOVE result{ Follow_Target(p_target_player) };

			if (RMM_ATTACK == result) {
				target_playe_id = p_target_player->id;
				damage = str;
				Get_Near_Player(vec_objects, vec_sp_players);
				Set_Update_time(curr_time);
				return RMM_ATTACK;
			}

			if (RMM_COLLISION == result) {
				Set_Update_time(curr_time);
				return RMM_COLLISION;
			}
		}

		else {
			Set_Move_New_Target();
		}
	}

	if (false == Can_Move()) {
		Set_Update_time(curr_time);
		return RMM_CANT_MOVE;
	}
	
	Type_POS pred_x{ x }, pred_y{ y };

	Update_position(curr_time);

	if (false == TrnMgr->Can_move(this)) {
		Reset_position(pred_x, pred_y);
		return RMM_CANT_MOVE;
	}

	SctMgr->Update_Sector(this, pred_x, pred_y);

	Get_Near_Player(vec_objects, vec_sp_players);

	Is_Sleep__Update_Move(vec_objects);

	if (true == Is_far_from_init_position())
		Set_move_init_position();
		
	return	RMM_MOVE_TARGET;
}

Type_POS Monster_type1::Get_Size() const
{
	return SIZE_MONSTER_TYPE1;
}

std::chrono::milliseconds Monster_type1::Get_Next_Move_Cooltime() const
{
	return TIME_MONSTER_TYPE1_NEXT_MOVE;
}

std::chrono::milliseconds Monster_type1::Get_Next_Attack_Cooltime() const
{
	return COOLTIME_MONSTER_TYPE1_ATTACK;
}

bool Monster_type1::Hurt(const Type_Damage& damage, const Type_ID& target_id)
{
	Type_ID curr_target_player_id{ target_player_id };

	if (curr_target_player_id != target_id) 
		CAS_ID(&target_player_id, curr_target_player_id, target_id);
	
	return Object::Hurt(damage, target_id);
}

Type_Hp Monster_type1::Get_init_hp() const
{
	return HP_MONSTER_TYPE1;
}

char Monster_type1::Get_packet_type__attak() const
{
	return SCPT_MONSTER_TYPE1_ATTACK;
}

RESULT_MONSTER_MOVE Monster_type1::Follow_Target(Object* const p_target)
{
	Type_POS distance{ sqrtf(pow(p_target->x - x, 2) + pow(p_target->y - y, 2)) };

	if (Get_Size() + p_target->Get_Size() < distance) {
		target_x = p_target->target_x;
		target_y = p_target->target_y;
		return RMM_MOVE_TARGET;
	}

	target_x = x;
	target_y = y;

	if (false == time__attack.Set_time(COOLTIME_MONSTER_TYPE1_ATTACK)) return RMM_COLLISION;

	if (false == p_target->Hurt(str, id)) return RMM_MOVE_TARGET;

	return RMM_ATTACK;
}
