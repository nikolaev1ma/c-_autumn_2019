#include "String.h"

typedef map<std::pair<int, char>, bool> mapZT;

class GetStrByZ {
public:
  GetStrByZ(const vector<int> z_) : z(z_), right(1) {
    // изначально любая буква может стоять на любой позиции
    for (int i = 0; i < z.size(); ++i) {
      for (char letter = 'a'; letter <= 'z'; ++letter) {
        can_use[{i, letter}] = true;
      }
    }
  }

  //вычисляем букву на n позиции
  void AddLetter(int n) {
    //если z функция в этой точке не равна 0, то мы можем точно востановить
    //букву
    if (z[n] != 0) {
      //заполняем однозначно строку
      for (int i = right; i < n + z[n]; ++i) {
        str += str[i - n];
      }
      // теперь самое правое обновленное значение моло измениться
      right = std::max(n + z[n], right);
      // звметим, что на месте right теперь не может стоять буква, что и на
      // right - n, тк если бы они были бы рпавны, то z[n] была бы больше
      can_use[{right, str[right - n]}] = false;
      return;
    }
    if (right > n) {
      return;
    }
    // z[n] == 0
    //ставим первую букву на место n, которуб можем поставить
    for (char letter = 'b'; letter <= 'z'; ++letter) {
      if (can_use[{n, letter}] == true) {
        str += letter;
        ++right;
        return;
      }
    }
  }

  //заполняем строку слева направо
  void Recovery() {
    if (z.size() == 0) {
      return;
    }
    str = 'a';
    for (int i = 1; i < z.size(); ++i) {
      AddLetter(i);
    }
  }

  string Str() { return str; }

private:
  vector<int> z; // z-функция
  string str;
  int right; // самая правая обработанная позиция
  mapZT can_use; // для пары [буква, позиция] определяем, может ли
                 // исполбзоваться эта буква на этой позиции
};

void String::get_str_by_z() {
  GetStrByZ auxiliary_class(z); //вспомогательный класс
  auxiliary_class.Recovery();   // востанавливаем строку
  str = auxiliary_class.Str();  // присваиваем
}
