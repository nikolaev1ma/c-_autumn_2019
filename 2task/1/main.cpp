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

class SuffixArrayAndLcpBuilder {
public:
  explicit SuffixArrayAndLcpBuilder(string str_);

  const vector<int> &LCP() const;

private:
  const string str_;
  const int str_size_;
  vector<int> suff_arr_; // суффиксный массив
  vector<int> lcp_;

  vector<int> InitSuffixArray(); // построение суффиксного массива для
  // подстрок длины 1

  void UpdateSuffixArray(
      vector<int> equivalence_class); // построение суффиксного массива

  void ConstructLCP(); // построение lcp_

  vector<int>
  ConstructReverseArr() const; // построение обратного суффиксного массива
};

SuffixArrayAndLcpBuilder::SuffixArrayAndLcpBuilder(string str)
    : str_(std::move(str) + '$'), str_size_(str_.size()),
      suff_arr_(vector<int>(str_size_)), lcp_(vector<int>(str_size_)) {
  const auto equivalence_class =
      InitSuffixArray(); // строим начальный вспомогательный суффиксный
  // массив
  UpdateSuffixArray(
      std::move(equivalence_class)); // строим суффиксный массив рекуррентно
  ConstructLCP(); // строим lcp_
}

// должны по сути отсортировать буквы в массиве
// будем использовать сортировку подсчетом
vector<int> SuffixArrayAndLcpBuilder::InitSuffixArray() {
  vector<int> letter_count(
      size_alphabet); // колво использования каждой букве в слове
  for (const auto letter : str_) {
    if (letter != '$') {
      ++letter_count[letter - 'a'];
    }
  }
  for (int i = 1; i < size_alphabet; ++i) {
    letter_count[i] += letter_count[i - 1];
  }
  suff_arr_[0] = str_size_ - 1; // тк самый большой приортет у '$'
  // проходимся по строке, увеличивая приортет этой
  // же буквы двигаясь ближе к началу слова
  for (int i = str_size_ - 2; i >= 0; --i) {
    --letter_count[str_[i] - 'a'];
    suff_arr_[letter_count[str_[i] - 'a'] + 1] = i;
  }
  // теперь и востановили начальный suff_arr_
  vector<int> equivalence_class(str_size_);
  int classes_count = 1;
  equivalence_class[str_size_ - 1] = 0; // тк '$' имеет самый бльшой приоритет
  for (int i = 1; i < str_size_; ++i) {
    // условие того, что начинается новый класс эквивалентности
    if (str_[suff_arr_[i]] != str_[suff_arr_[i - 1]]) {
      ++classes_count;
    }
    equivalence_class[suff_arr_[i]] = classes_count - 1;
  }
  return equivalence_class;
}

