#ifndef StateMachine_h
#define StateMachine_h

#include <Arduino.h>

// Definir el tipo de callback para los estados
using StateCallback = void (*)(class StateMachine &);

// Estructura para representar un estado
struct DState
{
  const char *id;         // Identificador del estado (usando un string)
  StateCallback onEnter;  // Callback al entrar al estado
  StateCallback onState;  // Callback mientras el estado está activo
  StateCallback onExit;   // Callback al salir del estado
  unsigned long interval; // Intervalo de tiempo para este estado (opcional)
};

// Estructura para representar una transición
struct DTransition
{
  const char *currentState; // Estado actual (usando un string)
  int event;                // Evento que dispara la transición
  const char *nextState;    // Siguiente estado (usando un string)
};

// Clase para la máquina de estados
class StateMachine
{
public:
  // Constructor
  StateMachine(DState *states, int numStates, DTransition *transitions, int numTransitions);

  // Método para inicializar la máquina de estados
  void begin(const char *initialState);

  // Método para actualizar la máquina de estados
  void update();

  // Método para cambiar de estado basado en un evento
  void changeState(int event);

  // Método para obtener el estado actual
  const char *getCurrentState();

private:
  DState *states;             // Lista de estados
  int numStates;              // Número de estados
  DTransition *transitions;   // Lista de transiciones
  int numTransitions;         // Número de transiciones
  DState *currentState;       // Estado actual
  unsigned long previousTime; // Último momento en que se cambió de estado
};

#endif