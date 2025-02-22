#ifndef SYSTEM_EVENT_BUS_H
#define SYSTEM_EVENT_BUS_H

#include <vector>
#include <functional>
#include <cstdint>
#include <Arduino.h>

enum class EventType {
  WIFI_CONNECTED,
  WIFI_DISCONNECTED,
  SENSOR_UPDATE,
  DISPLAY_READY,
  ERROR_OCCURRED,
  BUTTON_PRESSED
};

struct Event {
  EventType type;
  void* data;
  size_t dataSize;
  
  // Constructor para eventos sin datos
  explicit Event(EventType t) : 
    type(t), data(nullptr), dataSize(0) {}
  
  // Constructor para eventos con datos
  template<typename T>
  Event(EventType t, const T& value) :
    type(t), data(new T(value)), dataSize(sizeof(T)) {}
  
  // Destructor para liberar memoria
  ~Event() {
    if(data != nullptr) {
      delete[] static_cast<uint8_t*>(data);
    }
  }
  
  // Eliminar copias
  Event(const Event&) = delete;
  Event& operator=(const Event&) = delete;
  
  // Movimiento
  Event(Event&& other) noexcept :
    type(other.type), data(other.data), dataSize(other.dataSize) {
    other.data = nullptr;
    other.dataSize = 0;
  }
};

class SystemEventBus {
public:
  using EventHandler = std::function<void(Event&&)>;
  
  static SystemEventBus& getInstance();
  
  template<typename T>
  void subscribe(EventType eventType, void (*handler)(T));
  
  void publish(Event&& event);

  // Eliminar operaciones de copia
  SystemEventBus(const SystemEventBus&) = delete;
  SystemEventBus& operator=(const SystemEventBus&) = delete;

private:
  SystemEventBus() = default;
  
  static constexpr int EVENT_TYPES_COUNT = 10;
  std::vector<EventHandler> handlers[EVENT_TYPES_COUNT];
};

#endif