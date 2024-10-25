#include "Bitap.hpp"

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <limits>

/*
	Алгоритм:
		Пусть есть текст для поиска text длинны N и шаблон поиска pattern длинны n.
		Тогда построим матрицу T размерами n x N: если i-префикс pattern совпадает с text в позициях j-i+1...j, ставим в матрице 1 на позиции ( i,j ). Иначе 0.
		Сразу покажем формулу для получения столбца j( обозначим T( j ) ): T( j ) = ( ( T( j-1 ) << 1 ) | 00...001 ) & vectorMask[text[j]], где j от 0 до N - 1.
		Т( -1 ) столбец заполнен нулями.
		vectorMask - так же матрица n x |A|, где |A| - размер алфавита. В нём в столбце i на позиции j стоит 1, если pattern[j] = 'i', где 'i' - символ с кодом i.
		Шаблон найден, если найдена 1 в последней строке этой матрицы.
		Так как при сдвиге на 1 влево на освободившееся место приходит 0, часто меняют ролями 1 и 0.
		Таким образом формула T( j ) выглядит так: T( j ) = ( T( j-1 ) << 1 ) & vectorMaskNeg[text[j]].
*/

size_t rightCode( char a )
{
	/*
		Символы русского алфавита имеют отрицательный код, поэтому пришлось написать перевод, чтобы обращение по индексу работало
	*/
	return static_cast< size_t >( a ) + 128;
}

void createMask
( 
	const std::string& pattern,
	std::vector< uint64_t >& mask
)
{
	/*
		В этой функции вычисляем vectorMaskNeg - vectorMask из описания с изменёнными ролями 1 и 0.
		Его проще представлять как двумерный массив, в котором номер строки - код символа, а номер
		столба - позиция в pattern. Тогда и получаем, что на позиции ( i,j ) стоит 0, если pattern[j] = 'i', где 'i' - символ с кодом i.

		Я не уверен, что это хороший вариант, потому что у нас в векторе очень много чисел, которые никак не используются, но лучше
		пока не придумал.
	*/
	mask.resize( std::numeric_limits< char >::max() * 2 );
	for ( int32_t i = 0; i < mask.size(); ++i )
	{
		mask[i] = ~0ULL;
	}
	for ( int32_t i = 0; i < pattern.size(); ++i )
	{
		mask[rightCode( pattern[i] )] &= ~( 1ULL << i );
	}
}


/*
	Чёткий поиск методом bitap( shift-or ). На его основе проще понять суть работы,
	но не уверен, что его нужно включать в финальную версию
*/
uint64_t bitap::exact_search::subSrtinginString
( 
	const std::string& text,
	const std::string& pattern
)
{
	/*
		Детали реализации:
			Так как при сдвиге на 1 влево на освободившееся место приходит 0, поменяем 1 и 0 ролями.
			То есть теперь формула T( j ) выглядит так: T( j ) = ( T( j-1 ) << 1 ) & vectorMaskNeg[text[j]].
			Если я всё правильно понимаю, то в 64х разрядных процессорах максимальная производительность битовых операций
			достигается при работе с числами не длиннее 64 бит, а так как вся скорость алгоритма заключается именно в битовых
			операциях, то для максимальной производительности длинна pattern не может превосходить 64 символа.
			Так как мы не можем напрямую работать с битами, будем представлять наши столбцы числами uint64_t, в таком
			случае нужно проверять не последний бит, а n-ый бит числа.
			Не будем строить таблицу, а будем пробегать её по столбцам, данамически создавая каждый следующий. Если в какой-то
			момент обнаружим 0 на "последнем" месте, заканчиваем поиск и выводим позицию первого символа, с которого начинается шаблон.
	*/

	uint64_t textSize = static_cast< uint64_t >( text.size() );
	uint64_t patternSize = static_cast<uint64_t >( pattern.size() );

	//Проверки корректности запроса.
	if ( patternSize == 0 )
	{
		throw std::invalid_argument( "Pattern size = 0." );
	}

	if ( patternSize > 64 )
	{
		throw std::invalid_argument( "Pattern size > 64." );
	}

	if ( patternSize > textSize )
	{
		return std::string::npos;
	}

	std::vector< uint64_t > mask;
	createMask( pattern, mask );

	//Наш столбец T( -1 )
	uint64_t columnTable = ~0ULL;

	std::vector< std::string > res;

	for ( size_t i = 0; i < textSize; ++i )
	{
		columnTable = ( columnTable << 1 ) | mask[rightCode( text[i] )]; //По формуле получаем каждый следующий столбец

		//Проверяем "последний" бит
		if ( !( columnTable & ( 1ULL << ( patternSize - 1 ) ) ) )
		{
			return i - patternSize;
		}
	}

	return std::string::npos;
}

