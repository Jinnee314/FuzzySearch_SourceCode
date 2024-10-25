#pragma once

#include <cstdint>
#include <string>

/*
	������������ ����, ���������� ������� ����������
	��������� ������ ��������� ������.
*/
namespace metrics
{
	// �������, ����������� ���������� ���������� ����� �������� left � right.
	uint32_t LevenshteinDistance( const std::string& left, const std::string& right );

	// �������, ����������� ���������� ����������-������� ����� �������� left � right.
	uint32_t LevenshteinDamerauDistance( const std::string& left, const std::string& right );
}