//
// Created by Robert F. Dickerson on 12/26/24.
//

#ifndef EVENT_H
#define EVENT_H

namespace braque {
enum class EventType {
  KeyPressed,
  KeyReleased,
  MouseMoved,
  MouseButtonPressed,
  MouseButtonReleased,
  MouseScrolled,
  AppQuit
};

struct Event {
  EventType type;
  int key;
  int action;
  int mods;
  float mouse_position_x;
  float mouse_position_y;
};
}

#endif //EVENT_H