void printOut( const std::vector< std::vector< uint64_t > >& out, uint64_t sizePattern )
{
	for ( size_t i = 0; i < out.size(); ++i )
	{
		for ( size_t j = 0; j < sizePattern; ++j )
		{
			for ( size_t z = 0; z < out[i].size(); ++z )
			{
				std::cout << ( ( out[i][z] >> j ) & 1 );
			}
			std::cout << "\n";
		}
		std::cout << "\n";
	}
}

uint64_t bitap::fuzzy_search::subStringInString
( 
	const std::string& text,
	const std::string& pattern,
	uint32_t numErrors
)
{
	/*
		Для нечёткого поиска нужно модифицировать алгоритм чёткого поиска следующим образом:
			Для простоты поменяем 1 и 0 ролями.
			Пусть pattern имеет не более, чем m ошибок по метрике Левенштейна. Нам нужно m+1 копий матрицы T.
			Пусть j ( от 0 до N - 1 ) - номер символа текста( номер столбаца ), k ( от 1 до m )- номер матрицы T.
			Тогда вычисляем столбец j матрицы k T( j,k ) следующим образом:
				q( j,k ) = ( T( j-1, k ) << 1 ) | vecMaskNeg[text[j]];
				insert( j,k ) = q( j,k ) & T( j-1, k-1 );
				delet( j,k ) = q( j,k ) & ( T( j, k-1 ) << 1 );
				replace( j,k ) = q( j,k ) & ( T( j-1, k-1 ) << 1 );
				T( j,k ) = insert( j,k ) & delet( j,k ) & replace( j,k );
			Столбец j матрицы 0 T( j,0 ) вычисляется по формуле: T( j,0 ) = ( T( j-1, 0 ) << 1 ) | vecMaskNeg[text[j]];
			Столбец Т( -1, * ) заполнен 1.
	*/

	/*
		Детали реализации:
			Опять же, будем работать не со всеми матрицами сразу, а с столбцами этих матриц, динамически вычисляя следующий.
	*/

	uint64_t textSize = static_cast< uint64_t >( text.size() );
	uint64_t patternSize = static_cast< uint64_t >( pattern.size() );

	if ( patternSize == 0 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Pattern size = 0." );
	}

	if ( patternSize > 64 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Pattern size > 64." );
	}

	if ( numErrors < 0 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Number of errors < 0." );
	}

	std::vector< uint64_t > mask;
	createMask( pattern, mask );

	//столбцы матриц
	std::vector< uint64_t > columnTables( numErrors + 1, ~0ULL );

	uint64_t oldColumn = 0ULL;

	uint64_t q = 0;
	uint64_t insert = 0;
	uint64_t delet = 0;
	uint64_t replace = 0;

	for ( size_t i = 0; i < textSize; ++i )
	{
		oldColumn = columnTables[0];

		columnTables[0] = ( columnTables[0] << 1 ) | mask[rightCode( text[i] )];

		for ( size_t k = 1; k <= numErrors; ++k )
		{
			q = ( columnTables[k] << 1 ) | mask[rightCode( text[i] )];
			insert = q & oldColumn;
			delet = q & ( columnTables[k - 1] << 1 );
			replace = q & ( oldColumn << 1 );

			oldColumn = columnTables[k];

			columnTables[k] = insert & delet & replace;
		}

		for ( size_t j = 0; j <= numErrors; ++j )
		{
			if ( !( columnTables[j] & ( 1ULL << ( patternSize - 1 ) ) ) )
			{
				return i;
			}
		}

	}

	return std::string::npos;
}

std::vector< std::string > bitap::fuzzy_search::stringsInFile
( 
	const std::filesystem::path& wayToFile,
	const std::string& request,
	uint32_t numErrors
)
{
	if ( numErrors < 0 )
	{
		throw std::invalid_argument( "Number of errors < 0" );
	}

	std::ifstream fin( wayToFile );

	if ( !fin.is_open() )
	{
		throw std::runtime_error( "Ошибка. Файл не открыт." );
	}

	if ( std::filesystem::is_empty( wayToFile ) )
	{
		throw std::runtime_error( "Файл пуст." );
	}

	std::string line;
	std::vector< std::string > res;

	while ( std::getline( fin, line ) )
	{
		if ( subStringInString( line, request, numErrors ) != std::string::npos )
		{
			res.push_back( line );
		}
	}

	fin.close();

	return res;
}

