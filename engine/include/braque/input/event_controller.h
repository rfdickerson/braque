//
// Created by Robert F. Dickerson on 12/26/24.
//

#ifndef EVENT_CONTROLLER_H
#define EVENT_CONTROLLER_H

#include "event.h"

namespace braque {
class EventController {
public:
  EventController() = default;
virtual ~EventController() = default;

 EventController(const EventController& other) = delete;
 EventController(EventController&& other) noexcept = delete;
 EventController& operator=(const EventController& other) = delete;
 EventController& operator=(EventController&& other) noexcept = delete;

 virtual void OnMouseMoved(float /*x*/, float /*y*/) {};
//  virtual void OnMouseButtonPressed(int button, int action, int mods) {};
//  virtual void OnMouseButtonReleased(int button, int action, int mods) {};
//  virtual void OnMouseScrolled(float x, float y) {};
  virtual void OnKeyPressed(int /*key*/) {};
//  virtual void OnKeyReleased(int key, int action, int mods) {};
  virtual void OnEvent (Event /*event*/) {};

  };
}

#endif //EVENT_CONTROLLER_H
