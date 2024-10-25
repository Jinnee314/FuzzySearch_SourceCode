#include "combinatorics.hpp"
#include "extended_sampling.hpp"

#include <vector>
#include <fstream>
#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

//создаём множество строк, в которых ошибка - это замена символов, и добавление его к общему множеству. 0 < Количество заменённых символов <= numErrors.
void addAllSubstitutions
(
	std::vector<std::string>& vectorSelection, 
	uint64_t& posInVectorSelection, 
	const std::string& pattern,
	const uint32_t numErrors, 
	const uint32_t patternSize
)
{
	std::string workCopy = pattern, saveCopy = pattern;
	//uint64_t countReplacment = static_cast<uint64_t>(pow(countLetters, numErrors));//количество замен
	std::vector<uint32_t> posInString(numErrors);

	/*
		В векторе posInString храним кандидатов позиций
		В данном алгоритме на возможные позиции накладывается ограничение - позиции всегда идут по убыванию;
	*/

	for (uint32_t i = 0; i < numErrors; ++i)
	{
		posInString[i] = i;
	}
	/*
		Перебираем все замены и запоминаем их.
		Идея в том, что бы выбрать numErrors позиций в исходной строке и перебрать на этих позициях все комбинации символов
		после этого выбрать другие numErrors позиций и так пробежать все возможные комбинации из numErrors позиций на всей длине исходной строки.

		Устанавливаем на выбранных позициях первый символ алфавита. Каждый раз стартуем с этого. Далее речь только про эти символы.

		Фиксируем символы на всех позициях, кроме последней. На последней позиции перебираем все возможные символы (Каждый раз сохраняем новую строку), после этого на предпоследней
		позиции меняем символ на следующий и снова пробегаем все символы на последней позиции. Когда пробежали все символы на предпоследней позиции, на предпредпоследней позиции меняем
		символ на следущий. И так далее пробегаем все возможные комбинации символов. После чего меняем набор позиций и повторяем.

		Набор позиций меняется по тому же принципу: сначала перебираем всех кандидатов на последнюю позицию, затем меняем предпоследнюю и снова переребираем всех кандидатов.
		Таким образом переберём все возможные позиции

		Далее по коду, если не сказано иначе, перебор символов и позиций (да и почти любой другой) осуществляется по той же логике, что и здесь, лишь с изменением органичений.

		Все позиции различны и идут по возрастанию
	*/

	while (posInString[0] != patternSize - numErrors + 1) //пока первая позиция не последняя из возможных для неё 
	{
		for (uint32_t i = 0; i < numErrors; ++i)//устанавливааем первый символ алфавита на всех позициях
		{
			workCopy[posInString[i]] = 'а';
		}

		//сохраняем новое слово
		vectorSelection[posInVectorSelection] = workCopy;
		++posInVectorSelection;

		while (workCopy[posInString[0]] != 'я' + 1)//пока символ на первой позиции не равен последнему символу алфавита + 1
		{
			++workCopy[posInString[numErrors - 1]];//меняем символ на последний позиции на следующий
			
			vectorSelection[posInVectorSelection] = workCopy;
			++posInVectorSelection;

			if (workCopy[posInString[numErrors - 1]] == 'я') //если символ на последней позиции равен последнему символу алфавита
			{
				workCopy[posInString[numErrors - 1]] = 'а';//меняем символ на последней позиции на стартовый
				
				if (!(numErrors - 1))	
				{
					break;//если количество ошибок = 1, далее идти не нужно
				}
				
				++workCopy[posInString[numErrors - 2]];//меняем символ на предпоследний позиции на следующий
				
				for (int32_t j = numErrors - 2; j > 0; --j)//пробегаемся по символам на всех позициях от конца к началу
				{
					if (workCopy[posInString[j]] == 'я' + 1)//если на зафиксированной позиции встречается последниц символ алфавита + 1
					{
						workCopy[posInString[j]] = 'а';//меняем его на первый
						
						++workCopy[posInString[j - 1]];//а символ на позиции до него меняем на следующий
					}
				}
				
				if (workCopy[posInString[0]] == 'я' + 1) 
				{
					break;//если символ на первой позиции равен последнему симолу алфавита + 1, то тормозим
				}

				vectorSelection[posInVectorSelection] = workCopy;
				++posInVectorSelection;
			}
		}

		++posInString[numErrors - 1];//меняем последнюю позицию
		if (posInString[numErrors - 1] == patternSize)//если последняя позиция вышла за пределы строки
		{
			for (int32_t i = numErrors - 2, diff = 1; i > -1; --i, ++diff)//поочердно меняем все позиции
			{
				++posInString[i];
				
				if (posInString[i] != patternSize - diff)//если i-ая позиция не равна своему последнему возможному варианту
				{
					for (uint32_t j = i + 1; j < numErrors; ++j)//переопределяем правильными позициями все позиции, после i-ой
					{
						posInString[j] = posInString[j - 1] + 1;
					}

					break;
				}
			}
		}

		workCopy = saveCopy;
	}
}

