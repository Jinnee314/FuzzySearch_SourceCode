#pragma once

#include <cstdint>
#include <string>

/*
	Пространство имен, содержащее функции вычисления
	различных метрик нечеткого поиска.
*/
namespace metrics
{
	// Функция, вычисляющая расстояние Левештейна между строками left и right.
	uint32_t LevenshteinDistance( const std::string& left, const std::string& right );

	// Функция, вычисляющая расстояние Левештейна-Дамерау между строками left и right.
	uint32_t LevenshteinDamerauDistance( const std::string& left, const std::string& right );
}