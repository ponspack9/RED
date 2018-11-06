// ----------------------------------------------------
// j1Timer.cpp
// Fast timer with milisecons precision
// ----------------------------------------------------

#include "j1Timer.h"
#include "SDL\include\SDL_timer.h"

// ---------------------------------------------
j1Timer::j1Timer()
{
	Start();
}

// ---------------------------------------------
void j1Timer::Start()
{
	// DONE TODO 1: Fill Start(), Read(), ReadSec() methods
	// they are simple, one line each!
	started_at = Read();
}

// ---------------------------------------------
uint32 j1Timer::Read() const
{
	return SDL_GetTicks();
}

// ---------------------------------------------
float j1Timer::ReadSec() const
{
	return (SDL_GetTicks() - started_at);
}