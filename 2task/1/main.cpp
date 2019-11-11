// ссылка на контест
// https://contest.yandex.ru/contest/14570/run-report/23198707/

#include <iostream>
#include <string>
#include <vector>

using std::cin;
using std::cout;
using std::string;
using std::vector;

int const size_alphabet = 26;

class SuffixArrayBuilder {
public:
  explicit SuffixArrayBuilder(string str_);

  vector<int> LCP() const;

private:
  string str;
  int str_size;
  int classes_count; // колво классов эквивалентности
  vector<int> suff_arr; // суффиксный массив
  vector<int> reverse_suff_arr; // обратный к суффиксному массиву
  vector<int> equivalence_class; // массив классов эквивалентности
  vector<int> lcp;

  void InitSuffixArray(); // построение суффиксного массива для
  // подстрок длины 1

  void UpdateSuffixArray(); // построение суффиксного массива

  void ConstructLCP(); // построение lcp

  void ConstructReverseArr(); // построение обратного суффиксного массива
};

SuffixArrayBuilder::SuffixArrayBuilder(string str_)
    : str(std::move(str_) + '$'), str_size(str.size()), classes_count(0),
      suff_arr(vector<int>(str_size)), equivalence_class(vector<int>(str_size)),
      reverse_suff_arr(vector<int>(str_size)), lcp(vector<int>(str_size)) {
  InitSuffixArray(); // строим начальный вспомогательный суффиксный
  // массив
  UpdateSuffixArray(); // строим суффиксный массив рекуррентно
  ConstructLCP(); // строим lcp
}

// должны по сути отсортировать буквы в массиве
// будем использовать сортировку подсчетом
void SuffixArrayBuilder::InitSuffixArray() {
  vector<int> letter_count(
      size_alphabet); // колво использования каждой букве в слове
  for (const auto letter : str) {
    if (letter != '$') {
      ++letter_count[letter - 'a'];
    }
  }
  for (int i = 1; i < size_alphabet; ++i) {
    letter_count[i] += letter_count[i - 1];
  }
  suff_arr[0] = str_size - 1; // тк самый большой приортет у '$'
  // проходимся по строке, увеличивая приортет этой
  // же буквы двигаясь ближе к началу слова
  for (int i = str_size - 2; i >= 0; --i) {
    --letter_count[str[i] - 'a'];
    suff_arr[letter_count[str[i] - 'a'] + 1] = i;
  }
  // теперь и востановили начальный suff_arr
  equivalence_class[str_size - 1] = 0; // тк '$' имеет самый бльшой приоритет
  classes_count = 1;
  for (int i = 1; i < str_size; ++i) {
    // условие того, что начинается новый класс эквивалентности
    if (str[suff_arr[i]] != str[suff_arr[i - 1]]) {
      ++classes_count;
    }
    equivalence_class[suff_arr[i]] = classes_count - 1;
  }
}

// Должны отсортировать циклические подстроки длины 2 ^ k опираясь на сортировку
// циклических подстрок длины 2 ^ (k - 1)
void SuffixArrayBuilder::UpdateSuffixArray() {
  // будем сортировать по степеням двойки, пока длина подстроки не привысит
  // str_size
  for (int degree = 0; (1 << degree) < str_size; ++degree) {
    vector<int> offset_arr(
        str_size); // сдвинутый по модулю суффиксный массив на 2 ^ k - 1
    for (int i = 0; i < str_size; ++i) {
      offset_arr[i] = suff_arr[i] - (1 << degree);
      if (offset_arr[i] < 0) {
        offset_arr[i] += str_size;
      }
    }
    // тк suff_arr уже отсортирован по первым 2 ^ (k - 1) элементам,
    // то теперь сортировка зависит только от следующих 2 ^ (k - 1) элементов,
    // тоесть от сортирвки массива offset
    vector<int> count_elements_in_class(
        classes_count); // храним колво элементов в каждом классе
    // эквивалентности
    for (auto element : offset_arr) {
      ++count_elements_in_class[equivalence_class[element]];
    }
    for (int i = 1; i < classes_count; ++i) {
      count_elements_in_class[i] += count_elements_in_class[i - 1];
    }
    for (int i = str_size - 1; i >= 0; --i) {
      const int tmp = offset_arr[i];
      --count_elements_in_class[equivalence_class[tmp]];
      suff_arr[count_elements_in_class[equivalence_class[tmp]]] = tmp;
    }
    // теперь восстанавливаем массив классов эквивалетности
    vector<int> tmp_equivalence_class(str_size);
    tmp_equivalence_class[suff_arr[0]] = 0;
    classes_count = 1;
    for (int i = 1; i < str_size; ++i) {
      int first_begin = suff_arr[i];
      int second_begin = suff_arr[i - 1];
      int first_end = (suff_arr[i] + (1 << degree)) % str_size;
      int second_end = (suff_arr[i - 1] + (1 << degree)) % str_size;
      // число классов увеличивается только при условии, что соседние строки не
      // совпадают, тоесть что текущее значение и сдвинутое на 2 ^ (k - 1) в
      // предыдущем классе эквивалентности различны
      if (equivalence_class[first_begin] != equivalence_class[second_begin] ||
          equivalence_class[first_end] != equivalence_class[second_end]) {
        ++classes_count;
      }
      tmp_equivalence_class[suff_arr[i]] = classes_count - 1;
    }
    equivalence_class = std::move(tmp_equivalence_class);
  }
}

// разворачиваем суффиксный массив
void SuffixArrayBuilder::ConstructReverseArr() {
  for (int i = 0; i < str_size; ++i) {
    reverse_suff_arr[suff_arr[i]] = i;
  }
}

// построение lcp
void SuffixArrayBuilder::ConstructLCP() {
  ConstructReverseArr();
  int shift = 0;
  lcp[str_size - 1] = -1; // тк у него нет следующего за ним идущего
  for (int i = 0; i < str_size; ++i) {
    if (reverse_suff_arr[i] == str_size - 1) {
      shift = 0;
      continue;
    }
    if (shift >= 1) {
      --shift;
    }
    // если lcp[reverse_suff_arr[i]] > 1, то lcp[reverse_suff_arr[i]] - 1 <=
    // lcp[reverse_suff_arr[i + 1]] пользуемся этим для вычисления следующего
    // lcp, храня shift
    int next = suff_arr[reverse_suff_arr[i] + 1];
    while (std::max(next, i) + shift < str_size &&
           str[next + shift] == str[i + shift]) {
      ++shift;
    }
    lcp[reverse_suff_arr[i]] = shift;
  }
}

// возвращает lcp
vector<int> SuffixArrayBuilder::LCP() const { return lcp; }

// считаем количество всех уникальных подстрок
int64_t NumberOfUniqueSubstrings(const SuffixArrayBuilder &c) {
  int str_size = c.LCP().size();
  int result = 0;
  // сначала посчитаем все возможные подстроки
  result = str_size * (str_size - 1) / 2;
  // заметим, что в у i суффикса и у i + 1 суффикса в суффиксном массиве дважды
  // посчитано lcp[i] идентичных префиксов этих суффиксов. Поэтому вычитае все
  // lcp[i] из текущего результата
  for (int i = 0; i < str_size - 1; ++i) {
    result -= c.LCP()[i];
  }
  return result;
}

int main() {
  string input_string;
  cin >> input_string;
  SuffixArrayBuilder my_class(std::move(input_string));
  cout << NumberOfUniqueSubstrings(my_class) << '\n';
}
