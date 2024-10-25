#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <filesystem>

namespace fuzzy_search
{
	/*
		�������, ��������� ��� ����� � ����� �� ���� wayToFile,
		���������� ���������� ������� �� ��������� ������� request �� �����������
		�������� numError
	*/
	std::vector< std::string > InFileLevenstein
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		�������, ��������� ��� ����� � ������ string,
		���������� ���������� ������� �� ��������� ������� request �� �����������
		�������� numError
	*/
	std::vector< std::string > InStringLevenstein
	(
		const std::string& string,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		�������, ��������� ��� ����� � ����� �� ���� wayToFile,
		���������� �������-���������� ������� �� ��������� ������� request �� �����������
		�������� numError
	*/
	std::vector< std::string > InFileLevensteinDamerau
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		�������, ��������� ��� ����� � ������ string,
		���������� �������-���������� ������� �� ��������� ������� request �� �����������
		�������� numError
	*/
	std::vector< std::string > InStringLevensteinDamerau
	(
		const std::string& string,
		const std::string& request,
		uint32_t numErrors
	);

	std::vector< std::string > BitapAndLevenstein
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		����� ������ � �����, � ������� ���� ���������, �������������� ������ request.
		���������� ������ �����, � ������� ������� ���������.
		� ������ ������� ���������� ������ ������.
	*/
	std::vector< std::string > bitapStringsInFile
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		�������� ��������� ������ �� ������ ������ ����������� bitap
	*/
	std::vector< std::string > BitapDLFirstAndLevensteinDamerau
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		�������� ��������� ������ �� ������ ������ ����������� bitap
	*/
	std::vector< std::string > BitapDLSecondAndLevensteinDamerau
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		�������� ����������� �������
	*/
	std::vector< std::string > ExtendedSampling
	(
		const std::filesystem::path& wayToFile,
		const std::unordered_set< std::string >& wrongWords
	);
}