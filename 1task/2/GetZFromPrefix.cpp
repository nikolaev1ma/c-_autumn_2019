#include "GetStrFromPrefix.cpp"
#include "GetZFromStr.cpp"
#include "ProcessingStr.h"

void ProcessingStr::GetZFromPrefix() {
  GetStrFromPrefix(); // сначала получаем строку по префикс-функции
  GetZFromStr();      // получаем z-функцию по строке
}
