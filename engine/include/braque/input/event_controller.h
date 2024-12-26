//
// Created by Robert F. Dickerson on 12/26/24.
//

#ifndef EVENT_CONTROLLER_H
#define EVENT_CONTROLLER_H

namespace braque {
class EventController {
public:
   virtual ~EventController() = default;

  virtual void OnMouseMoved(float x, float y) {};
  virtual void OnMouseButtonPressed(int button, int action, int mods) {};
  virtual void OnMouseButtonReleased(int button, int action, int mods) {};
  virtual void OnMouseScrolled(float x, float y) {};
  virtual void OnKeyPressed(int key, int action, int mods) {};
  virtual void OnKeyReleased(int key, int action, int mods) {};

  };
}

#endif //EVENT_CONTROLLER_H
