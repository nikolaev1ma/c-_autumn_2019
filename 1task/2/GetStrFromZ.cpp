#include "ProcessingStr.h"

typedef vector <vector<bool>> vectorZT;

class GetStrByZ {
public:
  GetStrByZ(vector<int> z_) : z(std::move(z_)), right(1) {
    // изначально любая буква может стоять на любой позиции
    can_use.resize(z.size());
    for (int i = 0; i < z.size(); ++i) {
      for (char letter = 'a'; letter <= 'z'; ++letter) {
        can_use[i].emplace_back(true);
      }
    }
    Recovery(); // востанавливаем строку
  }

  //вычисляем букву на n позиции
  void AddLetter(int n) {
    // если z функция в этой точке не равна 0, то мы можем точно востановить
    // букву
    if (z[n] != 0) {
      // заполняем однозначно строку
      for (int i = right; i < n + z[n]; ++i) {
        substr += substr[i - n];
      }
      // теперь самое правое обновленное значение моло измениться
      right = std::max(n + z[n], right);
      // звметим, что на месте right теперь не может стоять буква, что и на
      // right - n, тк если бы они были бы рпавны, то z[n] была бы больше
      can_use[right][substr[right - n] - 'a'] = false;
      return;
    }
    if (right > n) {
      return;
    }
    // z[n] == 0
    // ставим первую букву на место n, которую можем поставить
    for (char letter = 'b'; letter <= 'z'; ++letter) {
      if (can_use[n][letter - 'a']) {
        substr += letter;
        ++right;
        return;
      }
    }
  }

  // заполняем строку слева направо
  void Recovery() {
    if (z.empty()) {
      return;
    }
    substr = 'a';
    for (int i = 1; i < z.size(); ++i) {
      AddLetter(i);
    }
  }

  string Str() { return substr; }

private:
  vector<int> z; // z-функция
  string substr;
  int right; // самая правая обработанная позиция
  vectorZT can_use; // для пары [буква, позиция] определяем, может ли
  // исполбзоваться эта буква на этой позиции
};

void ProcessingStr::GetStrFromZ() {
  if (initialized_str) {
    return;
  }
  initialized_str = true;
  GetStrByZ auxiliary_class(std::move(z)); // вспомогательный класс
  str = auxiliary_class.Str();             // присваиваем

}
