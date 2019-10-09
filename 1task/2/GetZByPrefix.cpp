#include "GetStrByPrefix.cpp"
#include "GetZByStr.cpp"
#include "String.h"

void String::get_z_by_prefix() {
  get_str_by_prefix(); // сначала получаем строку по префикс-функции
  get_z_by_str();      // получаем z-функцию по строке
}
