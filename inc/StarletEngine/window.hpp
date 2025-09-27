#pragma once

struct GLFWwindow;
class InputManager;

class Window {
public:
	Window() = default;
	~Window();

	bool createWindow(const unsigned int widthIn, const unsigned int heightIn, const char* title);

	GLFWwindow* getGLFWwindow() const { return window; }
	bool shouldClose() const;

	unsigned int getWidth()  const { return width; }
	unsigned int getHeight() const { return height; }
	float        getAspect() const { return static_cast<float>(width) / static_cast<float>(height); }

	void pollEvents() const;
	void swapBuffers() const;
	void requestClose() const;

	void setWindowPointer(void* userPointer) const;
	void setCurrentWindow() const;

	void updateViewport(const unsigned int width, const unsigned int height);

	bool switchActiveWindowVisibility();
	bool switchCursorLock();

private:
	GLFWwindow* window{ nullptr };
	unsigned int width{ 0 }, height{ 0 };
};