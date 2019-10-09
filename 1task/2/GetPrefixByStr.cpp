#include "String.h"

void String::get_prefix_by_str() {
  prefix.resize(str_size);
  for (int i = 1; i < str_size; ++i) {
    //если str[prefix[i - 1]] = str[i], то очевидно что prefix[i] = prefix[i -
    // 1] + 1 если же нет, то просто рекурсивно спускаемся
    int temp_pref = prefix[i - 1];
    while (temp_pref > 0 && str[i] != str[temp_pref]) {
      temp_pref = prefix[temp_pref - 1];
    }
    if (str[i] == str[temp_pref]) {
      ++temp_pref;
    }
    // если условие не выполнено, то prefix[i] = 0
    prefix[i] = temp_pref;
  }
}
