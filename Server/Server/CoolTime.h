#pragma once

class CoolTime
{
private:
	std::chrono::high_resolution_clock::time_point time;

public:
	CoolTime();

	bool Set_time(std::chrono::milliseconds);
};

