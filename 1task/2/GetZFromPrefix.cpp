#include "GetStrFromPrefix.cpp"
#include "GetZFromStr.cpp"
#include "ProcessingStr.h"

void ProcessingStr::GetZFromPrefix() {
  if (initialized_z) {
    return;
  }
  initialized_z = true;
  GetStrFromPrefix(); // сначала получаем строку по префикс-функции
  GetZFromStr();      // получаем z-функцию по строке

}
