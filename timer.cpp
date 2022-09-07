#include "timer.h"

const unsigned Timer::s_freq{ []() { LARGE_INTEGER li; QueryPerformanceFrequency(&li); return static_cast<unsigned>(li.QuadPart); }() };

Timer::Timer() : m_startedAt{ this->get_tick() } {}




void Timer::reset()
{
	m_startedAt = this->get_tick();
}




void Timer::operator+=(double time)
{
	m_startedAt -= static_cast<__int64>(time * s_freq);
}

void Timer::operator-=(double time)
{
	m_startedAt += static_cast<__int64>(time * s_freq);
}




/* Vrátí èas od vytvoøení èasovaèe (nebo od zavolání funkce reset()). */
Timer::operator double() const
{
	return static_cast<double>(this->get_tick() - m_startedAt) / s_freq;
}




__int64 Timer::get_tick() const
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return li.QuadPart;
}


Timer::~Timer() {}