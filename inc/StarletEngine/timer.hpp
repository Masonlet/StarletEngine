#pragma once

class Timer {
public:
	Timer();

	float tick();

private:
	  float lastTime;
};