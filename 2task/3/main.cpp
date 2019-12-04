// ссылка на контест
// https://contest.yandex.ru/contest/14570/run-report/23350728/

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
  SuffixArrayAndLcpBuilder(string a, string b, int64_t a_size);

  void SearchKStatistic(int64_t k) const;

private:
  const string str_;
  const int64_t a_size_; // длина первой строки

  const int64_t str_size_;
  vector<int64_t> suff_arr_; // суффиксный массив
  vector<int64_t> lcp_;

  vector<int64_t> InitSuffixArray(); // построение суффиксного массива для
  // подстрок длины 1

  void UpdateSuffixArray(
      vector<int64_t> equivalence_class); // построение суффиксного массива

  void ConstructLCP(); // построение lcp_

  vector<int64_t>
  ConstructReverseArr() const; // построение обратного суффиксного массива
};

SuffixArrayAndLcpBuilder::SuffixArrayAndLcpBuilder(string a, string b,
                                                   int64_t a_size)
    : a_size_(a_size), str_(std::move(a) + '$' + std::move(b) + '#'),
      str_size_(str_.size()), suff_arr_(vector<int64_t>(str_size_)),
      lcp_(vector<int64_t>(str_size_)) {
  const auto equivalence_class =
      InitSuffixArray(); // строим начальный вспомогательный суффиксный
  // массив
  UpdateSuffixArray(
      std::move(equivalence_class)); // строим суффиксный массив рекуррентно
  ConstructLCP(); // строим lcp_
}

