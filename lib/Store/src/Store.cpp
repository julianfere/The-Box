#include <Arduino.h>
#include <Store.h>

Store::Store()
{
}

void Store::storeWifiSettings(WifiSettings &settings)
{
  preferences.begin(STORE_NAMESPACE, false);
  preferences.putString(WIFI_SSID, settings.ssid);
  preferences.putString(WIFI_PASSWORD, settings.password);
  preferences.end();
}

WifiSettings Store::getWifiSettings()
{
  preferences.begin(STORE_NAMESPACE, true);
  WifiSettings settings;
  settings.ssid = preferences.getString(WIFI_SSID, "");
  settings.password = preferences.getString(WIFI_PASSWORD, "");
  preferences.end();

  return settings;
}

bool Store::hasWifiSettings()
{
  Serial.println("ENTRE AL HAS WIFI SETTINGS");
  preferences.begin(STORE_NAMESPACE, true);
  Serial.println("=============================================");
  Serial.println("Wifi ssid:" + preferences.getString(WIFI_SSID, "no ta"));
  Serial.println("=============================================");

  bool has = preferences.getString(WIFI_SSID, "").length() > 0;
  preferences.end();

  return has;
}

void Store::clear()
{
  preferences.begin(STORE_NAMESPACE, false);
  preferences.clear();
  preferences.end();
}