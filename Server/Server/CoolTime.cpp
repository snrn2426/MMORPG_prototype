#include "stdafx.h"
#include "CoolTime.h"

CoolTime::CoolTime()
{
	time = std::chrono::high_resolution_clock::now();
}

bool CoolTime::Set_time(std::chrono::milliseconds add_time)
{
	std::chrono::high_resolution_clock::time_point curr_time
					{ std::chrono::high_resolution_clock::now() };
	
	if (curr_time < time)	return false;

	time = curr_time + add_time;
	return true;
}