// должны по сути отсортировать буквы в массиве
// будем использовать сортировку подсчетом
vector<int64_t> SuffixArrayAndLcpBuilder::InitSuffixArray() {
  suff_arr_[str_size_ - 1] = 0; // так как '$' имеет самый большой приоритет
  suff_arr_[a_size_] = 1; // так как '#' имеет самый большой приоритет после '$'
  vector<int64_t> letter_count(size_alphabet);
  for (const auto letter : str_) {
    if (letter != '$' && letter != '#') {
      ++letter_count[letter - 'a'];
    }
  }
  for (int64_t i = 1; i < size_alphabet; ++i) {
    letter_count[i] += letter_count[i - 1];
  }
  suff_arr_[0] = str_size_ - 1; // так как '$' имеет самый большой приоритет
  suff_arr_[1] = a_size_; // так как '#' имеет самый большой приоритет после '$'
  // проходимся по строке, увеличивая приортет этой
  // же буквы двигаясь ближе к началу слова
  for (int64_t i = str_size_ - 2; i > a_size_; --i) {
    --letter_count[str_[i] - 'a'];
    suff_arr_[letter_count[str_[i] - 'a'] + 2] = i;
  }
  for (int64_t i = a_size_ - 1; i >= 0; --i) {
    --letter_count[str_[i] - 'a'];
    suff_arr_[letter_count[str_[i] - 'a'] + 2] = i;
  }
  // теперь и востановили начальный suff_arr
  int classes_count = 1;
  vector<int64_t> equivalence_class(str_size_);
  for (int64_t i = 1; i < str_size_; ++i) {
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
    vector<int64_t> equivalence_class) {
  // будем сортировать по степеням двойки, пока длина подстроки не привысит
  // str_size
  int64_t classes_count = 0;
  for (const auto class_ : equivalence_class) {
    classes_count = std::max(classes_count, class_);
  }
  ++classes_count;
  for (int64_t degree = 0; (1 << degree) < str_size_; ++degree) {
    vector<int64_t> offset_arr(
        str_size_); // сдвинутый по модулю суффиксный массив на 2 ^ k - 1
    for (int64_t i = 0; i < str_size_; ++i) {
      offset_arr[i] = suff_arr_[i] - (1 << degree);
      if (offset_arr[i] < 0) {
        offset_arr[i] += str_size_;
      }
    }
    // тк suff_arr_ уже отсортирован по первым 2 ^ (k - 1) элементам,
    // то теперь сортировка зависит только от следующих 2 ^ (k - 1) элементов,
    // то есть от сортирвки массива offset
    vector<int64_t> count_elements_in_class(
        classes_count); // храним колво элементов в каждом классе
    // эквивалентности
    for (const auto element : offset_arr) {
      ++count_elements_in_class[equivalence_class[element]];
    }
    for (int64_t i = 1; i < classes_count; ++i) {
      count_elements_in_class[i] += count_elements_in_class[i - 1];
    }
    for (int64_t i = str_size_ - 1; i >= 0; --i) {
      const int64_t tmp = offset_arr[i];
      --count_elements_in_class[equivalence_class[tmp]];
      suff_arr_[count_elements_in_class[equivalence_class[tmp]]] = tmp;
    }
    // теперь восстанавливаем массив классов эквивалетности
    vector<int64_t> tmp_equivalence_class(str_size_);
    tmp_equivalence_class[suff_arr_[0]] = 0;
    classes_count = 1;
    for (int64_t i = 1; i < str_size_; ++i) {
      const int64_t first_begin = suff_arr_[i];
      const int64_t second_begin = suff_arr_[i - 1];
      const int64_t first_end = (suff_arr_[i] + (1 << degree)) % str_size_;
      const int64_t second_end = (suff_arr_[i - 1] + (1 << degree)) % str_size_;
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
vector<int64_t> SuffixArrayAndLcpBuilder::ConstructReverseArr() const {
  vector<int64_t> reverse_suff_arr(str_size_);
  for (int64_t i = 0; i < str_size_; ++i) {
    reverse_suff_arr[suff_arr_[i]] = i;
  }
  return reverse_suff_arr;
}

// построение lcp
void SuffixArrayAndLcpBuilder::ConstructLCP() {
  const auto reverse_suff_arr = ConstructReverseArr();
  int64_t shift = 0;
  lcp_[str_size_ - 1] = -1; // тк у него нет следующего за ним идущего
  for (int64_t i = 0; i < str_size_; ++i) {
    if (reverse_suff_arr[i] == str_size_ - 1) {
      shift = 0;
      continue;
    }
    if (shift >= 1) {
      --shift;
    }
    // если lcp[reverse_suff_arr[i]] > 1, то lcp[reverse_suff_arr[i]] - 1 <=
    // lcp[reverse_suff_arr[i + 1]] пользуемся этим для вычисления следующего
    // lcp, храня shift
    const int64_t next = suff_arr_[reverse_suff_arr[i] + 1];
    while (std::max(next, i) + shift < str_size_ &&
           str_[next + shift] == str_[i + shift]) {
      ++shift;
    }
    lcp_[reverse_suff_arr[i]] = shift;
  }
}

// поиск к-той порядковой ститистики для 2 строк
void SuffixArrayAndLcpBuilder::SearchKStatistic(int64_t k) const {
  int64_t counter = 0; // считает сколькло общих подстрок в лексиграфическом
  // порядке мы обработали
  int64_t min_lcp = 0; // хотим хранить LCP, какой именно опишу ниже
  for (int64_t i = 2; i < str_size_ - 1; ++i) {
    min_lcp = std::min(min_lcp, lcp_[i]); // LCP(a, b) = min(LCP(i, i + 1)), где
    // i принадлежит [a ... b - 1]
    // проверяем то, что 2 соседних суффикса в суффиксном
    // массиве лежат по разные стороны от '#'.
    if ((suff_arr_[i] < a_size_ && suff_arr_[i + 1] > a_size_) ||
        (suff_arr_[i] > a_size_ && suff_arr_[i + 1] < a_size_)) {
      // тогда заметим, что мы обработали lcp_[i] новых подстрок(в
      // лексиграфическом порядке), являющихся префиксами suff_arr_[i] и
      // suff_arr_[i + 1], то есть эти строки лежат в разных словах. Но при этом
      // мы посчитали несколько лишних подстрок, а именно LCP(i, j), где j -
      // предыдущее состояние, в котором мы увеличили counter. LCP(i, j)
      // считается как минимум lcp_ между ними, он равен min_lcp. Вычитаем его
      counter += lcp_[i] - min_lcp;
      min_lcp = lcp_[i]; // обновляем новый минимум
      if (counter >= k) {
        // если мы пересекли counter, то наша k-тая статистика лежит где-то в
        // новой добавленной порции. тк приортет 'ax' ниже чем просто 'a',
        // поэтому длина нашего подслова будел lcp[i] - counter + k
        string result;
        for (int64_t result_letter = 0; result_letter < lcp_[i] - counter + k;
             ++result_letter) {
          result += str_[suff_arr_[i] + result_letter];
        }
        cout << result << '\n';
        return;
      }
    }
  }
  // выводим при условии, что counter всегда < k
  cout << -1 << '\n';
  return;
}

int main() {
  string str_a, str_b;
  cin >> str_a >> str_b;
  int64_t k;
  cin >> k;
  int64_t a_size = str_a.size(); // передаем в качестве аргумента, чтобы можно
  // было строки передать c помощью move
  const SuffixArrayAndLcpBuilder my_class(std::move(str_a), std::move(str_b),
                                          a_size);
  my_class.SearchKStatistic(k);
}
