#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

/*
	Bitap ��������, �������� �� �������� ����� ��������� �������������� ������
	��������� ������� �� ���������� �����������.
	Bitap �������� �������� ��� ���� �� ������� ���������� ������� agrep � Unix-��������. 
*/

namespace bitap
{
	namespace exact_search
	{
		/*
			׸���� ����� � ������� bitap.
			���������� ������� � ������, c ������� ��������� ������ ����� pattern.
			� ������� ������� ���������� std::string::npos.
		*/
		uint64_t subSrtinginString
		(
			const std::string& text,
			const std::string& pattern
		);
	}	

	namespace fuzzy_search
	{
		/*
			�������� ����� � ������� bitap � ������
			���������� ������ ������� � ������(pos), �� ������� ��������� ������ �������������� ����� pattern.
			� ������� ������� ���������� std::string::npos.
			��� ��� ����������� � ��������, � �������, �� �� ����� ����� �������, � ����� ������� ���������
			������ �������������� ����� pattern. ����� ���� ����� �������, ��� ��������� ���������� �� ������
			������� pos - (pattern.size + numErrors).
			� �� ������, ����� �� �������� ������ ��� ����� ��� �������� ���, ��� ����.
		*/
		uint64_t subStringInString
		(
			const std::string& text,
			const std::string& pattern,
			uint32_t numErrors
		);

		/*
			����� ������ � �����, � ������� ���� ���������, �������������� ������ request.
			���������� ������ �����, � ������� ������� ���������.
			� ������ ������� ���������� ������ ������.
		*/
		std::vector< std::string > stringsInFile
		(
			const std::filesystem::path& wayToFile,
			const std::string& request,
			uint32_t numErrors
		);

		/*
			����� ���� �������� � �����.
			���������� ������ ������� � ������, �� ������� ��������� ������ �������������� ����� pattern.
			� ������ ������� ���������� ������ ������.
		*/
		std::vector< uint64_t > allSubStringInString
		(
			const std::string& text,
			const std::string& pattern,
			uint32_t numErrors
		);

		/*uint64_t bitapAsLevensteinMetric
		(
			const std::string& text,
			const std::string& pattern,
			uint32_t numErrors
		);*/

		std::vector< std::string > stringsInFileDL
		(
			const std::filesystem::path& wayToFile,
			const std::string& request,
			uint32_t numErrors
		);

		uint64_t subStringInStringDL
		(
			const std::string& text,
			const std::string& pattern,
			uint32_t numErrors
		);

		std::vector< size_t > allSubStringInStringDL
		(
			const std::string& text,
			const std::string& pattern,
			uint32_t numErrors
		);

		std::vector< std::string > wordsInFileDL
		(
			const std::filesystem::path& wayToFile,
			const std::string& request,
			uint32_t numErrors
		);
	}
}