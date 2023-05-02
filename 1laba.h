#pragma once
#include <cinttypes>
#include <vector>
#include <string>
#include <iostream>
using std::string;
using std::vector;

// Arbitrary-precision arithmetic representation of integers
namespace arbitary_precision {
	class Integer final {
	public:
		string ToString(); // Метод возвращающий представление целого числа в строку.
		Integer() : _digits({ 0 }), _negative(false) {} //Конструктор по умолчанию, Устанавливает значение целого числа равным нулю.
		Integer(const Integer&); //Конструктор копирования. Создает копию переданного числа.
		Integer& operator=(const Integer&); //Оператор присваивания. Присваивает одно целое число другому.

		Integer(string); // ~~~String constructors~~~ //Конструктор принимающий строку. Создает целое число на основе переданной строки.
		Integer(char* str) : Integer(string(str)) {}; //Конструктор,принимающий указатель на строку типа char*. Вызывает конструктор от string(str),
                                                        // Создает целое число на основе переданной строки.
		Integer(const char* str) : Integer(string(str)) {}; //Конструктор, принимающий указатель на строку const char*, тоже самое.


        //Конструктор, принимающий значение типа T, вызывается конструктор от integer(to_string(number)).
		template<typename T> // FIXME
		Integer(T number) : Integer(std::to_string(number)) {}

		~Integer() = default; //Деструктор.

		// arithmetic operators

		// arithmetic unary operators

        //Унарные операторы, которые нужно перегрузить.
		Integer operator-() const;
		Integer& operator--();
		Integer operator--(int);

		Integer operator+() const;
		Integer& operator++();
		Integer operator++(int);

		// arithmetic binary operators
        //Бинарные операторы которые нужно перегрузить.
		[[nodiscard]] Integer operator-(const Integer&) const;
		Integer& operator-=(const Integer&);

		[[nodiscard]] Integer operator+(const Integer&) const;
		Integer& operator+=(const Integer&);

		[[nodiscard]] Integer operator*(const Integer&) const;
		Integer& operator*=(const Integer&);

		[[nodiscard]] Integer operator/(const Integer&) const;
		Integer& operator/=(const Integer&);

		[[nodiscard]] Integer operator%(const Integer&) const;
		Integer& operator%=(const Integer&);


        //Приведение к типам примитивных данных.
		[[nodiscard]] explicit operator bool() const { return static_cast<bool>(_digits[0]); }
		[[nodiscard]] explicit operator unsigned char() const { return static_cast<unsigned char>(_digits[0]); }
		[[nodiscard]] explicit operator char() const { return static_cast<char>(_negative ? -_digits[0] : _digits[0]); }
		[[nodiscard]] explicit operator short unsigned int() const { return static_cast<unsigned int>(_digits[0]); }
		[[nodiscard]] explicit operator short int() const { return static_cast<short int>(_negative ? -_digits[0] : _digits[0]); }
		[[nodiscard]] explicit operator unsigned int() const { return static_cast<unsigned int>(_digits[0]); }
		[[nodiscard]] explicit operator int() const { return static_cast<int>(_negative ? -_digits[0] : _digits[0]); }
		[[nodiscard]] explicit operator long unsigned int() const { return static_cast<long unsigned int>(_digits[0]); }
		[[nodiscard]] explicit operator long int() const { return static_cast<long int>(_negative ? -_digits[0] : _digits[0]); }
		[[nodiscard]] explicit operator long long unsigned int() const { return static_cast<long long unsigned int>(_digits[0]); }
		[[nodiscard]] explicit operator long long int() const { return static_cast<long long int>(_negative ? -_digits[0] : _digits[0]); }

		// Comparators
        //Перегрузка компараторов.
		[[nodiscard]] bool operator==(const Integer&) const;
		[[nodiscard]] bool operator!=(const Integer&) const;
		[[nodiscard]] bool operator<(const Integer&) const;
		[[nodiscard]] bool operator<=(const Integer&) const;
		[[nodiscard]] bool operator>(const Integer&) const;
		[[nodiscard]] bool operator>=(const Integer&) const;


		// Output stream print helper
        //Нужно перегрузить operator<<, который позволяет выводить объекты класса Integer в выходной поток.
		template <typename OStream>
		friend OStream&& operator<<(OStream&&, Integer&);

	private:
		static const int64_t BASE = 1000000000; // billion
		static const int64_t LENGTH_FOR_KARATSUBA = 2;
		vector<int64_t> _digits;
		bool _negative{};
		void RemoveLeadingZeroes();
		static bool OnlyZeroes(const string&);
		bool IsZero() const;
		static void Normalize(Integer&);
		void ShiftRight();
	};


    //Перегрузка оператора <<.
	template <typename OStream>
	OStream&& operator<<(OStream&& os, Integer& integer) {
		os << integer.ToString();
		return os;
	}



}  // namespace arbitary_precision