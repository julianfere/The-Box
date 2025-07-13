#include "ButtonHandler.h"

extern ButtonHandler buttonHandler;
extern QueueHandle_t uiQueue;
extern QueueHandle_t inputQueue;

void ButtonHandler::setup()
{
  pinMode(ADC_PIN, INPUT);
  Serial.println("ButtonHandler: Usando GPIO35 (ADC1) - Compatible con WiFi");
}

InputMessage::Type ButtonHandler::readButtonType()
{
  int adcValue = analogRead(ADC_PIN);

  if (adcValue >= BTN_ACCEPT_MIN)
  {
    return InputMessage::BUTTON_SELECT;
  }
  else if (adcValue >= BTN_UP_MIN && adcValue <= BTN_UP_MAX)
  {
    return InputMessage::BUTTON_UP;
  }
  else if (adcValue >= BTN_DOWN_MIN && adcValue <= BTN_DOWN_MAX)
  {
    return InputMessage::BUTTON_DOWN;
  }

  return InputMessage::BUTTON_BACK;
}

void ButtonHandler::update()
{
  unsigned long now = millis();
  InputMessage::Type currentButton = readButtonType();

  if (currentButton != InputMessage::BUTTON_BACK)
  {
    if (!buttonPressed &&
        currentButton != lastButton &&
        (now - lastPress > DEBOUNCE_TIME) &&
        (now - lastRelease > RELEASE_TIME))
    {

      buttonPressed = true;
      lastButton = currentButton;
      lastPress = now;

      InputMessage msg = {currentButton, now};

      if (xQueueSend(inputQueue, &msg, 0) == pdTRUE)
      {
        Serial.println("✅ Botón enviado: " + String(currentButton) +
                       " (ADC: " + String(analogRead(ADC_PIN)) + ")");
      }
      else
      {
        Serial.println("⚠️ Cola input llena, botón ignorado");
      }
    }
  }
  else
  {
    if (buttonPressed)
    {
      buttonPressed = false;
      lastRelease = now;
      lastButton = InputMessage::BUTTON_BACK;
      Serial.println("🔄 Botón liberado");
    }
  }
}

void ButtonHandler::debugADC()
{
  Serial.println("=== CALIBRACIÓN ADC ===");
  for (int i = 0; i < 10; i++)
  {
    int value = analogRead(ADC_PIN);
    Serial.println("Lectura " + String(i) + ": " + String(value));
    delay(500);
  }
  Serial.println("======================");
}