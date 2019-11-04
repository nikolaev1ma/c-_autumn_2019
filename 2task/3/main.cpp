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

class ProcessingStr {
public:
  ProcessingStr(string a_, string b_, long long k_, long long a_size_);

  void SearchKStatistic() const;

private:
  string str;
  long long k;      // к статистика
  long long a_size; // длина первой строки

  long long str_size;
  long long classes_count; // колво классов эквивалентности
  vector<long long> suff_arr; // суффиксный массив
  vector<long long> reverse_suff_arr; // обратный к суффиксному массиву
  vector<long long> equivalence_class; // массив классов эквивалентности
  vector<long long> lcp;

  void InitializationSupportArr(); // построение суффиксного массива для
  // подстрок длины 1

  void UpdateSupportArr(); // построение суффиксного массива

  void ConstructLCP(); // построение lcp

  void ConstructReverseArr(); // построение обратного суффиксного массива
};

ProcessingStr::ProcessingStr(string a_, string b_, long long k_,
                             long long a_size_)
    : k(k_), a_size(a_size_), str(std::move(a_) + '$' + std::move(b_) + '#'),
      str_size(str.size()), classes_count(0),
      suff_arr(vector<long long>(str_size)),
      equivalence_class(vector<long long>(str_size)),
      reverse_suff_arr(vector<long long>(str_size)),
      lcp(vector<long long>(str_size)) {
  InitializationSupportArr(); // строим начальный вспомогательный суффиксный
  // массив
  UpdateSupportArr(); // строим суффиксный массив рекуррентно
  ConstructLCP(); // строим lcp
}

// должны по сути отсортировать буквы в массиве
// будем использовать сортировку подсчетом
void ProcessingStr::InitializationSupportArr() {
  suff_arr[str_size - 1] = 0; // так как '$' имеет самый большой приоритет
  suff_arr[a_size] = 1; // так как '#' имеет самый большой приоритет после '$'
  vector<long long> letter_count(size_alphabet);
  for (auto letter : str) {
    if (letter != '$' && letter != '#') {
      ++letter_count[letter - 'a'];
    }
  }
  for (long long i = 1; i < size_alphabet; ++i) {
    letter_count[i] += letter_count[i - 1];
  }
  suff_arr[0] = str_size - 1; // так как '$' имеет самый большой приоритет
  suff_arr[1] = a_size; // так как '#' имеет самый большой приоритет после '$'
                        // проходимся по строке, увеличивая приортет этой
                        // же буквы двигаясь ближе к началу слова
  for (long long i = str_size - 2; i > a_size; --i) {
    --letter_count[str[i] - 'a'];
    suff_arr[letter_count[str[i] - 'a'] + 2] = i;
  }
  for (long long i = a_size - 1; i >= 0; --i) {
    --letter_count[str[i] - 'a'];
    suff_arr[letter_count[str[i] - 'a'] + 2] = i;
  }
  // теперь и востановили начальный suff_arr
  classes_count = 1;
  for (long long i = 1; i < str_size; ++i) {
    // условие того, что начинается новый класс эквивалентности
    if (str[suff_arr[i]] != str[suff_arr[i - 1]]) {
      ++classes_count;
    }
    equivalence_class[suff_arr[i]] = classes_count - 1;
  }
}

