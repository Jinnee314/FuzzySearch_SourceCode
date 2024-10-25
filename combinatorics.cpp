#include "combinatorics.hpp"

#include <cstdint>

//считаем факториал
uint64_t combinatorics::factorial( const int32_t num )
{
	if (num == 1 || num == 0)
	{
		return 1;
	}

	int64_t res = 2;

	for (int32_t i = 3; i <= num; ++i)
	{
		res *= i;
	}

	return res;
}

//количество сочетаний
uint64_t combinatorics::combination
(
	const int32_t from,
	const int32_t count
)
{
	if (count > from)
	{
		return 0;
	}

	if (count == 1)
	{
		return from;
	}

	if (count == from)
	{
		return 1;
	}

	return factorial(from) / (factorial(count) * factorial(from - count));
}

//количество сочетаний с повторениями
uint64_t combinatorics::combinationWithReplace(uint32_t from, uint32_t num)
{
	uint64_t chis = static_cast<uint64_t>(from);
	for (size_t i = 1; i < num; ++i)
	{
		chis *= static_cast<uint64_t>(from) + i;
	}
	return chis / factorial(num);
}