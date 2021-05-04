#pragma once

#include <unordered_set>

class Object;
class Player;

class Near_set
{
private:
	std::unordered_set<Type_ID> set;
	std::shared_mutex mutex;

public:
	std::unordered_set<Type_ID>::iterator			begin() { return set.begin(); }
	std::unordered_set<Type_ID>::const_iterator	begin() const { return set.begin(); }
	std::unordered_set<Type_ID>::iterator			end() { return set.end(); }
	std::unordered_set<Type_ID>::const_iterator	end()	const { return set.end(); }

public:
	void R_lock();
	void R_unlock();

	void insert(const Type_ID&);
	void insert(const std::vector<Object*>&);

	void erase(const Type_ID&);
	
	bool try_insert(const Type_ID&);

	void clear();
	void Update(const std::vector<std::pair<Type_ID, RESULT_MOVE_OBJECT_TYPE>>&);
};

