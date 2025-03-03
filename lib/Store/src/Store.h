#ifndef Store_h
#define Store_h

#include <Arduino.h>
#include <Preferences.h>

#define STORE_NAMESPACE "internalConfig"

#define WIFI_SSID "wifiSsid"
#define WIFI_PASSWORD "wifiPassword"

struct WifiSettings
{
  String ssid;
  String password;
};

class Store
{
public:
  Store();
  WifiSettings getWifiSettings();
  void storeWifiSettings(WifiSettings &settings);
  bool hasWifiSettings();
  void clear();

private:
  Preferences preferences;
};

#endif
