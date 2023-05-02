#include "1laba.h"
#include <sstream>
#include <cstring>
#include <stdexcept>
namespace arbitary_precision {

    namespace {
        template <typename T>
        T fromString(const std::string& s) {
            std::istringstream iss(s);
            T res;
            iss >> res;
            return res;
        }
    }


    //Реализация метода ToString
    string Integer::ToString() {
        string out;
        string tmp;
        if (this->_negative)
            out += '-';

        out += std::to_string(_digits[_digits.size() - 1]);
        for (int64_t i = _digits.size() - 2; i >= 0; --i) {
            tmp = std::to_string(_digits[i]);
            while (tmp.size() != 9)
                tmp = '0' + tmp;
            out += tmp;
        }
        return out;
    }

    //Метод удаления лишних нулей.
    void Integer::RemoveLeadingZeroes() {
        while (_digits[_digits.size() - 1] == 0 && _digits.size() > 1)
            _digits.pop_back();
    }


    //Метод, нормализирующий i-ое число.
    void Integer::Normalize(Integer& integer) {
        int64_t carry;
        for (size_t i = 0; i < integer._digits.size() - 1; ++i) {
            if (integer._digits[i] > Integer::BASE) {
                carry = integer._digits[i] / Integer::BASE;

                integer._digits[i + 1] += carry;
                integer._digits[i] -= carry * Integer::BASE;
            }
        }
    }

    //-----Constructors-----

    //Реализация конструктора, принимающего строку. Проверка на пустоту str, также проверка наличия
    //минуса, для того чтобы изменить параметр _negative на true, substr - вырезаем число от pos 1
    //потом загоняем каждые последующие 9 цифр числа в _digits[i]
    Integer::Integer(string str) {
        if (str.empty()) {
            _digits = {0};
            _negative = false;
            return;
        }
        if (str[0] == '-') {
            _negative = !OnlyZeroes(str.substr(1));
            str = str.substr(1);
        }
        else
            _negative = false;

        for (int64_t i = str.size(); i > 0; i -= 9) {
            if (i < 9)
                _digits.push_back(fromString<int32_t>(str.substr(0, i)));
            else
                _digits.push_back(fromString<int32_t>(str.substr(i - 9, 9)));
        }
        RemoveLeadingZeroes();
    }

    //Конструктор копирования
    Integer::Integer(const Integer& anotherBig) {
        _digits = anotherBig._digits;
        _negative = anotherBig._negative;
    }

    //Метод проверки строки на нули.
    bool Integer::OnlyZeroes(const string& str) {
        for (auto a : str)
            if (a != '0')
                return false;
        return true;
    }

    //Метод проверки, если ли в _digits вообще какое то число
    bool Integer::IsZero() const {
        if (_digits.size() == 1 && _digits[0] == 0)
            return true;
        return false;
    }

    // --- Binary operations ---

    //Перегрузка оператора присваивания. default значит происходит обычное копирование данных.
    Integer& Integer::operator=(const Integer& integer) = default;


    //Перегрузка оператор сложения с другим integer. Проверяем является ли какое то из чисел отрицательным,
    //Если оба отриц, то меняем знак каждого числа и складываем их,
    //Если одно из них отриц., то мы вычетаем его из большего числа.
    //Если оба положительны, то складываем их цифры по порядку и сохраняем резултат в новом объекте
    //integer. Если в какой то момент бы выходим за BASE, то делаем делаем перенос разрядов.
    //В конце удаляем ведущие нули.
    Integer Integer::operator+(const Integer& integer) const {
        if (_negative) {
            if (integer._negative)
                return -(-(*this) + (-integer));
            else
                return integer - (-*this);
        }
        if (integer._negative)
            return *this - (-integer);

        Integer new_integer;
        size_t l = 0;
        size_t r = 0;

        while (l < _digits.size() && r < integer._digits.size()) {
            while (new_integer._digits.size() <= l + 1)
                new_integer._digits.push_back(0);
            new_integer._digits[l] += (_digits[l] + integer._digits[r]) % Integer::BASE;
            new_integer._digits[l + 1] += (_digits[l] + integer._digits[r]) / Integer::BASE;
            ++l, ++r;
        }

        while (l < _digits.size()) {
            while (new_integer._digits.size() <= l + 1)
                new_integer._digits.push_back(0);

            new_integer._digits[l] += (_digits[l]) % Integer::BASE;
            new_integer._digits[l + 1] += (_digits[l]) / Integer::BASE;
            ++l;
        }

        while (r < integer._digits.size()) {
            while (new_integer._digits.size() <= r + 1)
                new_integer._digits.push_back(0);

            new_integer._digits[r] += (integer._digits[r]) % Integer::BASE;
            new_integer._digits[r + 1] += (integer._digits[r]) / Integer::BASE;
            ++r;
        }
        new_integer.RemoveLeadingZeroes();
        return new_integer;
    }

