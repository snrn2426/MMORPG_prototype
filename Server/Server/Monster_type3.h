#pragma once
#include "Monster_Base.h"

class Player;

class Monster_type3 : public Monster_Base
{
private:

public:
	Monster_type3() = delete;
	Monster_type3(const Type_ID&, const Type_POS&, const Type_POS&);

private:
	void Set_Move_Init_Target();
	bool Can_Move();

public:
	Player* Get_Nearest_Player(std::vector<std::pair<Player*, RESULT_MOVE_OBJECT_TYPE>>&, std::vector<LF::shared_ptr<Player>>&);
	virtual void Awake__Set_Move_target() override final;
	virtual bool Is_far_from_init_position() override final;
	virtual RESULT_MONSTER_MOVE Move_or_Attack(std::vector<std::pair<Player*, RESULT_MOVE_OBJECT_TYPE>>&, std::vector<LF::shared_ptr<Player>>&, Type_ID&, Type_Damage&) override final;
	virtual Type_POS Get_Size() const override final;
	virtual std::chrono::milliseconds Get_Next_Move_Cooltime() const override final;
	virtual std::chrono::milliseconds Get_Next_Attack_Cooltime() const override final;

	virtual Type_Hp	Get_init_hp() const override final;
	virtual char Get_packet_type__attak() const override final;

	RESULT_MONSTER_MOVE Follow_Target(Object* const);
};


