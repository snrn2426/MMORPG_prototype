#include "stdafx.h"
#include <iostream>

#include "Object_Manager.h"
#include "Terrain_Manager.h"
#include "Object.h"
#include "Player.h"
#include "Monster_type1.h"
#include "Monster_type2.h"
#include "Monster_type3.h"

Object_Manager* Object_Manager::ObjMgr = nullptr;
Terrain_Manager* Object_Manager::TrnMgr = nullptr;

Object_Manager* Object_Manager::Create_Object_Manager()
{
	if (ObjMgr == nullptr) {
		ObjMgr = new Object_Manager();
		printf("[Object_Manager] - Create \n");
	}
	return ObjMgr;
}


LF::shared_ptr<Player>* Object_Manager::Login
	(const SOCKET& socket_player, const Type_ID& id)
{
	players[id] = LF::make_shared<Player>(socket_player, id);
	Player* p_player{ players[id].get() };
	objects[id] = reinterpret_cast<Object*>(p_player);
	
	return &players[id];
}

void Object_Manager::Logout(Player* const p_player)
{
	objects[p_player->id] = nullptr;
	players[p_player->id] = nullptr;
}

void Object_Manager::insert(Object* const p_object, const Type_ID& index)
{
	objects[index] = p_object;
}

void Object_Manager::erase(Object* const p_object)
{
	objects[p_object->id] = nullptr;
}

void Object_Manager::Include_Manager
	(Sector_Manager* const _SctMgr, Terrain_Manager* const _TrnMgr, Timer_Manager* const _TmrMgr)
{
	if (TrnMgr == nullptr) TrnMgr = _TrnMgr;

	Object::Include_Object_Manager(this);
	Object::Include_Sector_Manager(_SctMgr);
	Object::Include_Terrain_Manager(_TrnMgr);
	Object::Include_Timer_Manager(_TmrMgr);
}

void Object_Manager::Create_Monsters()
{
	/*
	for (int id = MAX_PLAYER; id < MAX_MONSTER_TYPE1; ++id ) 
		objects[id] = new Monster_type1(id, 455.f, 204.f);
	
	for (int id = MAX_MONSTER_TYPE1; id < MAX_MONSTER_TYPE2; ++id)
		objects[id] = new Monster_type2(id, 455.f, 209.f);

	for (int id = MAX_MONSTER_TYPE2; id < MAX_MONSTER_TYPE3; ++id)
		objects[id] = new Monster_type3(id, 459.f, 209.f);
	*/

	Type_POS width_end{ WORLD_MAP_SIZE_WIDTH };

	Type_POS px = 0.f, py = 0.f;

	/*for (int id = MAX_MONSTER_TYPE2; id < MAX_MONSTER_TYPE3; ) {
		if (true == TrnMgr->Can_Create_Object(px, py))
			objects[id] = new Monster_type3(id, px, py);

		px = (Type_POS)(px + 10.f);

		if (WORLD_MAP_SIZE_WIDTH <= px) {
			px = 0.f;
			py = (py + 10.f);

			if (WORLD_MAP_SIZE_HEIGHT <= py) {
				printf("M1 - max : %d \n", id - MAX_PLAYER);
				break;
			}
		}

		id++;
	}*/

	for (int id = MAX_PLAYER; id < MAX_MONSTER_TYPE1; ) {
		if (true == TrnMgr->Can_Create_Object(px, py)) 
			objects[id] = new Monster_type1(id, px, py);

		px = (Type_POS)(px + 15.f);

		if (WORLD_MAP_SIZE_WIDTH <= px) {
			px = 0.f;
			py = (py + 5.f);

			if (WORLD_MAP_SIZE_HEIGHT <= py) {
				printf("M1 - max : %d \n", id - MAX_PLAYER);
				break;
			}
		}

		id++;
	}

	px = 5.f, py = 0.f;

	for (int id = MAX_MONSTER_TYPE1; id < MAX_MONSTER_TYPE2; ) {
		
		if (true == TrnMgr->Can_Create_Object(px, py)) 
			objects[id] = new Monster_type2(id, px, py);
		
		px = (Type_POS)(px + 15.f);

		if (WORLD_MAP_SIZE_WIDTH <= px) {
			px = 5.f;
			py = (py + 5.f);
			if (WORLD_MAP_SIZE_HEIGHT <= py) {
				printf("M2 - max : %d \n", id - MAX_MONSTER_TYPE1);
				break;
			}
		}

		++id;
	}
	
	px = 10.f, py = 0.f;

	for (int id = MAX_MONSTER_TYPE2; id < MAX_MONSTER_TYPE3; ) {
		if (true == TrnMgr->Can_Create_Object(px, py)) 
			objects[id] = new Monster_type3(id, px, py);

		px = (Type_POS)(px + 15.f);

		if (WORLD_MAP_SIZE_WIDTH <= px) {
			px = 10.f;
			py = (py + 5.f);
			if (WORLD_MAP_SIZE_HEIGHT <= py) {
				printf("M3 - max : %d \n", id - MAX_MONSTER_TYPE2);
				break;
			}
		}

		++id;
	}

	printf("[Object_Manager] - Create Monsters \n");
}

Object* Object_Manager::Get_Object(const Type_ID& id) const
{
	Object* p_object{ objects[id] };

	return objects[id];
}

LF::shared_ptr<Player>& Object_Manager::Get_Player(const Type_ID& id)
{
	return *(new LF::shared_ptr<Player>(players[id]));
}
