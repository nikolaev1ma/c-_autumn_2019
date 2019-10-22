#include "ProcessingStr.h"

void ProcessingStr::GetPrefixFromStr() {
  prefix.reserve(str_size);
  prefix.emplace_back(0);
  for (int i = 1; i < str_size; ++i) {
    // если str[prefix[i - 1]] = str[i], то очевидно что
    // prefix[i] = prefix[i - 1] + 1 если же нет, то просто рекурсивно
    // спускаемся
    int tmp_prefix = prefix[i - 1];
    while (tmp_prefix > 0 && str[i] != str[tmp_prefix]) {
      tmp_prefix = prefix[tmp_prefix - 1];
    }
    if (str[i] == str[tmp_prefix]) {
      ++tmp_prefix;
    }
    // если условие не выполнено, то prefix[i] = 0
    prefix.emplace_back(tmp_prefix);
  }
}
