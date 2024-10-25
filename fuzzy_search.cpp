#include "fuzzy_search.hpp"
#include "metrics.hpp"
#include "bitap.hpp"
#include "extended_sampling.hpp"

#include <fstream>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

std::vector< std::string > fuzzy_search::InFileLevenstein
( 
	const fs::path& wayToFile,
	const std::string& request,
	uint32_t numErrors
)
{
	/*
		Алгоритм нечёткого поиска, который просто берёт каждое следующее слово и считает
		расстояние с словом-запросом, запоминая только те, которые имеют расстояние меньше заданного.
	*/
	
	if (fs::is_empty(wayToFile))
	{
		throw std::runtime_error{ "Файл пуст" };
	}

	if ( numErrors < 0 )
	{
		throw std::invalid_argument( "Number of errors < 0" );
	}

	std::ifstream fin( wayToFile );

	if ( !fin.is_open() ) // То же самое, что и !fin. Просто для наглядности.
	{
		throw std::runtime_error( "Ошибка. Файл не открыт." );
	}

	std::string word;
	std::vector< std::string > res;

	for ( fin >> word; !fin.eof(); fin >> word )
	{
		if (std::abs(static_cast<int32_t>(word.size() - request.size())) < static_cast<int32_t>(numErrors) + 1)
		{
			if (metrics::LevenshteinDistance(request, word) <= numErrors)
			{
				res.push_back(word);
			}
		}
	}

	fin.close();
	return res;
}

std::vector< std::string > fuzzy_search::InStringLevenstein
( 
	const std::string& string,
	const std::string& request,
	uint32_t numErrors
)
{
	if ( string.empty() )
	{
		throw std::invalid_argument( "String empty" );
	}

	std::stringstream stream{ string };
	std::string word;

	std::vector< std::string > res;

	while ( stream >> word )
	{
		if (std::abs(static_cast<int32_t>(word.size() - request.size())) < static_cast<int32_t>(numErrors) + 1)
		{
			if (metrics::LevenshteinDistance(request, word) <= numErrors)
			{
				res.push_back(word);
			}
		}
	}

	return res;
}

std::vector< std::string> fuzzy_search::InFileLevensteinDamerau
( 
	const fs::path& wayToFile,
	const std::string& request,
	uint32_t numErrors
)
{
	/*
		Для этой функции все аналогично.
	*/
	if (fs::is_empty(wayToFile))
	{
		throw std::runtime_error{ "Файл пуст" };
	}

	if ( numErrors < 0 )
	{
		throw std::invalid_argument( "Number of errors < 0" );
	}

	std::ifstream fin( wayToFile );

	if ( !fin )
	{
		return std::vector< std::string >{ "Ошибка. Файл не открыт." };
	}

	std::string word = "";
	std::vector< std::string > res;

	for ( fin >> word; !fin.eof(); fin >> word )
	{
		if (std::abs(static_cast<int32_t>(word.size() - request.size())) < static_cast<int32_t>(numErrors) + 1)
		{
			if (metrics::LevenshteinDamerauDistance(request, word) <= numErrors)
			{
				res.push_back(word);
			}
		}
	}

	fin.close();
	return res;
}

std::vector< std::string > fuzzy_search::InStringLevensteinDamerau
( 
	const std::string& string,
	const std::string& request,
	uint32_t numErrors
)
{
	if ( string.empty() )
	{
		throw std::invalid_argument( "String empty" );
	}

	std::stringstream stream{ string };
	std::string word;

	std::vector< std::string > res;

	while ( stream >> word )
	{
		if (std::abs(static_cast<int32_t>(word.size() - request.size())) < static_cast<int32_t>(numErrors) + 1)
		{
			if (metrics::LevenshteinDamerauDistance(request, word) <= numErrors)
			{
				res.push_back(word);
			}
		}
	}

	return res;
}

std::vector< std::string > fuzzy_search::BitapAndLevenstein
( 
	const fs::path& wayToFile,
	const std::string& request,
	uint32_t numErrors
)
{
	if ( fs::is_empty( wayToFile ) )
	{
		throw std::runtime_error( "File empty" );
	}

	std::vector< std::string > res;
	std::vector< std::string > rab;

	for ( const auto& string : bitap::fuzzy_search::stringsInFile( wayToFile, request, numErrors ) )
	{
		rab = fuzzy_search::InStringLevenstein( string, request, numErrors );
		if ( !rab.empty() )
		{
			res.insert( res.end(), rab.begin(), rab.end() );
		}
	}

	return res;
}

std::vector< std::string > fuzzy_search::bitapStringsInFile
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
		if ( bitap::fuzzy_search::subStringInString( line, request, numErrors ) != std::string::npos )
		{
			res.push_back( line );
		}
	}

	fin.close();

	return res;
}

std::vector< std::string > fuzzy_search::BitapDLFirstAndLevensteinDamerau
( 
	const fs::path& wayToFile,
	const std::string& request,
	uint32_t numErrors
)
{
	if ( fs::is_empty( wayToFile ) )
	{
		throw std::runtime_error( "File empty" );
	}

	std::vector< std::string > res;

	for ( const auto& string : bitap::fuzzy_search::stringsInFileDL( wayToFile, request, numErrors ) )
	{
		std::vector< std::string > rab{ std::move( fuzzy_search::InStringLevensteinDamerau( string, request, numErrors ) ) };
		if ( !rab.empty() )
		{
			res.insert( res.end(), rab.begin(), rab.end() );
		}
	}

	return res;
}

std::vector< std::string > fuzzy_search::BitapDLSecondAndLevensteinDamerau
( 
	const std::filesystem::path& wayToFile,
	const std::string& request,
	uint32_t numErrors
)
{
	if ( fs::is_empty( wayToFile ) )
	{
		throw std::runtime_error( "File empty" );
	}

	std::vector< std::string > res;

	for ( const auto& word : bitap::fuzzy_search::wordsInFileDL( wayToFile, request, numErrors ) )
	{
		if ( metrics::LevenshteinDamerauDistance( request, word ) <= numErrors )
		{
			res.push_back( word );
		}
	}

	return res;
}

std::vector< std::string > fuzzy_search::ExtendedSampling
( 
	const std::filesystem::path& wayToFile,
	const std::unordered_set< std::string >& wrongWords
)
{
	if ( fs::is_empty( wayToFile ) )
	{
		throw std::runtime_error( "File empty" );
	}

	std::ifstream in( wayToFile );

	if ( !in.is_open() )
	{
		throw std::runtime_error( "File not open" );
	}

	auto it = wrongWords.begin();
	std::string word;
	std::vector< std::string > res;

	while ( in >> word )
	{
		it = wrongWords.find( word );

		if ( it != wrongWords.end() )
		{
			res.push_back( *it );
		}
	}

	return res;
}