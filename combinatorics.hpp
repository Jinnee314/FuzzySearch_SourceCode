#pragma once

#include <cstdint>

namespace combinatorics
{
	//���������
	uint64_t factorial( const int32_t num );
	
	//���������� ���������
	uint64_t combination
	(
		const int32_t count,
		const int32_t from
	);

	//���������� ��������� � ������������
	uint64_t combinationWithReplace
	(
		uint32_t from,
		uint32_t num
	);
}