    //Перегрузка оператора вычитания. В начале проверяем отрицательные ли какие то из объектов,
    //Если да, то возвращаем суммы с соответствующими отрицаниями соответствующих чисел.
    //Если же оба положительные то заводим new_integer
    //Затем происходит поэлементное вычитание
    //При вычитании из цифры старшего разряда вычитается также значение переноса.
    //Если результат меньше 0, то значение переноса устанавливается в 1,
    //и к вычитаемой цифре прибавляется значение основания системы счисления (BASE).
    // Затем метод удаляет незначащие нули из результата и возвращает его.
    Integer Integer::operator-(const Integer& integer) const {
        if (integer._negative)
            return ((*this) + (-integer));
        else if (_negative)
            return -(-(*this) + integer);
        else if ((*this) < integer)
            return -(integer - (*this));

        Integer new_integer(*this);

        int carry = 0;
        for (size_t i = 0; i < integer._digits.size() || carry != 0; ++i) {
            new_integer._digits[i] -= carry + (i < integer._digits.size() ? integer._digits[i] : 0);
            carry = new_integer._digits[i] < 0;
            if (carry != 0) new_integer._digits[i] += Integer::BASE;
        }

        new_integer.RemoveLeadingZeroes();
        return new_integer;
    }

    //Перегрузка оператора +=. Возвращаем *this + integer.
    Integer& Integer::operator+=(const Integer& integer) {
        return *this = (*this + integer);
    }

    //Перегрузка оператора -=.
    Integer& Integer::operator-=(const Integer& integer) {
        return *this = (*this - integer);
    }

    //Перегрузка оператора умножения. Если _digits.size() == 1 то производим обычное умножение,
    //иначе по алгоритму карацубы, делаем разделения, вычисляем соответствующие произведения, суммы и делаем сдвиги
    //для получения ответа.
    Integer Integer::operator*(const Integer& integer) const {
        Integer product;
        product._digits.resize(_digits.size() + integer._digits.size());


        //default multiply
        if (_digits.size() < Integer::LENGTH_FOR_KARATSUBA) {
            std::fill(product._digits.begin(), product._digits.end(), 0);
            for (size_t i = 0; i < _digits.size(); ++i) {
                for (size_t j = 0; j < integer._digits.size(); ++j) {
                    product._digits[i + j] += _digits[i] * integer._digits[j];
                }
            }

        }
        // Karatsuba's algorithm
        else {
            Integer a_part1;
            a_part1._digits = _digits;
            a_part1._digits.resize((_digits.size() + 1) / 2);

            Integer a_part2;
            a_part2._digits = vector<int64_t>(&_digits[a_part1._digits.size()], &_digits[_digits.size()]);

            Integer b_part1;
            b_part1._digits = integer._digits;
            b_part1._digits.resize((integer._digits.size() + 1) / 2);

            Integer b_part2;
            b_part2._digits = vector<int64_t>(&integer._digits[b_part1._digits.size()], &integer._digits[integer._digits.size()]);


            Integer sum_of_a_parts = a_part1 + a_part2;
            Normalize(sum_of_a_parts);
            Integer sum_of_b_parts = b_part1 + b_part2;
            Normalize(sum_of_b_parts);

            Integer product_of_sums = sum_of_a_parts * sum_of_b_parts;

            Integer product_of_first_parts = a_part1 * b_part1;
            Integer product_of_second_parts = a_part2 * b_part2;

            Integer sum_of_middle_terms = (product_of_sums - product_of_first_parts) - product_of_second_parts;

            for (size_t i = 0; i < product_of_first_parts._digits.size(); ++i)
                product._digits[i] = product_of_first_parts._digits[i];

            //    memcpy(&product._digits, &product_of_first_parts._digits,
            //           product_of_first_parts._digits.size() * sizeof(int64_t));

            for (size_t i = product_of_first_parts._digits.size(); i - product_of_first_parts._digits.size() < product_of_second_parts._digits.size(); ++i)
                product._digits[i] = product_of_second_parts._digits[i - product_of_first_parts._digits.size()];
            //    memcpy(&product._digits + product_of_first_parts._digits.size(),
            //           &product_of_second_parts._digits,
            //           product_of_second_parts._digits.size() * sizeof(int64_t));

            for (size_t i = 0; i < sum_of_middle_terms._digits.size(); ++i)
                product._digits[a_part1._digits.size() + i] +=
                sum_of_middle_terms._digits[i];
        }

        Integer::Normalize(product);
        product.RemoveLeadingZeroes();
        product._negative = _negative != integer._negative;
        return product;

    }

