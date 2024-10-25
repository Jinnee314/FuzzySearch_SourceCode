#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <filesystem>

namespace fuzzy_search
{
	/*
		Функция, находящее все слова в файле по пути wayToFile,
		расстояние Левештейна которых до заданного шаблона request не превосходит
		величины numError
	*/
	std::vector< std::string > InFileLevenstein
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		Функция, находящее все слова в строке string,
		расстояние Левештейна которых до заданного шаблона request не превосходит
		величины numError
	*/
	std::vector< std::string > InStringLevenstein
	(
		const std::string& string,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		Функция, находящее все слова в файле по пути wayToFile,
		расстояние Дамерау-Левештейна которых до заданного шаблона request не превосходит
		величины numError
	*/
	std::vector< std::string > InFileLevensteinDamerau
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		Функция, находящее все слова в строке string,
		расстояние Дамерау-Левештейна которых до заданного шаблона request не превосходит
		величины numError
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
		Поиск строки в файле, в которой есть подстрока, приблизительно равная request.
		Возвращает вектор строк, в которых найдена подстрока.
		В случае неудачи возвращает пустой вектор.
	*/
	std::vector< std::string > bitapStringsInFile
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		Алгоритм нечёткого поиска на основе первой модификации bitap
	*/
	std::vector< std::string > BitapDLFirstAndLevensteinDamerau
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		Алгоритм нечёткого поиска на основе второй модификации bitap
	*/
	std::vector< std::string > BitapDLSecondAndLevensteinDamerau
	(
		const std::filesystem::path& wayToFile,
		const std::string& request,
		uint32_t numErrors
	);

	/*
		Алгоритм расширенной выборки
	*/
	std::vector< std::string > ExtendedSampling
	(
		const std::filesystem::path& wayToFile,
		const std::unordered_set< std::string >& wrongWords
	);
}