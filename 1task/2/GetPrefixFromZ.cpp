#include "GetPrefixFromStr.cpp"
#include "GetStrFromZ.cpp"
#include "ProcessingStr.h"

void ProcessingStr::GetPrefixFromZ() {
  GetStrFromZ(); // сначала востанавливаем строку по z-функции
  GetPrefixFromStr(); // востанавливаем prefix по стоке
}
