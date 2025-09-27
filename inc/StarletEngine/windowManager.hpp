#pragma once

#include "window.hpp"
#include <memory>

class WindowManager {
public:
  WindowManager();
  ~WindowManager();

	GLFWwindow* getGLFWwindow() const { return activeWindow ? activeWindow->getGLFWwindow() : nullptr; }
  unsigned int getWidth()      const { return activeWindow ? activeWindow->getWidth() : 0; }
  unsigned int getHeight()     const { return activeWindow ? activeWindow->getHeight() : 0; }
  float        getAspect()     const { return activeWindow ? static_cast<float>(activeWindow->getWidth()) / static_cast<float>(activeWindow->getHeight()) : -1.0f; }

  bool createWindow(const unsigned int width, const unsigned int height, const char* title);
  bool shouldClose() const { return activeWindow ? activeWindow->shouldClose() : true; }

  void pollEvents()   const { if (activeWindow) activeWindow->pollEvents(); }
  void swapBuffers()  const { if (activeWindow) activeWindow->swapBuffers(); }
  void requestClose() const { if (activeWindow) activeWindow->requestClose(); }

  void setWindowPointer(void* userPointer) const { if (activeWindow) activeWindow->setWindowPointer(userPointer); }

  void updateViewport(const unsigned int width, const unsigned int height) { if (activeWindow) activeWindow->updateViewport(width, height); }

  bool switchActiveWindowVisibility() const { return activeWindow ? activeWindow->switchActiveWindowVisibility() : false; }
  bool switchCursorLock() const { return activeWindow ? activeWindow->switchCursorLock() : false; }

private:
  std::unique_ptr<Window> activeWindow;
};