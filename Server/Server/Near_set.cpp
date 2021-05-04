#include "stdafx.h"
#include "Near_set.h"
#include "Object.h"

void Near_set::R_lock()
{
	mutex.lock_shared();
}

void Near_set::R_unlock()
{
	mutex.unlock_shared();
}

void Near_set::insert(const Type_ID& id)
{
	mutex.lock();

	set.insert(id);

	mutex.unlock();
}

void Near_set::insert(const std::vector<Object*>& vec_objects)
{
	mutex.lock();

	for (Object* const p_object : vec_objects)
		set.insert(p_object->Get_ID());
	
	mutex.unlock();
}

void Near_set::erase(const Type_ID& id)
{
	mutex.lock();

	set.erase(id);

	mutex.unlock();
}

bool Near_set::try_insert(const Type_ID& id)
{
	mutex.lock();
	auto pair_result = set.insert(id);
	mutex.unlock();

	return pair_result.second;
}

void Near_set::clear()
{
	mutex.lock();
	set.clear();
	mutex.unlock();
}

void Near_set::Update(const std::vector<std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>>& vec_id)
{	
	mutex.lock();

	for (const std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>& vec_element : vec_id) {
		
		if (vec_element.second == RMOT_PLAYER_SIGHT_IN ||
			vec_element.second == RMOT_OBJECT_SIGHT_IN) 
		{
			set.insert(vec_element.first);
		}

		else if (vec_element.second == RMOT_UNEXIST ||
			vec_element.second == RMOT_PLAYER_SIGHT_OUT ||
			vec_element.second == RMOT_OBJECT_SIGHT_OUT)
		{
			set.erase(vec_element.first);
		}
	}

	mutex.unlock();
}