//создаём множество строк, в которых ошибка - это удаление(пропуск) символов, и добавление его к общему множеству. 0 < Количество удалённых символов <= numErrors.
void addAllErase
(
	std::vector<std::string>& vectorSelection,
	uint64_t& posInVectorSelection,
	const std::string& pattern,
	const uint32_t numErrors,
	const uint32_t patternSize,
	const uint64_t bitProof
)
{
	//количество всех возможных удалений на количестве позиций <= numErrors
	uint64_t countAllErase = combinatorics::combination(patternSize, numErrors) * static_cast<uint64_t>(pow(2, numErrors));
	
	std::string workCopy = pattern, saveCopy = pattern;
	
	uint64_t countIterErase = countAllErase / bitProof; //количество итераций внешного цикла
	
	std::vector<std::uint32_t> posInString(numErrors);
	
	// в данном алгоритме на возможные позиции накладывается ограничение - позиции всегда идут по убыванию;
	for (uint32_t i = 0; i < numErrors; ++i)// ставим позиции в исходное состояние
	{
		posInString[i] = i;
	}

	/*
		Идея в том, чтобы перебрать все возможные варианты позиций, а на них все возможные варианты удалений.
		Перебор позиций как и в прошлом алгоритме.
	*/

	for (uint64_t z = 0; z < countIterErase; ++z)
	{
		/*
			Идея перебора вариантов удаления в том, что в битовом представлении i пробежит все комбинации из 1 в numErrors позициях
			нам нужно лишь проверить, какие биты в i равны 1 и удалить символы, позиции которых соотносятся с позициями этих битов. 
		*/

		for (uint32_t i = 1; i < bitProof; ++i)
		{
			for (uint32_t j = 0; j < numErrors; ++j)//проверка битов путём сдвига и побитового и
			{
				if (((i >> j) & 1) == 1)
				{
					workCopy[posInString[j]] = '~'; // этот символ будем удалять, ставим его на позиции, на которых будет происходить удаление
				}
			}

			workCopy.erase(std::remove(workCopy.begin(), workCopy.end(), '~'), workCopy.end());//способ удалить все конкретные символы из строки
			
			vectorSelection[posInVectorSelection] = workCopy;
			++posInVectorSelection;
			
			workCopy = saveCopy;
		}

		//перебор позиций
		++posInString[numErrors - 1];
		if (posInString[numErrors - 1] == patternSize)
		{
			for (int32_t i = numErrors - 2, diff = 1; i > -1; --i, ++diff)
			{
				++posInString[i];
				
				if (posInString[i] != patternSize - diff)
				{
					for (uint32_t j = i + 1; j < numErrors; ++j)
					{
						posInString[j] = posInString[j - 1] + 1;
					}

					break;
				}
			}
		}
	}
}


//создаём множество строк, в которых ошибка - это вставка символов, и добавление его к общему множеству. 0 < Количество вставленных символов <= numErrors.
void addAllInsert
(
	std::vector<std::string>& vectorSelection,
	uint64_t& posInVectorSelection, 
	const std::string& pattern,
	const uint32_t numErrors,
	const uint64_t bitProof, 
	const uint32_t maxPos, 
	const uint64_t countElementsForInsert
)
{
	std::string workCopy = pattern, saveCopy = pattern;
	
	std::string symbInsert(numErrors, 'а');//в этой строке перебираем все возможны комбинации символов
	
	std::vector<uint32_t> posInString(numErrors, 0);//зануляем вектор позиций, в данном алгоритме позиции могут быть одинаковыми
	
	/*
		перебираем все позиции, для каждой позиции перебираем все возможные комбинации символов
		для каждой комбинации символов перебираем комбинации их добавлений
	*/

	while (posInString[0] != maxPos)//пока позиция, первая в векторе, не равна максимально возможной позицие
	{
		for (uint64_t j = 0; j < countElementsForInsert; ++j)//перебираем все комбинации символов
		{
			for (uint64_t z = 1; z < bitProof; ++z)//перебираем все комбинации добавлений
			{
				for (uint32_t q = 0; q < numErrors; ++q)//проверка битов путём сдвига и побитового и
				{
					if (((z >> q) & 1) == 1)
					{
						workCopy.insert(posInString[numErrors - 1 - q], 1, symbInsert[numErrors - 1 - q]);
					}
				}

				vectorSelection[posInVectorSelection] = workCopy;
				++posInVectorSelection;

				workCopy = saveCopy;
			}

			++symbInsert[numErrors - 1];

			if (symbInsert[numErrors - 1] == 'я' + 1)//перебираем все возможные символы так же, как и в алгоритме до этого
			{
				symbInsert[numErrors - 1] = 'а';

				if (numErrors - 1)
				{
					++symbInsert[numErrors - 2];

					for (int32_t z = numErrors - 2; z > 0; --z)
					{
						if (symbInsert[z] == 'я' + 1)
						{
							symbInsert[z] = 'а';
							++symbInsert[z - 1];
						}
					}
				}
			}
		}

		symbInsert = std::string(numErrors, 'а');

		++posInString[numErrors - 1];
		if (posInString[numErrors - 1] == maxPos)//перебор всех возможных позиций как и в прошлых алгоритмах
		{
			for (int32_t z = numErrors - 2; z > -1; --z)
			{
				++posInString[z];
				if (posInString[z] != maxPos)
				{
					for (uint32_t j = z + 1; j < numErrors; ++j)
					{
						posInString[j] = posInString[j - 1];
					}

					break;
				}
			}
		}
	}
}

