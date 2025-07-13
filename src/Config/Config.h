#ifndef CONFIG_H
#define CONFIG_H

// Configuración del proyecto
#define CLIENT_ID "CLIENT_ID"
#define CLIENT_SECRET "CLIENT_SECRET"

// Configuración de colas
#define UI_QUEUE_SIZE 15
#define INPUT_QUEUE_SIZE 8

// Timeouts
#define DEBOUNCE_TIME 200
#define RELEASE_TIME 100
#define WIFI_TIMEOUT 10000
#define NTP_SYNC_INTERVAL 600000 // 10 minutos

// UI Settings
#define ANIMATION_INTERVAL 800
#define CLOCK_UPDATE_INTERVAL 1000
#define SPOTIFY_UPDATE_INTERVAL 3000
#define WEATHER_CACHE_TIME 300000 // 5 minutos

#endif