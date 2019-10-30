#include "GetPrefixFromStr.cpp"
#include "GetStrFromZ.cpp"
#include "ProcessingStr.h"

void ProcessingStr::GetPrefixFromZ() {
  if (!initialized_prefix) {
    initialized_prefix = true;
    GetStrFromZ(); // сначала востанавливаем строку по z-функции
    GetPrefixFromStr(); // востанавливаем prefix по стоке
  }
}
