#pragma once

#define __LINEAR_INTERPOLATION_PERFORMANCE_IMPROVEMENT__

#ifdef __LINEAR_INTERPOLATION_PERFORMANCE_IMPROVEMENT__
#define __LINEAR_INTERPOLATION_RECTANGLE__
#else
#define __LINEAR_INTERPOLATION_TRIANGLE__
#endif // __LINEAR_INTERPOLATION__


using Type_ID	= int;
using Type_POS	= float;
using Type_Hp = int;
using Type_Damage = Type_Hp;
using Type_EXP  = int;
using Type_LEV  = int;

// ** CONNECT ** 
constexpr int SERVER_PORT{ 3500 };

// ** WORLD ** 
constexpr Type_POS WORLD_MAP_SIZE_WIDTH{ 1000 };
constexpr Type_POS WORLD_MAP_SIZE_HEIGHT{ 1000 };
constexpr size_t WORLD_MAP_SIZE{ (size_t)(WORLD_MAP_SIZE_WIDTH * WORLD_MAP_SIZE_HEIGHT) };

// ** Object **
constexpr size_t MAX_PLAYER{ 3000 };
constexpr size_t NUMBER_OF_MONSTER_TYPE1{ 13399 };
constexpr size_t MAX_MONSTER_TYPE1{ MAX_PLAYER + NUMBER_OF_MONSTER_TYPE1 };
constexpr size_t NUMBER_OF_MONSTER_TYPE2{ 13399 };
constexpr size_t MAX_MONSTER_TYPE2{ MAX_MONSTER_TYPE1 + NUMBER_OF_MONSTER_TYPE2 };
constexpr size_t NUMBER_OF_MONSTER_TYPE3{ 13199 };
constexpr size_t MAX_MONSTER_TYPE3{ MAX_MONSTER_TYPE2 + NUMBER_OF_MONSTER_TYPE3 };
constexpr size_t MAX_OBJECTS{ MAX_MONSTER_TYPE3 };

constexpr Type_POS PLAYER_VIEW_SIZE_WIDTH{ 10 };
constexpr Type_POS PLAYER_VIEW_SIZE_HEIGHT{ 6.25 };

constexpr std::chrono::milliseconds S_TIME_UPDATE_OBJECT_MOVE{ 500 };	

constexpr std::chrono::milliseconds CS_TIME_SEND_PLAYER_MOVE{ 500 };	

constexpr std::chrono::milliseconds COOLTIME_PLAYER_NORMAL_ATTACK{ 500 };	
constexpr std::chrono::milliseconds COOLTIME_PLAYER_SPELL{ 1000 };	

constexpr Type_POS SIZE_PLAYER{ 0.5f };
constexpr Type_POS SIZE_MONSTER_TYPE1{ 0.25f };
constexpr Type_POS SIZE_MONSTER_TYPE2{ 0.5f };
constexpr Type_POS SIZE_MONSTER_TYPE3{ 0.5f };

constexpr Type_POS DISTANCE_PLAYER_NORMAL_ATTACK{ 0.1f };
constexpr Type_POS DISTANCE_PLAYER_SPELL{ 3.f };

constexpr Type_POS DISTANCE_MONSTER_TYPE3_ATTACK{ 3.f };

constexpr Type_POS VELOCITY_PLAYER_SPELL{ 4.f };
constexpr Type_POS VELOCITY_MONSTER_TYPE3_ATTACK{ 4.f };

constexpr Type_Hp HP_MONSTER_TYPE1{ 10 };
constexpr Type_Hp HP_MONSTER_TYPE2{ 20 };
constexpr Type_Hp HP_MONSTER_TYPE3{ 15 };

//  *********************************
//  ***** Player Login / Logout *****
//  *********************************

constexpr char SCPT_LOGIN_SUCCESS{ 001 };
constexpr char SCPT_LOGIN_FAILURE{ 002 };
		
#pragma pack(push ,1)
struct SCP_LOGIN_SUCCESS {
	char size;
	char type;
	Type_ID id;
	Type_POS x, y;
	Type_POS vel;
	Type_Hp Hp;
	Type_Damage str;
};

