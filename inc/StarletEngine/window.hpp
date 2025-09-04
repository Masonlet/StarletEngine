#pragma once

struct GLFWwindow;

class Window {
public:
	~Window();

	void pollEvents() const;
	void swapBuffers() const;
	bool shouldClose() const;
	void updateViewport(const unsigned int width, const unsigned int height);

	inline GLFWwindow* getGLFWwindow() const { return window; }
	inline unsigned int getWidth() const { return width; }
	inline unsigned int getHeight() const { return height; }
	inline float getAspect() const { return static_cast<float>(width) / static_cast<float>(height); }

	bool createWindow(const unsigned int widthIn, const unsigned int heightIn, const char* title);
private:
	GLFWwindow* window{ nullptr };
	unsigned int width{ 0 }, height{ 0 };
};