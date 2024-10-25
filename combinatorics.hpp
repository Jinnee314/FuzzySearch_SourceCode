#pragma once

#include <cstdint>

namespace combinatorics
{
	//факториал
	uint64_t factorial( const int32_t num );
	
	//количество сочетаний
	uint64_t combination
	(
		const int32_t count,
		const int32_t from
	);

	//количество сочетаний с повторениями
	uint64_t combinationWithReplace
	(
		uint32_t from,
		uint32_t num
	);
}