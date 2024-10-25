#include "metrics.hpp"

#include <vector>
#include <algorithm>

uint32_t metrics::LevenshteinDistance
(
	const std::string& left,
	const std::string& right
)
{
	/*
		Данная функция вычисляет расстояние Левенштейна при помощи
		алгоритма Вагнера - Фишера.

		Обозначение: d(x, y) - расстояние Левенштейна между строками x и y.
		
		Суть в том, что по алгоритму создаётся матрица, в которой на позиции(i, j) находится
		количество операций (цена) для перехода от префикса длинны i слова х
		к префиксу длинны j слова y. В правом нижнем углу этой матрицы,
		на позиции (leftSize, rightSize), будет число, являющееся расстоянием Левенштейна.
	*/
	
	uint32_t leftSize{ static_cast< uint32_t >( left.size() ) };
	uint32_t rightSize{ static_cast< uint32_t >( right.size() ) };

	/*
		Алгоритм вычисления предполагает последовательное рассмотрение строк матрицы,
		размеры которой определяются длинами входных строк,
		Эффективнее с точки зрения потребляемой памяти строить матрицу с меньшим числом строк.
		Следовательно, возможно воспользоваться одной из аксиом метрики и изменить порядок входных строк
		соответсвующим образом.
	*/
	if ( leftSize > rightSize )
	{
		return LevenshteinDistance( right, left );
	}

	/*
		Для вычисления расстояния Левенштейна
		в памяти достаточно хранить только 2 строки матрицы.
	*/

	std::vector<int32_t> prevLine( rightSize + 1 );
	std::vector<int32_t> currentLine( rightSize + 1 );

	for ( uint32_t i{ 0 }; i <= rightSize; ++i )
	{
		currentLine[i] = i;
	}

	// "Цена" вставки.
	int32_t insertCost{ 0 };

	// "Цена" удаления.
	int32_t deleteCost{ 0 };

	// "Цена" замены
	int32_t replaceCost{ 0 };

	for ( size_t i{ 1 }; i <= leftSize; ++i )
	{
		std::swap( currentLine, prevLine );
		currentLine[0] = static_cast<int32_t>(i);

		for ( size_t j{ 1 }; j <= rightSize; ++j )
		{
			insertCost = currentLine[j - 1] + 1;
			deleteCost = prevLine[j] + 1;

			replaceCost =
				prevLine[j - 1] +
				(left[i - 1] ==
				  right[j - 1] ? 0 : 1 );

			/*
				Выбираем минимальное из всех и получаем расстояние (количество базовых операций
				для преобразования одного префикса в другой) между двумя префиксами.
			*/
			currentLine[j] = std::min( { insertCost, deleteCost, replaceCost } );
		}
	}

	return currentLine.back();
}

uint32_t metrics::LevenshteinDamerauDistance( const std::string& left, const std::string& right )
{
	/*
		Алгоритм вычисления расстояния Левенштейна - Дамерау аналогичен
		алгоритму вычисления расстояния Левештейна за исключением необходимости
		учета транспозиций символов в качестве базовой операции.
	*/
	
	uint32_t leftSize{ static_cast< uint32_t >( left.size() ) };
	uint32_t rightSize{ static_cast< uint32_t >( right.size() ) };

	if ( leftSize > rightSize )
	{
		return LevenshteinDamerauDistance( right, left );
	}
	
	/*
		Если транспозиция невозможна, то необходимо задать максимальную цену,
		чтобы она точно не учитывалась. Такая цена соответствует длине строки матрицы.
	*/
	const uint32_t MAXCOST{ rightSize + 2 };

	// В данном случае в памяти неолбходимо хранить 3 строкки матрицы

	std::vector<int32_t> prevLine( rightSize + 1 );
	std::vector<int32_t> prevPrevLine( rightSize + 1 );
	std::vector<int32_t> currentLine( rightSize + 1 );
	
	for ( uint32_t i{ 0 }; i <= rightSize; ++i )
	{
		currentLine[i] = i;
	}

	int32_t insertCost{ 0 };
	int32_t deleteCost{ 0 };
	int32_t replaceCost{ 0 };
	int32_t transpositionCost = MAXCOST;

	for ( size_t i{ 1 }; i <= leftSize; ++i )
	{
		if ( i > 1 ) 
		{
			std::swap( prevPrevLine, prevLine );
		}

		std::swap( currentLine, prevLine );
		currentLine[0] = static_cast<int32_t>(i);
		transpositionCost = MAXCOST;

		for ( size_t j{ 1 }; j <= rightSize; ++j )
		{
			insertCost = currentLine[j - 1] + 1;
			deleteCost = prevLine[j] + 1;
			replaceCost = 
				prevLine[j - 1] + 
				( left[i - 1] == 
					right[j - 1] ? 0 : 1 );

			// Транспозицию учитывается только если она возможна и соседние символы перекрёстно равны.
			if ( i > 1 && j > 1 )
			{
				transpositionCost = MAXCOST;

				if ( left[i - 1] == right[j - 2] && left[i - 2] == right[j - 1] )
				{
					transpositionCost = prevPrevLine[j - 2] + 1;
				}
			}

			currentLine[j] = std::min( { insertCost, deleteCost, replaceCost, transpositionCost } );
		}
	}

	return currentLine.back();
}
