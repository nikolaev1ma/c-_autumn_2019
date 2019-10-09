#include "GetPrefixByStr.cpp"
#include "GetStrByZ.cpp"
#include "String.h"

void String::get_prefix_by_z() {
  get_str_by_z(); //сначала востанавливаем строку по z-функции
  get_prefix_by_str(); //востанавливаем prefix по стоке
}
