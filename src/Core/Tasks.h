#ifndef TASKS_H
#define TASKS_H

#include <Arduino.h>

// Declaraciones de tasks
void uiTask(void *parameters);
void inputTask(void *parameters);
void screenUpdateTask(void *parameters);

// Funciones de setup
void setupTasks();

#endif
