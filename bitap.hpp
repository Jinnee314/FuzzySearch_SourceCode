#pragma once
#include <cstdint>
#include <string>
#include <vector>
#include <filesystem>

/*
	Bitap сообщает, содержит ли заданный текст подстроку приблизительно равную
	заданному шаблону по рассто€нию Ћевенштейна.
	Bitap наиболее известен как один из базовых алгоритмов утилиты agrep в Unix-системах. 
*/

namespace bitap
{
	namespace exact_search
	{
		/*
			„Єткий поиск с помощью bitap.
			¬озвращает позицию в строке, c которой подстрока строки равна pattern.
			¬ случаем неудачи возвращает std::string::npos.
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
			ЌечЄткий поиск с помощью bitap в строке
			¬озвращает первую позицию в строке(pos), до которой подстрока строки приблизительно равна pattern.
			¬ случаем неудачи возвращает std::string::npos.
			“ак как учитываетс€ и удаление, и вставка, мы не можем точно сказать, с какой позиции подстрока
			текста приблизительно равна pattern. ћожно лишь точно сказать, что подстрака начинаетс€ не раньше
			позиции pos - (pattern.size + numErrors).
			я не уверен, стоит ли выводить именно это число или оставить так, как есть.
		*/
		uint64_t subStringInString
		(
			const std::string& text,
			const std::string& pattern,
			uint32_t numErrors
		);

		/*
			ѕоиск строки в файле, в которой есть подстрока, приблизительно равна€ request.
			¬озвращает вектор строк, в которых найдена подстрока.
			¬ случае неудачи возвращает пустой вектор.
		*/
		std::vector< std::string > stringsInFile
		(
			const std::filesystem::path& wayToFile,
			const std::string& request,
			uint32_t numErrors
		);

		/*
			ѕоиск всех подстрок в троке.
			¬озвращает вектор позиций в строке, до которых подстроки строки приблизительно равны pattern.
			¬ случае неудачи возвращает пустой вектор.
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