    //Перегрузка оператора/. Сначала проверяем равен ли integer 0, если да,
    //то генерируем исключение.
    //Затем создается локальная копия делителя b и ей присваивается абсолютное значение делителя.
    //Также создается объект result, который будет хранить результат операции, и объект current,
    //который будет хранить текущее число, которое мы будем делить.
    //Затем происходит цикл,
    //в котором мы перебираем цифры делимого this справа налево и выполняем деление на делитель b в столбик.
    //На каждом шаге мы сдвигаем current на один разряд вправо, добавляя к нему следующую цифру делимого.
    //Затем мы бинарным поиском находим максимальное число x, умножив которое на делитель b не превышает current,
    //и записываем его в соответствующий разряд результата result.
    //Затем мы вычитаем из current произведение b на x, чтобы получить остаток от деления.

    Integer Integer::operator/(const Integer& integer) const {
        if (integer == 0) throw std::runtime_error("Math error: Attempted to divide by Zero\n");
        Integer b = integer;
        b._negative = false;
        Integer result;
        Integer current;
        result._digits.resize(_digits.size());
        for (int32_t i = static_cast<int32_t>(_digits.size()) - 1; i >= 0; --i) {
            current.ShiftRight();
            current._digits[0] = _digits[i];
            current.RemoveLeadingZeroes();
            int x = 0, l = 0, r = Integer::BASE;
            while (l <= r) {
                int m = (l + r) / 2;
                Integer t = b * m;
                if (t <= current) {
                    x = m;
                    l = m + 1;
                }
                else r = m - 1;
            }

            result._digits[i] = x;
            current = current - b * x;
        }

        result._negative = (_negative != integer._negative) && !result.IsZero();
        result.RemoveLeadingZeroes();
        return result;
    }


    //Перегрузка оператора взятия по остатку. Сначала делим число на integer. Потом полученное значение умножаем на само
    //число integer. И в итоге от нашего числа отнимаем полученное значение.
    //Если результат отрицательный, то добавляем делитель чтобы получить положительный результат.
    Integer Integer::operator%(const Integer& integer) const {
        Integer result = (*this) - ((*this) / integer) * integer;
        if (result._negative)
            result += integer;
        return result;
    }

    // --- Comparators ---
    //Перегрузка операторов сравнения.
    bool Integer::operator==(const Integer& integer) const {
        if (_negative != integer._negative)
            return false;

        if (_digits.size() != integer._digits.size())
            return false;

        for (size_t i = 0; i < _digits.size(); ++i)
            if (_digits[i] != integer._digits[i])
                return false;

        return true;
    }

    bool Integer::operator!=(const Integer& integer) const { return !(*this == integer); }

    bool Integer::operator<(const Integer& integer) const {
        if (*this == integer)
            return false;
        if (_negative) {
            if (integer._negative) {
                return (-(*this)) < (-integer);
            }
            else
                return true;
        }
        else if (integer._negative)
            return false;


        if (_digits.size() != integer._digits.size()) {
            return _digits.size() < integer._digits.size();
        }

        for (int32_t i = _digits.size() - 1; i != -1; --i) {
            if (_digits[i] != integer._digits[i])
                return _digits[i] < integer._digits[i];
        }
        return false;
    }

    //Перегрузка бинарных операторов.
    Integer& Integer::operator*=(const Integer& integer) {
        return *this = (*this * integer);
    }
    Integer& Integer::operator/=(const Integer& integer) {
        return *this = (*this / integer);
    }
    Integer& Integer::operator%=(const Integer& integer) {
        return *this = ((*this) % integer);
    }

    //Перегрузка компараторов оставшихся
    bool Integer::operator<=(const Integer& integer) const {
        return (*this) < integer || (*this) == integer;
    }

    bool Integer::operator>(const Integer& integer) const {
        return !((*this) <= integer);
    }

    bool Integer::operator>=(const Integer& integer) const {
        return !((*this) < integer);
    }

    // --- Unary operations ---
    //Перегрузка унарных операторов.

    //Перегрузка унарного минуса.
    Integer Integer::operator-() const {
        Integer copy(*this);
        if (!IsZero())
            copy._negative = !_negative;
        return copy;
    }

    //Унарный плюс
    Integer Integer::operator+() const {
        return Integer(*this);
    }

    //Унарные --, уменьшает значение на 1
    Integer& Integer::operator--() {
        return *this -= 1;
    }

    Integer Integer::operator--(int) {
        *this -= 1;
        return *this + 1;
    }
    //Унарный ++, увеличивает значение.
    Integer& Integer::operator++() {
        return *this += 1;
    }

    Integer Integer::operator++(int) {
        *this += 1;
        return *this - 1;
    }

    //Метод используется для сдвига числа на один разряд вправо.
    void Integer::ShiftRight() {
        _digits.push_back(_digits[_digits.size() - 1]);

        for (size_t i = _digits.size() - 2; i > 0; --i)
            _digits[i] = _digits[i - 1];
        _digits[0] = 0;
    }


} // namespace

int main(){
    int a;
}