std::vector< uint64_t > bitap::fuzzy_search::allSubStringInString
( 
	const std::string& text,
	const std::string& pattern,
	uint32_t numErrors
)
{
	/*
		Тоже самое, что и в поиске первой подстроки в строке, кроме одного цикла
	*/

	uint64_t textSize = static_cast< uint64_t >( text.size() );
	uint64_t patternSize = static_cast< uint64_t >( pattern.size() );

	if ( patternSize == 0 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Pattern size = 0." );
	}

	if ( patternSize > 64 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Pattern size > 64." );
	}

	if ( numErrors < 0 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Number of errors < 0." );
	}

	std::vector< uint64_t > mask;
	createMask( pattern, mask );

	std::vector< uint64_t > columnTables( numErrors + 1, ~0ULL );

	uint64_t oldColumn = 0ULL;

	uint64_t q = 0;
	uint64_t insert = 0;
	uint64_t delet = 0;
	uint64_t replace = 0;

	std::vector< uint64_t > res;

	for ( size_t i = 0; i < textSize; ++i )
	{
		oldColumn = columnTables[0];

		columnTables[0] = ( columnTables[0] << 1 ) | mask[rightCode( text[i] )];

		for ( size_t j = 1; j <= numErrors; ++j )
		{
			q = ( columnTables[j] << 1 ) | mask[rightCode( text[i] )];
			insert = q & oldColumn;
			delet = q & ( columnTables[j - 1] << 1 );
			replace = q & ( oldColumn << 1 );

			oldColumn = columnTables[j];

			columnTables[j] = insert & delet & replace;
		}

		for ( size_t j = 0; j <= numErrors; ++j )
		{
			/*
				Вместо выхода при нахождении первого нуля запоминаем позицию текста в вектор.
			*/
			if ( !( columnTables[j] & ( 1ULL << ( patternSize - 1 ) ) ) )
			{
				res.push_back( i );
				break;//дабы не запоминать несколько одних и тех же вариантов
			}
		}
	}

	return res;
}

uint64_t bitap::fuzzy_search::subStringInStringDL
( 
	const std::string& text,
	const std::string& pattern,
	uint32_t numErrors
)
{
	uint64_t textSize = static_cast< uint64_t >( text.size() );
	uint64_t patternSize = static_cast< uint64_t >( pattern.size() );

	if ( patternSize == 0 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Pattern size = 0." );
	}

	if ( patternSize > 64 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Pattern size > 64." );
	}

	if ( numErrors < 0 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Number of errors < 0." );
	}

	if ( numErrors > patternSize / 2 )
	{
		throw std::invalid_argument( "numErrors > patternSize / 2" );
	}

	std::vector< uint64_t > mask;
	createMask( pattern, mask );

	//столбцы матриц
	std::vector< uint64_t > columnTables( numErrors + 1, ~0ULL );
	std::vector< uint64_t > elderColumns( numErrors, ~0ULL );

	uint64_t oldColumn = 0ULL;

	uint64_t q = 0;
	uint64_t insert = 0;
	uint64_t delet = 0;
	uint64_t replace = 0;
	uint64_t transposition = ~0ULL;

	for ( size_t i = 0; i < textSize; ++i )
	{
		oldColumn = columnTables[0];

		columnTables[0] = ( columnTables[0] << 1 ) | mask[rightCode( text[i] )];
		for ( size_t k = 1; k <= numErrors; ++k )
		{
			q = ( columnTables[k] << 1 ) | mask[rightCode( text[i] )];
			insert = q & oldColumn;
			delet = q & ( columnTables[k - 1] << 1 );
			replace = q & ( oldColumn << 1 );
			if ( i )
			{
				transposition = q & ( elderColumns[k - 1] << 2 );
			}

			elderColumns[k - 1] = oldColumn;

			oldColumn = columnTables[k];

			columnTables[k] = insert & delet & replace & transposition;
		}

		for ( size_t j = 0; j <= numErrors; ++j )
		{
			if ( !( columnTables[j] & ( 1ULL << ( patternSize - 1 ) ) ) )
			{
				return i;
			}
		}
	}


	return std::string::npos;
}

