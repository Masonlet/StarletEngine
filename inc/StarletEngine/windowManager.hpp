#pragma once

#include "window.hpp"

class InputManager;

class WindowManager {
public:
  WindowManager();
  ~WindowManager();

  bool createWindow(const unsigned int width, const unsigned int height, const char* title);
  void destroyWindow();

  inline Window* getWindow() const { return window; }
  void pollEvents() const;
  void swapBuffers() const;
  bool shouldClose() const;

  void switchActiveWindowVisibility();
  void updateInput(InputManager& inputManager);
  void requestClose();

private:
  Window* window = nullptr;

  void setInitialViewport();
};