#include "StarletEngine/timer.hpp"
#include "StarletParser/utils/log.hpp"

#include "GLFW/glfw3.h"

Timer::Timer() : lastTime(0.0f) {}

float Timer::tick() {
	const float currentTime = static_cast<float>(glfwGetTime());

	if (lastTime == 0.0f) {
		lastTime = currentTime;
		return 0.0f;
	}

	const float rawDelta = currentTime - lastTime;
	constexpr float maxDelta = 0.1f;

	lastTime = currentTime;
	float deltaTime = (rawDelta > maxDelta) ? maxDelta : rawDelta;

	if (rawDelta > maxDelta) debugLog("Engine", "Tick", "deltaTime clamped to " + std::to_string(maxDelta) + " (was " + std::to_string(rawDelta) + ")");
	return deltaTime;
}