std::vector< size_t > bitap::fuzzy_search::allSubStringInStringDL
( 
	const std::string& text,
	const std::string& pattern,
	uint32_t numErrors
)
{
	uint64_t textSize = static_cast< uint64_t >( text.size() );
	uint64_t patternSize = static_cast< uint64_t >( pattern.size() );

	if ( patternSize == 0 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Pattern size = 0." );
	}

	if ( patternSize > 64 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Pattern size > 64." );
	}

	if ( numErrors < 0 )
	{
		throw std::invalid_argument( "FuzzySearchInString: Number of errors < 0." );
	}

	if ( numErrors > patternSize / 2 )
	{
		throw std::invalid_argument( "numErrors > patternSize / 2" );
	}

	std::vector< uint64_t > mask;
	createMask( pattern, mask );


	//столбцы матриц
	std::vector< uint64_t > columnTables( numErrors + 1, ~0ULL );
	std::vector< uint64_t > elderColumns( numErrors, ~0ULL );

	uint64_t oldColumn = 0ULL;

	uint64_t q = 0;
	uint64_t insert = 0;
	uint64_t delet = 0;
	uint64_t replace = 0;
	uint64_t transposition = ~0ULL;

	std::vector< size_t > res;

	for ( size_t i = 0; i < textSize; ++i )
	{
		oldColumn = columnTables[0];

		columnTables[0] = ( columnTables[0] << 1 ) | mask[rightCode( text[i] )];

		for ( size_t k = 1; k <= numErrors; ++k )
		{
			q = ( columnTables[k] << 1 ) | mask[rightCode( text[i] )];
			insert = q & oldColumn;
			delet = q & ( columnTables[k - 1] << 1 );
			replace = q & ( oldColumn << 1 );

			if ( i )
			{
				transposition = q & ( elderColumns[k - 1] << 2 );
			}

			elderColumns[k - 1] = oldColumn;

			oldColumn = columnTables[k];

			columnTables[k] = insert & delet & replace & transposition;
		}

		for ( size_t j = 0; j <= numErrors; ++j )
		{
			/*
				Вместо выхода при нахождении первого нуля запоминаем позицию текста в вектор.
			*/

			if ( !( columnTables[j] & ( 1ULL << ( patternSize - 1 ) ) ) )
			{
				res.push_back( i );
				break;//дабы не запоминать несколько одних и тех же вариантов
			}
		}
	}

	return res;
}

std::vector< std::string > bitap::fuzzy_search::stringsInFileDL
( 
	const std::filesystem::path& wayToFile,
	const std::string& request,
	uint32_t numErrors
)
{
	if ( numErrors < 0 )
	{
		throw std::invalid_argument( "Number of errors < 0" );
	}

	std::ifstream fin( wayToFile );

	if ( !fin.is_open() )
	{
		throw std::runtime_error( "Ошибка. Файл не открыт." );
	}

	if ( std::filesystem::is_empty( wayToFile ) )
	{
		throw std::runtime_error( "Файл пуст." );
	}

	std::string line;
	std::vector< std::string > res;

	while ( std::getline( fin, line ) )
	{
		if ( subStringInStringDL( line, request, numErrors ) != std::string::npos )
		{
			res.push_back( line );
		}
	}

	fin.close();

	return res;
}

std::vector< std::string > wordsByPositionsInString
( 
	const std::string& string,
	const std::vector< size_t >& positions,
	size_t sizeRequest,
	uint32_t numErrors
)
{
	std::vector< std::string > res;

	size_t numIter = sizeRequest + numErrors + 1;

	size_t startPos = 0;
	size_t sepPos = 0;
	bool startPosFound = false;
	bool sepPosFound = false;

	if ( positions.size() != 0 )
	{
		std::cout << "";
	}

	for ( const auto pos : positions )
	{
		if ( string[pos] == ' ' || pos < sepPos )
		{
			continue;
		}

		startPosFound = false;
		sepPosFound = false;

		for ( size_t i = 1; i < numIter; ++i )
		{
			if ( !startPosFound )
			{
				if ( string[pos - i] == ' ' )
				{
					startPos = pos - i + 1;
					startPosFound = true;
				}
			}

			if ( !sepPosFound )
			{
				if ( string[pos + i] == ' ' )
				{
					sepPos = pos + i;
					sepPosFound = true;
				}

			}

			if ( sepPosFound && startPosFound )
			{
				res.push_back( string.substr( startPos, sepPos - startPos ) );
				break;
			}
		}
	}

	return res;
}

std::vector< std::string > bitap::fuzzy_search::wordsInFileDL
( 
	const std::filesystem::path& wayToFile,
	const std::string& request,
	uint32_t numErrors
)
{
	if ( numErrors < 0 )
	{
		throw std::invalid_argument( "Number of errors < 0" );
	}

	std::ifstream fin( wayToFile );

	if ( !fin.is_open() )
	{
		throw std::runtime_error( "Ошибка. Файл не открыт." );
	}

	if ( std::filesystem::is_empty( wayToFile ) )
	{
		throw std::runtime_error( "Файл пуст." );
	}

	std::string line;
	std::string dop( numErrors, ' ' );
	std::vector< std::string > res;

	size_t sizeRequest = request.size();

	while ( std::getline( fin, line ) )
	{
		line.insert( 0, dop );
		line += dop;
		std::vector< std::string >rab{ wordsByPositionsInString( line, allSubStringInStringDL( line, request, numErrors ), sizeRequest, numErrors ) };
		if ( !rab.empty() )
		{
			res.insert( res.end(), rab.begin(), rab.end() );
		}
	}

	fin.close();

	return res;
}