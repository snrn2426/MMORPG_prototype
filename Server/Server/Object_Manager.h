#pragma once

class Object;
class Player;
class Monster_type1;
class Sector_Manager;
class Terrain_Manager;
class Timer_Manager;
class Server;

class Object_Manager
{
private:
	std::array<Object*, MAX_OBJECTS> objects;
	std::array<LF::shared_ptr<Player>, MAX_PLAYER> players;

public:
	LF::shared_ptr<Player>* Login(const SOCKET&, const Type_ID&);
	void Logout(Player* const);

	Object*	Get_Object(const Type_ID&) const;
	LF::shared_ptr<Player>* Get_Player(const Type_ID&);

private:
	void insert(Object* const, const Type_ID&);
	void erase(Object* const);

private:
	static Object_Manager* ObjMgr;
	static Terrain_Manager* TrnMgr;

	Object_Manager() = default;

public:
	static Object_Manager* Create_Object_Manager();
	void Include_Manager(Sector_Manager* const, Terrain_Manager* const, Timer_Manager* const);
	void Create_Monsters();
};

