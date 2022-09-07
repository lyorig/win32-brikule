#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class Timer {
public:

	Timer();
	~Timer();

	void reset();

	void operator+=(double time);
	void operator-=(double time);

	operator double() const;

private:

	__int64					get_tick() const;

	__int64					m_startedAt;
	static const unsigned	s_freq;
};