// Должны отсортировать циклические подстроки длины 2 ^ k опираясь на сортировку
// циклических подстрок длины 2 ^ (k - 1)
void ProcessingStr::UpdateSupportArr() {
  // будем сортировать по степеням двойки, пока длина подстроки не привысит
  // str_size
  for (long long degree = 0; (1 << degree) < str_size; ++degree) {
    vector<long long> offset_arr(
        str_size); // сдвинутый по модулю суффиксный массив на 2 ^ k - 1
    for (long long i = 0; i < str_size; ++i) {
      offset_arr[i] = suff_arr[i] - (1 << degree);
      if (offset_arr[i] < 0) {
        offset_arr[i] += str_size;
      }
    }
    // тк suff_arr уже отсортирован по первым 2 ^ (k - 1) элементам,
    // то теперь сортировка зависит только от следующих 2 ^ (k - 1) элементов,
    // тоесть от сортирвки массива offset
    vector<long long> count_elements_in_class(
        classes_count); // храним колво элементов в каждом классе
                        // эквивалентности
    for (auto i : offset_arr) {
      ++count_elements_in_class[equivalence_class[i]];
    }
    for (long long i = 1; i < classes_count; ++i) {
      count_elements_in_class[i] += count_elements_in_class[i - 1];
    }
    for (long long i = str_size - 1; i >= 0; --i) {
      long long tmp = offset_arr[i];
      --count_elements_in_class[equivalence_class[tmp]];
      suff_arr[count_elements_in_class[equivalence_class[tmp]]] = tmp;
    }
    // теперь восстанавливаем массив классов эквивалетности
    vector<long long> tmp_equivalence_class(str_size);
    tmp_equivalence_class[suff_arr[0]] = 0;
    classes_count = 1;
    for (long long i = 1; i < str_size; ++i) {
      long long first_begin = suff_arr[i];
      long long second_begin = suff_arr[i - 1];
      long long first_end = (suff_arr[i] + (1 << degree)) % str_size;
      long long second_end = (suff_arr[i - 1] + (1 << degree)) % str_size;
      // число классов увеличивается только при условии, что соседние строки не
      // совпадают, тоесть что текущее значение и сдвинутое на 2 ^ (k - 1) в
      // предыдущем классе эквивалентности различны
      if (equivalence_class[first_begin] != equivalence_class[second_begin] ||
          equivalence_class[first_end] != equivalence_class[second_end]) {
        ++classes_count;
      }
      tmp_equivalence_class[suff_arr[i]] = classes_count - 1;
    }
    equivalence_class = tmp_equivalence_class;
  }
}

// разворачиваем суффиксный массив
void ProcessingStr::ConstructReverseArr() {
  for (long long i = 0; i < str_size; ++i) {
    reverse_suff_arr[suff_arr[i]] = i;
  }
}

// построение lcp
void ProcessingStr::ConstructLCP() {
  ConstructReverseArr();
  long long shift = 0;
  lcp[str_size - 1] = -1; // тк у него нет следующего за ним идущего
  for (long long i = 0; i < str_size; ++i) {
    if (reverse_suff_arr[i] == str_size - 1) {
      shift = 0;
    } else {
      if (shift >= 1) {
        --shift;
      }
      // если lcp[reverse_suff_arr[i]] > 1, то lcp[reverse_suff_arr[i]] - 1 <=
      // lcp[reverse_suff_arr[i + 1]] пользуемся этим для вычисления следующего
      // lcp, храня shift
      long long next = suff_arr[reverse_suff_arr[i] + 1];
      while (next + shift < str_size && i + shift < str_size &&
             str[next + shift] == str[i + shift]) {
        ++shift;
      }
      lcp[reverse_suff_arr[i]] = shift;
    }
  }
}

// поиск к-той порядковой ститистики для 2 строк
void ProcessingStr::SearchKStatistic() const {
  long long counter = 0; // считает сколькло общих подстрок в лексиграфическом
                         // порядке мы обработали
  long long min_lcp = 0; // хотим хранить LCP, какой именно опишу ниже
  for (long long i = 2; i < str_size - 1; ++i) {
    min_lcp = std::min(
        min_lcp, lcp[i]); // lCP(a, b) = min(LCP(i, i + 1)), где
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
        for (long long result_letter = 0; result_letter < lcp[i] - counter + k;
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
  long long k;
  cin >> k;
  long long a_size = str_a.size(); // передаем в качестве аргумента, чтобы можно
                                   // было строки передать c помощью move
  ProcessingStr my_class(std::move(str_a), std::move(str_b), k, a_size);
  my_class.SearchKStatistic();
}
