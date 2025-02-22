#include "SystemEventBus.h"

SystemEventBus& SystemEventBus::getInstance() {
  static SystemEventBus instance;
  return instance;
}

void SystemEventBus::publish(Event&& event) {
  int index = static_cast<int>(event.type);
  if(index >= 0 && index < EVENT_TYPES_COUNT) {
    for(auto& handler : handlers[index]) {
      if(handler) {
        handler(std::move(event));
      }
    }
  }
}

template<typename T>
void SystemEventBus::subscribe(EventType eventType, void (*handler)(T)) {
  int index = static_cast<int>(eventType);
  if(index >= 0 && index < EVENT_TYPES_COUNT) {
    handlers[index].emplace_back([handler](Event&& event) {
      if(event.dataSize == sizeof(T)) {
        T value = *static_cast<T*>(event.data);
        handler(value);
      }
    });
  }
}