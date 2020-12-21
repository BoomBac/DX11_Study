#pragma once

#include <chrono>


class CusTimer
{
public:
	CusTimer();
	float Mark();
	float Peek() const;
private:
	std::chrono::steady_clock::time_point last;
};

