#include "ProcessingStr.h"

void ProcessingStr::GetZFromStr() {
  if (initialized_z) {
    return;
  }
  initialized_z = true;
  z.resize(str_size);
  int left = 0,
          right = 0; // [left, right] - самый правый паттерн, совпадающий с
  // началом строки
  for (int i = 1; i < str_size; ++i) {
    if (i <= right) {
      // заметим, что z[i] не меньше z[i - left] и right - i + 1
      z[i] = std::min(z[i - left], right - i + 1);
    }
    // наивный проход
    while (i + z[i] < str_size && str[z[i]] == str[i + z[i]]) {
      ++z[i];
    }
    if (i + z[i] > right) {
      // обновляем left, right
      right = i + z[i] - 1;
      left = i;
    }
  }

}
