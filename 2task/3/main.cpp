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

class SuffixArrayBuilder {
public:
  SuffixArrayBuilder(string a_, string b_, int64_t a_size_);

  void SearchKStatistic(int64_t k) const;

private:
  string str;
  int64_t a_size; // длина первой строки

  int64_t str_size;
  int64_t classes_count; // колво классов эквивалентности
  vector<int64_t> suff_arr; // суффиксный массив
  vector<int64_t> reverse_suff_arr; // обратный к суффиксному массиву
  vector<int64_t> equivalence_class; // массив классов эквивалентности
  vector<int64_t> lcp;

  void InitSuffixArray(); // построение суффиксного массива для
  // подстрок длины 1

  void UpdateSuffixArray(); // построение суффиксного массива

  void ConstructLCP(); // построение lcp

  void ConstructReverseArr(); // построение обратного суффиксного массива
};

SuffixArrayBuilder::SuffixArrayBuilder(string a_, string b_, int64_t a_size_)
    : a_size(a_size_), str(std::move(a_) + '$' + std::move(b_) + '#'),
      str_size(str.size()), classes_count(0),
      suff_arr(vector<int64_t>(str_size)),
      equivalence_class(vector<int64_t>(str_size)),
      reverse_suff_arr(vector<int64_t>(str_size)),
      lcp(vector<int64_t>(str_size)) {
  InitSuffixArray(); // строим начальный вспомогательный суффиксный
  // массив
  UpdateSuffixArray(); // строим суффиксный массив рекуррентно
  ConstructLCP(); // строим lcp
}

// должны по сути отсортировать буквы в массиве
// будем использовать сортировку подсчетом
void SuffixArrayBuilder::InitSuffixArray() {
  suff_arr[str_size - 1] = 0; // так как '$' имеет самый большой приоритет
  suff_arr[a_size] = 1; // так как '#' имеет самый большой приоритет после '$'
  vector<int64_t> letter_count(size_alphabet);
  for (const auto letter : str) {
    if (letter != '$' && letter != '#') {
      ++letter_count[letter - 'a'];
    }
  }
  for (int64_t i = 1; i < size_alphabet; ++i) {
    letter_count[i] += letter_count[i - 1];
  }
  suff_arr[0] = str_size - 1; // так как '$' имеет самый большой приоритет
  suff_arr[1] = a_size; // так как '#' имеет самый большой приоритет после '$'
  // проходимся по строке, увеличивая приортет этой
  // же буквы двигаясь ближе к началу слова
  for (int64_t i = str_size - 2; i > a_size; --i) {
    --letter_count[str[i] - 'a'];
    suff_arr[letter_count[str[i] - 'a'] + 2] = i;
  }
  for (int64_t i = a_size - 1; i >= 0; --i) {
    --letter_count[str[i] - 'a'];
    suff_arr[letter_count[str[i] - 'a'] + 2] = i;
  }
  // теперь и востановили начальный suff_arr
  classes_count = 1;
  for (int64_t i = 1; i < str_size; ++i) {
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
  for (int64_t degree = 0; (1 << degree) < str_size; ++degree) {
    vector<int64_t> offset_arr(
        str_size); // сдвинутый по модулю суффиксный массив на 2 ^ k - 1
    for (int64_t i = 0; i < str_size; ++i) {
      offset_arr[i] = suff_arr[i] - (1 << degree);
      if (offset_arr[i] < 0) {
        offset_arr[i] += str_size;
      }
    }
    // тк suff_arr уже отсортирован по первым 2 ^ (k - 1) элементам,
    // то теперь сортировка зависит только от следующих 2 ^ (k - 1) элементов,
    // тоесть от сортирвки массива offset
    vector<int64_t> count_elements_in_class(
        classes_count); // храним колво элементов в каждом классе
    // эквивалентности
    for (const auto i : equivalence_class) {
      ++count_elements_in_class[i];
    }
    for (int64_t i = 1; i < classes_count; ++i) {
      count_elements_in_class[i] += count_elements_in_class[i - 1];
    }
    for (int64_t i = str_size - 1; i >= 0; --i) {
      const int64_t tmp = offset_arr[i];
      --count_elements_in_class[equivalence_class[tmp]];
      suff_arr[count_elements_in_class[equivalence_class[tmp]]] = tmp;
    }
    // теперь восстанавливаем массив классов эквивалетности
    vector<int64_t> tmp_equivalence_class(str_size);
    tmp_equivalence_class[suff_arr[0]] = 0;
    classes_count = 1;
    for (int64_t i = 1; i < str_size; ++i) {
      int64_t first_begin = suff_arr[i];
      int64_t second_begin = suff_arr[i - 1];
      int64_t first_end = (suff_arr[i] + (1 << degree)) % str_size;
      int64_t second_end = (suff_arr[i - 1] + (1 << degree)) % str_size;
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
  for (int64_t i = 0; i < str_size; ++i) {
    reverse_suff_arr[suff_arr[i]] = i;
  }
}

// построение lcp
void SuffixArrayBuilder::ConstructLCP() {
  ConstructReverseArr();
  int64_t shift = 0;
  lcp[str_size - 1] = -1; // тк у него нет следующего за ним идущего
  for (int64_t i = 0; i < str_size; ++i) {
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
    int64_t next = suff_arr[reverse_suff_arr[i] + 1];
    while (std::max(next, i) + shift < str_size &&
           str[next + shift] == str[i + shift]) {
      ++shift;
    }
    lcp[reverse_suff_arr[i]] = shift;
  }
}

// поиск к-той порядковой ститистики для 2 строк
void SuffixArrayBuilder::SearchKStatistic(int64_t k) const {
  int64_t counter = 0; // считает сколькло общих подстрок в лексиграфическом
  // порядке мы обработали
  int64_t min_lcp = 0; // хотим хранить LCP, какой именно опишу ниже
  for (int64_t i = 2; i < str_size - 1; ++i) {
    min_lcp = std::min(min_lcp, lcp[i]); // lCP(a, b) = min(LCP(i, i + 1)), где
    // i пренадлежит [a ... b - 1]
    // проверяем то, что 2 соседних суффикса в суффиксном
    // массиве лежат по разные стороны от '#'.
    if ((suff_arr[i] < a_size && suff_arr[i + 1] > a_size) ||
        (suff_arr[i] > a_size && suff_arr[i + 1] < a_size)) {
      // тогда заметим, что мы обработали lcp[i] новых подстрок(в
      // лексиграфическом порядке), являющихся префиксами suff_arr[i] и
      // suff_arr[i + 1], тоесть эти строки лежат в разных словах. Но при этом
      // мы посчитали несколько лишних подстрок, а именно LCP(i, j), где j -
      // предидущее состояние, в котором мы увеличили counter. LCP(i, j)
      // считается как минимум lcp между ними, он равен min_lcp. Вычитаем его
      counter += lcp[i] - min_lcp;
      min_lcp = lcp[i]; // обновляем новый минимум
      if (counter >= k) {
        // если мы пересекли counter, то наша k-тая статистика лежит где-то в
        // новой добавленной порции. тк приортет 'ax' ниже чем просто 'a',
        // поэтому длина нашего подслова будел lcp[i] - counter + k
        string result;
        for (int64_t result_letter = 0; result_letter < lcp[i] - counter + k;
             ++result_letter) {
          result += str[suff_arr[i] + result_letter];
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
  SuffixArrayBuilder my_class(std::move(str_a), std::move(str_b), a_size);
  my_class.SearchKStatistic(k);
}
