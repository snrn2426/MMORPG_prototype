#include "stdafx.h"
#include "Monster_type3.h"
#include "Terrain_Manager.h"
#include "Sector_Manager.h"
#include "Timer_Manager.h"
#include "Object_Manager.h"
#include "Player.h"

Monster_type3::Monster_type3(const Type_ID& _id, const Type_POS& px, const Type_POS& py)
{
	id = _id;
	x = target_x = init_x = px;
	y = target_y = init_y = py;

	vel = 1.5f;
	Hp = Get_init_hp();
	str = 5;
}

void Monster_type3::Set_Move_Init_Target()
{
	target_x = init_x;
	target_y = init_y;
}

bool Monster_type3::Can_Move()
{
	if (x != target_x || y != target_y) return true;

	if (true == init_moving) {
		init_moving = false;
	}

	return false;
}

Player* Monster_type3::Get_Nearest_Player(std::vector<std::pair<Player*, RESULT_MOVE_OBJECT_TYPE>>& vec_objects, std::vector<LF::shared_ptr<Player>*>& vec_sp_players)
{
	std::vector<Sector_Base*> vec_near_sector{ SctMgr->Get_near_sector(x, y) };
	Type_ID target_player_id{ -1 };
	Type_POS nearest_len{ 100.f };

	Player* p_player;

	for (Sector_Base* const sector : vec_near_sector) {

		sector->R_lock();

		for (const Type_ID& sector_object_id : *sector) {

			if (MAX_PLAYER <= sector_object_id)	continue;

			LF::shared_ptr<Player>* p_sp_near_player{ ObjMgr->Get_Player(sector_object_id) };
			if (p_sp_near_player == nullptr)	continue;

			vec_sp_players.emplace_back(p_sp_near_player);

			p_player = p_sp_near_player->get();
			if (p_player == nullptr) 			continue;

			if (false == is_near(p_player))		continue;

			vec_objects.emplace_back(p_player, RMOT_PLAYER_NEAR);

			if (true == p_player->Is_die())	continue;

			Type_POS len_player{ abs(p_player->x - x) + abs(p_player->y - y) };

			if (len_player < nearest_len) {
				nearest_len = len_player;
				target_player_id = sector_object_id;
			}

		}

		sector->R_unlock();
	}

	if (target_player_id == -1) return nullptr;

	else {
		LF::shared_ptr<Player>* new_p_sp_target_player{ ObjMgr->Get_Player(target_player_id) };
		if (new_p_sp_target_player == nullptr)	return nullptr;

		p_player = new_p_sp_target_player->get();
		if (p_player == nullptr) {
			delete new_p_sp_target_player;		return nullptr;
		}
		vec_sp_players.emplace_back(new_p_sp_target_player);

		return p_player;
	}
}

void Monster_type3::Awake__Set_Move_target()
{
	pred_move_time = std::chrono::high_resolution_clock::now();
}

bool Monster_type3::Is_far_from_init_position()
{
	if (DISTANCE_MONSTER_TYPE3_FAR_FROM_INIT_X < x - init_x)		return true;
	if (DISTANCE_MONSTER_TYPE3_FAR_FROM_INIT_Y < init_x - x)		return true;
	if (DISTANCE_MONSTER_TYPE3_FAR_FROM_INIT_X * 2 < y - init_y)	return true;
	if (DISTANCE_MONSTER_TYPE3_FAR_FROM_INIT_Y < init_y - y)	return true;

	return false;
}

RESULT_MONSTER_MOVE Monster_type3::Move_or_Attack
(std::vector<std::pair<Player*, RESULT_MOVE_OBJECT_TYPE>>& vec_objects, std::vector<LF::shared_ptr<Player>*>& vec_sp_players, Type_ID& target_playe_id, Type_Damage& damage)
{
	using namespace std;
	using namespace chrono;

	high_resolution_clock::time_point curr_time{ high_resolution_clock::now() };

	if (false == init_moving) {
		Player* p_target_player{ Get_Nearest_Player(vec_objects, vec_sp_players) };

		if (nullptr != p_target_player) {
			RESULT_MONSTER_MOVE result{ Follow_Target(p_target_player) };

			if (RMM_ATTACK == result) {
				target_playe_id = p_target_player->id;
				damage = str;
				Set_Update_time(curr_time);
				return RMM_ATTACK;
			}

			if (RMM_COLLISION == result) {
				Set_Update_time(curr_time);
				return RMM_COLLISION;
			}
		}

		else {
			Set_Move_Init_Target();
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

	Get_Near_Player(vec_objects, vec_sp_players);

	SctMgr->Update_Sector(this, pred_x, pred_y);

	Is_Sleep__Update_Move(vec_objects);

	if (true == Is_far_from_init_position()) 
		Set_move_init_position();

	return	RMM_MOVE_TARGET;
}

Type_POS Monster_type3::Get_Size() const
{
	return SIZE_MONSTER_TYPE3;
}

std::chrono::milliseconds Monster_type3::Get_Next_Move_Cooltime() const
{
	return S_TIME_UPDATE_OBJECT_MOVE;
}

std::chrono::milliseconds Monster_type3::Get_Next_Attack_Cooltime() const
{
	return COOLTIME_MONSTER_TYPE3_ATTACK;
}

Type_Hp Monster_type3::Get_init_hp() const
{
	return HP_MONSTER_TYPE3;
}

char Monster_type3::Get_packet_type__attak() const
{
	return SCPT_MONSTER_TYPE3_ATTACK;
}

RESULT_MONSTER_MOVE Monster_type3::Follow_Target(Object* const p_target)
{
	if (true == init_moving)	return RMM_MOVE_TARGET;

	Type_POS distance{ sqrtf(pow(p_target->x - x, 2) + pow(p_target->y - y, 2)) };

	if (DISTANCE_MONSTER_TYPE3_ATTACK < distance) {
		target_x = p_target->target_x;
		target_y = p_target->target_y;
		return RMM_MOVE_TARGET;
	}

	target_x = x;
	target_y = y;

	if (false == time__attack.Set_time(COOLTIME_MONSTER_TYPE3_ATTACK)) return RMM_COLLISION;

	if (false == p_target->Hurt(str, id)) return RMM_MOVE_TARGET;

	return RMM_ATTACK;
}
