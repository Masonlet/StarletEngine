#pragma once

#include "window.hpp"
#include <memory>

class InputManager;

class WindowManager {
public:
  WindowManager();
  ~WindowManager();

  bool createWindow(const unsigned int width, const unsigned int height, const char* title);
  inline bool shouldClose() const { return activeWindow ? activeWindow->shouldClose() : true; }

  inline unsigned int getWidth()      const { return activeWindow ? activeWindow->getWidth() : 0; }
  inline unsigned int getHeight()     const { return activeWindow ? activeWindow->getHeight() : 0; }
  inline float        getAspect()     const { return activeWindow ? static_cast<float>(activeWindow->getWidth()) / static_cast<float>(activeWindow->getHeight()) : -1.0; }

  inline void pollEvents()   const { if (activeWindow) activeWindow->pollEvents(); }
  inline void swapBuffers()  const { if (activeWindow) activeWindow->swapBuffers(); }
  inline void requestClose() const { if (activeWindow) activeWindow->requestClose(); }

  inline void setWindowPointer(void* userPointer) const { if (activeWindow) activeWindow->setWindowPointer(userPointer); }

  inline void updateInput(InputManager& inputManager) { if (activeWindow) activeWindow->updateInput(inputManager); }
  inline void updateViewport(const unsigned int width, const unsigned int height) { if (activeWindow) activeWindow->updateViewport(width, height); }

  inline bool switchActiveWindowVisibility() const { return activeWindow ? activeWindow->switchActiveWindowVisibility() : false; }
  inline bool switchCursorLock() const { return activeWindow ? activeWindow->switchCursorLock() : false; }

private:
  std::unique_ptr<Window> activeWindow;
};