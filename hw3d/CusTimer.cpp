#include "CusTimer.h"

using namespace std::chrono;

CusTimer::CusTimer()
{
	last = steady_clock::now();
}

float CusTimer::Mark()
{
	const auto old = last;
	last = steady_clock::now();
	const duration<float> FrameTime = last - old;
	return FrameTime.count();
}

float CusTimer::Peek() const
{
	return duration<float>(steady_clock::now() - last).count();
}