// Должны отсортировать циклические подстроки длины 2 ^ k опираясь на сортировку
// циклических подстрок длины 2 ^ (k - 1)
void SuffixArrayAndLcpBuilder::UpdateSuffixArray(
    vector<int> equivalence_class) {
  // будем сортировать по степеням двойки, пока длина подстроки не привысит
  // str_size_
  int classes_count = 0;
  for (const auto class_ : equivalence_class) {
    classes_count = std::max(classes_count, class_);
  }
  ++classes_count;
  for (int degree = 0; (1 << degree) < str_size_; ++degree) {
    vector<int> offset_arr(
        str_size_); // сдвинутый по модулю суффиксный массив на 2 ^ k - 1
    for (int i = 0; i < str_size_; ++i) {
      offset_arr[i] = suff_arr_[i] - (1 << degree);
      if (offset_arr[i] < 0) {
        offset_arr[i] += str_size_;
      }
    }
    // тк suff_arr_ уже отсортирован по первым 2 ^ (k - 1) элементам,
    // то теперь сортировка зависит только от следующих 2 ^ (k - 1) элементов,
    // тоесть от сортирвки массива offset

    vector<int> count_elements_in_class(
        classes_count); // храним колво элементов в каждом классе
    // эквивалентности
    for (const auto element : offset_arr) {
      ++count_elements_in_class[equivalence_class[element]];
    }
    for (int i = 1; i < classes_count; ++i) {
      count_elements_in_class[i] += count_elements_in_class[i - 1];
    }
    for (int i = str_size_ - 1; i >= 0; --i) {
      const int tmp = offset_arr[i];
      --count_elements_in_class[equivalence_class[tmp]];
      suff_arr_[count_elements_in_class[equivalence_class[tmp]]] = tmp;
    }
    // теперь восстанавливаем массив классов эквивалетности
    vector<int> tmp_equivalence_class(str_size_);
    tmp_equivalence_class[suff_arr_[0]] = 0;
    classes_count = 1;
    for (int i = 1; i < str_size_; ++i) {
      const int first_begin = suff_arr_[i];
      const int second_begin = suff_arr_[i - 1];
      const int first_end = (suff_arr_[i] + (1 << degree)) % str_size_;
      const int second_end = (suff_arr_[i - 1] + (1 << degree)) % str_size_;
      // число классов увеличивается только при условии, что соседние строки не
      // совпадают, тоесть что текущее значение и сдвинутое на 2 ^ (k - 1) в
      // предыдущем классе эквивалентности различны
      if (equivalence_class[first_begin] != equivalence_class[second_begin] ||
          equivalence_class[first_end] != equivalence_class[second_end]) {
        ++classes_count;
      }
      tmp_equivalence_class[suff_arr_[i]] = classes_count - 1;
    }
    equivalence_class = std::move(tmp_equivalence_class);
  }
}

// разворачиваем суффиксный массив
vector<int> SuffixArrayAndLcpBuilder::ConstructReverseArr() const {
  vector<int> reverse_suff_arr_(str_size_);
  for (int i = 0; i < str_size_; ++i) {
    reverse_suff_arr_[suff_arr_[i]] = i;
  }
  return reverse_suff_arr_;
}

// построение lcp_
void SuffixArrayAndLcpBuilder::ConstructLCP() {
  const auto reverse_suff_arr_ = ConstructReverseArr();
  int shift = 0;
  lcp_[str_size_ - 1] = -1; // тк у него нет следующего за ним идущего
  for (int i = 0; i < str_size_; ++i) {
    if (reverse_suff_arr_[i] == str_size_ - 1) {
      shift = 0;
      continue;
    }
    if (shift >= 1) {
      --shift;
    }
    // если lcp_[reverse_suff_arr_[i]] > 1, то lcp_[reverse_suff_arr_[i]] - 1 <=
    // lcp_[reverse_suff_arr_[i + 1]] пользуемся этим для вычисления следующего
    // lcp_, храня shift
    const int next = suff_arr_[reverse_suff_arr_[i] + 1];
    while (std::max(next, i) + shift < str_size_ &&
           str_[next + shift] == str_[i + shift]) {
      ++shift;
    }
    lcp_[reverse_suff_arr_[i]] = shift;
  }
}

// возвращает lcp_
const vector<int> &SuffixArrayAndLcpBuilder::LCP() const { return lcp_; }

// считаем количество всех уникальных подстрок
int64_t NumberOfUniqueSubstrings(const SuffixArrayAndLcpBuilder &c) {
  const int str_size_ = c.LCP().size();
  int64_t result = 0;
  // сначала посчитаем все возможные подстроки
  result = str_size_ * (str_size_ - 1) / 2;
  // заметим, что в у i суффикса и у i + 1 суффикса в суффиксном массиве дважды
  // посчитано lcp_[i] идентичных префиксов этих суффиксов. Поэтому вычитае все
  // lcp_[i] из текущего результата
  for (int i = 0; i < str_size_ - 1; ++i) {
    result -= c.LCP()[i];
  }
  return result;
}

int main() {
  string input_string;
  cin >> input_string;
  const SuffixArrayAndLcpBuilder my_class(std::move(input_string));
  cout << NumberOfUniqueSubstrings(my_class) << '\n';
}