struct SCP_LOGIN_FAILURE {
	char size;
	char type;
};
#pragma pack (pop)

//  *********************************
//  ***** Player sight in / out *****
//  *********************************

constexpr char SCPT_SIGHT_IN{ 101 };
constexpr char SCPT_SIGHT_OUT{ 102 };

#pragma pack(push ,1)
struct SCP_SIGHT_IN {
	char size;
	char type;
	Type_ID id;
	Type_POS x, y;
	Type_POS target_x, target_y;
	Type_POS vel;
	Type_Hp Hp;
};

struct SCP_SIGHT_OUT {
	char size;
	char type;
	Type_ID id;
};
#pragma pack (pop)


//  ***********************
//  ***** Player move *****
//  ***********************

constexpr char CSPT_MOVE{ 001 };
constexpr char CSPT_MOVE_TARGET{ 002 };

constexpr char SCPT_MOVE{ 003 };
constexpr char SCPT_MOVE_TARGET{ 004 };

constexpr char T_MOVE_TARGET{ 005 };

#pragma pack(push ,1)
struct CSP_MOVE
{
	char	size;
	char	type;
	Type_POS x, y;
};

struct CSP_MOVE_TARGET {
	char	size;
	char	type;
	Type_POS x, y;
	Type_POS target_x, target_y;
};

struct SCP_MOVE {
	char	size;
	char	type;
	Type_ID id;
	Type_POS x, y;
};

struct SCP_MOVE_TARGET {
	char size;
	char type;
	Type_ID id;
	Type_POS x, y;
	Type_POS target_x, target_y;
};

struct TEST_MOVE_TARGET {
	char	size;
	char	type;
};

#pragma pack (pop)


//  ***********************
//  **** Player attack ****
//  ***********************

constexpr char CSPT_PLAYER_NORMAL_ATTACK{ 006 };
constexpr char CSPT_PLAYER_SPELL{ 007 };

constexpr char SCPT_PLAYER_NORMAL_ATTACK{ 010 };
constexpr char SCPT_PLAYER_SPELL{ 011 };

constexpr char SCPT_PLAYER_NORMAL_ATTACK_KILL{ 012 };
constexpr char SCPT_PLAYER_SPELL_KILL{ 013 };

constexpr char SCPT_PLAYER_NORMAL_ATTACK_RESULT{ 014 };
constexpr char SCPT_PLAYER_SPELL_RESULT{ 015 };

constexpr char SCPT_PLAYER_NORMAL_ATTACK_RESULT_KILL{ 016 };
constexpr char SCPT_PLAYER_SPELL_RESULT_KILL{ 017 };

constexpr char SCPT_MONSTER_TYPE1_ATTACK{ 020 }; 
constexpr char SCPT_MONSTER_TYPE2_ATTACK{ 021 };
constexpr char SCPT_MONSTER_TYPE3_ATTACK{ 022 };

#pragma pack(push ,1)
struct CSP_PLAYER_ATTACK
{
	char	size;
	char	type;
	Type_ID hurt_id;
	Type_Damage damage;
};

struct SCP_PLAYER_ATTACK
{
	char	size;
	char	type;
	Type_ID player_id;
	Type_ID hurt_id;
	Type_Damage damage;
};

struct SCP_PLAYER_ATTACK_RESULT
{
	char	size;
	char	type;
	Type_ID hurt_id;
	Type_Damage damage;
};

struct SCP_MONSTER_ATTACK
{
	char	size;
	char	type;
	Type_POS x, y;
	Type_POS target_x, target_y;
	Type_ID player_id;
	Type_ID monster_id;
	Type_Damage damage;
};

#pragma pack (pop)

constexpr char SCPT_RESPWAN{ 023 };

struct SCP_RESPWAN {
	char size;
	char type;
	Type_ID id;
	Type_POS x, y;
	Type_POS vel;
	Type_Hp Hp;
	Type_Damage str;
};