//создаём множество строк, в которых ошибка - это транспозиция символов, и добавление его к общему множеству. 0 < Количество транспозиций <= numErrors.
void addAllTransposition
(
	std::vector<std::string>& vectorSelection,
	uint64_t& posInVectorSelection,
	const std::string& pattern, 
	const uint32_t numErrors,
	const uint32_t countPermutationForOneTransposition
)
{
	uint64_t countExternalCycles = 0;//количество повторов первого for

	/*
		так как строить слова после ряда транспозиций мы будем на основе уже имеющихся слов, общее количество циклов равно 
		сумме всех степеней countPermutationForOneTransposition до numErrors не включая numErrors
	*/
	for (uint32_t i = 1; i < numErrors; ++i)
	{
		countExternalCycles += static_cast<uint64_t>(pow(countPermutationForOneTransposition, i));
	}

	vectorSelection[posInVectorSelection] = pattern;//начинаем строить на основании искомого слова, поэтому помещаем его в массив последним на данный момент
	
	uint64_t savePos = posInVectorSelection;//и запоминаем эту позицию
	
	++posInVectorSelection;
	
	std::string workCopy = pattern;
	
	/*
		берём слово с savePos и применяем одну транспозицию к каждой паре соседних символов
		запоминаем их в наш массив, а затем к полученным словам применяем ту же операцию, и повторяем это numErrors раз, чтобы получить все слова,
		которые можно получить numErrors транспозициями
	*/

	for (uint64_t i = 0; i <= countExternalCycles; ++i)
	{
		for (int32_t j = countPermutationForOneTransposition; j > 0; --j)//применяем транспозицию к каждой паре соседних символов
		{
			workCopy = vectorSelection[savePos + i];

			std::swap(workCopy[j], workCopy[j - 1]);

			vectorSelection[posInVectorSelection] = workCopy;
			++posInVectorSelection;
		}
	}
}


/*
	создаём выборку
	так как изменение ячейки вектора рабтает за константу, решил всю выборку сначала создать через вектор
	для этого сначала нужно посчитает необходимое количество памяти
	так как поиск в set работает за log(n), после создания вектора со всей выборкой, создаётся set и инициализируется вектором
	но не уверен что такое создание быстрее, нежели изначальная работа с set
*/
std::unordered_set<std::string> extended_sampling::createWrongWords
(
	std::string pattern,
	uint32_t numErrors
)
{
	uint32_t patternSize = static_cast<uint32_t>(pattern.size());
	uint32_t countLetters = 'я' - 'а' + 1;
	uint64_t sizeVectorSelection = 0, workInt64 = 0;

	//считаем количество ячеек в векторе для всех возможных замен, учитывая, что варианты могут повторяться, в силу особенностей реализации
	sizeVectorSelection = combinatorics::combination(patternSize, numErrors) * static_cast<uint64_t>(pow(countLetters, numErrors));

	//считаем количество ячеек для всех вариантов удалений и прибавляем к имеющемуся
	workInt64 = combinatorics::combination(patternSize, numErrors);
	sizeVectorSelection += workInt64 * (static_cast<uint64_t>(pow(2, numErrors)) - 1); //2 - так как символ может быть удалён или не удалён

	//считаем количество ячеек для всех вариантов добавлений
	uint32_t maxPos = patternSize + 1;

	uint64_t countVariantsPos = combinatorics::combinationWithReplace(maxPos, numErrors);
	uint64_t countElementsForInsert = static_cast<uint64_t>(pow(countLetters, numErrors));
	uint64_t bitProof = static_cast<uint64_t>(pow(2, numErrors));

	sizeVectorSelection += countVariantsPos * countElementsForInsert * (bitProof - 1);

	//считаем количество ячеек для всех вариантов перестановок
	uint32_t countPermutationForOneTransposition = patternSize - 1;
	
	for (uint32_t i = 0; i <= numErrors; ++i)
	{
		sizeVectorSelection += static_cast<uint64_t>(pow(countPermutationForOneTransposition, i));
	}

	std::vector<std::string> vectorSelection(sizeVectorSelection);
	uint64_t posInVectorSelection = 0;
	std::string workCopy = pattern, saveCopy = pattern;

	//замены
	addAllSubstitutions(vectorSelection, posInVectorSelection, pattern, numErrors, patternSize);

	//удаления
	addAllErase(vectorSelection, posInVectorSelection, pattern, numErrors, patternSize, bitProof);

	//добавление
	addAllInsert(vectorSelection, posInVectorSelection, pattern, numErrors, bitProof, maxPos, countElementsForInsert);

	//транспозиции
	addAllTransposition(vectorSelection, posInVectorSelection, pattern, numErrors, countPermutationForOneTransposition);

	return std::unordered_set<std::string>(vectorSelection.begin(), vectorSelection.end());
}


