#include <Arduino.h>
#include "StateMachine.h"

StateMachine::StateMachine(DState *states, int numStates, DTransition *transitions, int numTransitions)
    : states(states), numStates(numStates), transitions(transitions), numTransitions(numTransitions), currentState(nullptr), previousTime(0) {}

void StateMachine::begin(const char *initialState)
{
  for (int i = 0; i < numStates; i++)
  {
    if (states[i].id == initialState)
    {
      currentState = &states[i];
      break;
    }
  }
  if (currentState && currentState->onEnter)
  {
    currentState->onEnter(*this);
  }
  previousTime = millis();
}

void StateMachine::update()
{
  if (!currentState)
    return;

  unsigned long currentTime = millis();

  if (currentState->onState)
  {
    currentState->onState(*this);
  }

  if (currentState->interval > 0 && currentTime - previousTime >= currentState->interval)
  {
    changeState(-1); // Change to the next state based on the interval
  }
}

void StateMachine::changeState(int event)
{
  for (int i = 0; i < numTransitions; i++)
  {
    if (transitions[i].currentState == currentState->id && transitions[i].event == event)
    {
      // Ejecutar onExit si está definido
      if (currentState->onExit)
      {
        currentState->onExit(*this);
      }

      // Cambiar al siguiente estado
      for (int j = 0; j < numStates; j++)
      {
        if (states[j].id == transitions[i].nextState)
        {
          currentState = &states[j];
          previousTime = millis();

          // Ejecutar onEnter si está definido
          if (currentState->onEnter)
          {
            currentState->onEnter(*this);
          }
          return;
        }
      }
    }
  }
}

const char *StateMachine::getCurrentState()
{
  return currentState ? currentState->id : "NULL";
}