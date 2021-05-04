#pragma once

#include <iostream>
#include <WS2tcpip.h>
#include <MSWSock.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "Mswsock.lib")

#include <thread>
#include <atomic>
#include <chrono>
#include <list>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include <cmath>
#include <algorithm>
#include <array>
#include <unordered_set>

#include "Lock-Free_Smart_Pointer.h"
#include "..\..\protocol.h"

using Type_sector = int;

#ifdef _WIN64
using POINTER_SIZE = long long;
using POINTER_TYPE = std::atomic_llong;
#else
using POINTER_SIZE = long;
using POINTER_TYPE = std::atomic_long;
#endif

using Type_atomic_Hp = std::atomic_int;
using Type_atomic_ID = std::atomic_int;

enum EXOVER_TYPE { EX_ACCEPT, EX_RECEIVE, EX_SEND,
	EX_MONSTER_AWAKE, EX_MONSTER_MOVE, EX_MONSTER_MOVE_START, EX_RESPWAN_PLAYER, EX_RESPWAN_MONSTER};

enum EVENT_TYPE	 { EV_MONSTER_MOVE, EV_MONSTER_MOVE_START, EV_RESPWAN};

enum RESULT_PLAYER_MOVE {RPM_CORRECT, RPM_MODIFY, RPM_INCORRECT};
enum RESULT_MONSTER_MOVE { RMM_CANT_MOVE, RMM_MOVE_TARGET, RMM_ATTACK, RMM_COLLISION };


enum RESULT_MOVE_OBJECT_TYPE { RMOT_UNEXIST,
	RMOT_PLAYER_SIGHT_IN, RMOT_PLAYER_SIGHT_OUT, RMOT_PLAYER_NEAR,
	RMOT_OBJECT_SIGHT_IN, RMOT_OBJECT_SIGHT_OUT, RMOT_OBJECT_NEAR };


constexpr int NUMBER_OF_THREAD__SERVER_PROCESS{ 3 };
constexpr int NUMBER_OF_THREAD__TIMER_PROCESS{ 2 };

// buffer info
constexpr size_t MAX_BUFFER{ 1024 };
constexpr size_t MIN_BUFFER{ 256 };

constexpr int WORLD_SECTOR_WIDTH_SIZE{ (int)(PLAYER_VIEW_SIZE_WIDTH * 4.f) };
constexpr int WORLD_SECTOR_HEIGHT_SIZE{ (int)(PLAYER_VIEW_SIZE_HEIGHT * 4.f) };

constexpr int WORLD_SECTOR_NUM_WIDTH{ (int)(WORLD_MAP_SIZE_WIDTH / WORLD_SECTOR_WIDTH_SIZE) };
constexpr int WORLD_SECTOR_NUM_HEIGHT{ (int)(WORLD_MAP_SIZE_HEIGHT / WORLD_SECTOR_HEIGHT_SIZE) };

constexpr int NUMBER_OF_SECTOR{ WORLD_SECTOR_NUM_WIDTH * WORLD_SECTOR_NUM_HEIGHT };

constexpr Type_POS WORLD_SECTOR_LEFT_EDGE{ PLAYER_VIEW_SIZE_WIDTH };
constexpr Type_POS WORLD_SECTOR_RIGHT_EDGE{ WORLD_SECTOR_WIDTH_SIZE - PLAYER_VIEW_SIZE_WIDTH };
constexpr Type_POS WORLD_SECTOR_TOP_EDGE{ PLAYER_VIEW_SIZE_HEIGHT };
constexpr Type_POS WORLD_SECTOR_BOTTOM_EDGE	{ WORLD_SECTOR_HEIGHT_SIZE - PLAYER_VIEW_SIZE_HEIGHT};

constexpr Type_POS MONSTER_PLAYER_NEAR_WIDTH{ PLAYER_VIEW_SIZE_WIDTH + 1.5f };
constexpr Type_POS MONSTER_PLAYER_NEAR_HEIGHT{ PLAYER_VIEW_SIZE_HEIGHT + 0.75f };

constexpr float TIME_MOVE_INTERPOLATION{ std::chrono::duration<float, std::milli>(CS_TIME_SEND_PLAYER_MOVE * 2).count() };

constexpr Type_POS PLAYER_NEAR_WIDTH{ PLAYER_VIEW_SIZE_WIDTH + 3.f };
constexpr Type_POS PLAYER_NEAR_HEIGHT{ PLAYER_VIEW_SIZE_HEIGHT + 1.5f };
constexpr Type_POS MONSTER_NEAR_WIDTH{ PLAYER_VIEW_SIZE_WIDTH + 0.5f };
constexpr Type_POS MONSTER_NEAR_HEIGHT{ PLAYER_VIEW_SIZE_HEIGHT + 0.25f };

constexpr std::chrono::milliseconds TIME_RECV_ALLOW_LATENCY{ 200 };
constexpr std::chrono::milliseconds TIME_RECV_PLAYER_MOVE{ CS_TIME_SEND_PLAYER_MOVE - TIME_RECV_ALLOW_LATENCY };			
constexpr std::chrono::milliseconds TIME_RECV_PLAYER_NORMAL_ATTACK{ CS_TIME_SEND_PLAYER_MOVE - TIME_RECV_ALLOW_LATENCY };	
constexpr std::chrono::milliseconds TIME_RECV_PLAYER_SPELL{ CS_TIME_SEND_PLAYER_MOVE - TIME_RECV_ALLOW_LATENCY };			

constexpr std::chrono::milliseconds TIME_MONSTER_TYPE1_NEXT_MOVE{ 2000 };	

constexpr std::chrono::milliseconds COOLTIME_MONSTER_TYPE1_ATTACK{ 700 };	
constexpr std::chrono::milliseconds COOLTIME_MONSTER_TYPE2_ATTACK{ 1500 };	
constexpr std::chrono::milliseconds COOLTIME_MONSTER_TYPE3_ATTACK{ 1500 };	

constexpr std::chrono::milliseconds TIME_PLAYER_RESPWAN{ 5000 };	
constexpr std::chrono::milliseconds TIME_MONSTER_RESPWAN{ 10000 };	

constexpr Type_POS DISTANCE_ALLOW_PLAYER_NORMAL_ATTACK{ DISTANCE_PLAYER_NORMAL_ATTACK + 0.3f };
constexpr Type_POS DISTANCE_ALLOW_PLAYER_SPELL{ DISTANCE_PLAYER_SPELL + 0.5f };

constexpr Type_POS DISTANCE_MONSTER_TYPE1_FAR_FROM_INIT_X{ MONSTER_NEAR_WIDTH / 2 };
constexpr Type_POS DISTANCE_MONSTER_TYPE1_FAR_FROM_INIT_Y{ MONSTER_NEAR_HEIGHT / 2 };
constexpr Type_POS DISTANCE_MONSTER_TYPE2_FAR_FROM_INIT_X{ MONSTER_NEAR_WIDTH };
constexpr Type_POS DISTANCE_MONSTER_TYPE2_FAR_FROM_INIT_Y{ MONSTER_NEAR_HEIGHT };
constexpr Type_POS DISTANCE_MONSTER_TYPE3_FAR_FROM_INIT_X{ MONSTER_NEAR_WIDTH * 2 };
constexpr Type_POS DISTANCE_MONSTER_TYPE3_FAR_FROM_INIT_Y{ MONSTER_NEAR_HEIGHT * 2 };