#include "Bitap.hpp"

#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <limits>

/*
	��������:
		����� ���� ����� ��� ������ text ������ N � ������ ������ pattern ������ n.
		����� �������� ������� T ��������� n x N: ���� i-������� pattern ��������� � text � �������� j-i+1...j, ������ � ������� 1 �� ������� ( i,j ). ����� 0.
		����� ������� ������� ��� ��������� ������� j( ��������� T( j ) ): T( j ) = ( ( T( j-1 ) << 1 ) | 00...001 ) & vectorMask[text[j]], ��� j �� 0 �� N - 1.
		�( -1 ) ������� �������� ������.
		vectorMask - ��� �� ������� n x |A|, ��� |A| - ������ ��������. � �� � ������� i �� ������� j ����� 1, ���� pattern[j] = 'i', ��� 'i' - ������ � ����� i.
		������ ������, ���� ������� 1 � ��������� ������ ���� �������.
		��� ��� ��� ������ �� 1 ����� �� �������������� ����� �������� 0, ����� ������ ������ 1 � 0.
		����� ������� ������� T( j ) �������� ���: T( j ) = ( T( j-1 ) << 1 ) & vectorMaskNeg[text[j]].
*/

size_t rightCode( char a )
{
	/*
		������� �������� �������� ����� ������������� ���, ������� �������� �������� �������, ����� ��������� �� ������� ��������
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
		� ���� ������� ��������� vectorMaskNeg - vectorMask �� �������� � ���������� ������ 1 � 0.
		��� ����� ������������ ��� ��������� ������, � ������� ����� ������ - ��� �������, � �����
		������ - ������� � pattern. ����� � ��������, ��� �� ������� ( i,j ) ����� 0, ���� pattern[j] = 'i', ��� 'i' - ������ � ����� i.

		� �� ������, ��� ��� ������� �������, ������ ��� � ��� � ������� ����� ����� �����, ������� ����� �� ������������, �� �����
		���� �� ��������.
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
	׸���� ����� ������� bitap( shift-or ). �� ��� ������ ����� ������ ���� ������,
	�� �� ������, ��� ��� ����� �������� � ��������� ������
*/
uint64_t bitap::exact_search::subSrtinginString
( 
	const std::string& text,
	const std::string& pattern
)
{
	/*
		������ ����������:
			��� ��� ��� ������ �� 1 ����� �� �������������� ����� �������� 0, �������� 1 � 0 ������.
			�� ���� ������ ������� T( j ) �������� ���: T( j ) = ( T( j-1 ) << 1 ) & vectorMaskNeg[text[j]].
			���� � �� ��������� �������, �� � 64� ��������� ����������� ������������ ������������������ ������� ��������
			����������� ��� ������ � ������� �� ������� 64 ���, � ��� ��� ��� �������� ��������� ����������� ������ � �������
			���������, �� ��� ������������ ������������������ ������ pattern �� ����� ������������ 64 �������.
			��� ��� �� �� ����� �������� �������� � ������, ����� ������������ ���� ������� ������� uint64_t, � �����
			������ ����� ��������� �� ��������� ���, � n-�� ��� �����.
			�� ����� ������� �������, � ����� ��������� � �� ��������, ����������� �������� ������ ���������. ���� � �����-��
			������ ��������� 0 �� "���������" �����, ����������� ����� � ������� ������� ������� �������, � �������� ���������� ������.
	*/

	uint64_t textSize = static_cast< uint64_t >( text.size() );
	uint64_t patternSize = static_cast<uint64_t >( pattern.size() );

	//�������� ������������ �������.
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

	//��� ������� T( -1 )
	uint64_t columnTable = ~0ULL;

	std::vector< std::string > res;

	for ( size_t i = 0; i < textSize; ++i )
	{
		columnTable = ( columnTable << 1 ) | mask[rightCode( text[i] )]; //�� ������� �������� ������ ��������� �������

		//��������� "���������" ���
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
		��� ��������� ������ ����� �������������� �������� ������� ������ ��������� �������:
			��� �������� �������� 1 � 0 ������.
			����� pattern ����� �� �����, ��� m ������ �� ������� �����������. ��� ����� m+1 ����� ������� T.
			����� j ( �� 0 �� N - 1 ) - ����� ������� ������( ����� �������� ), k ( �� 1 �� m )- ����� ������� T.
			����� ��������� ������� j ������� k T( j,k ) ��������� �������:
				q( j,k ) = ( T( j-1, k ) << 1 ) | vecMaskNeg[text[j]];
				insert( j,k ) = q( j,k ) & T( j-1, k-1 );
				delet( j,k ) = q( j,k ) & ( T( j, k-1 ) << 1 );
				replace( j,k ) = q( j,k ) & ( T( j-1, k-1 ) << 1 );
				T( j,k ) = insert( j,k ) & delet( j,k ) & replace( j,k );
			������� j ������� 0 T( j,0 ) ����������� �� �������: T( j,0 ) = ( T( j-1, 0 ) << 1 ) | vecMaskNeg[text[j]];
			������� �( -1, * ) �������� 1.
	*/

	/*
		������ ����������:
			����� ��, ����� �������� �� �� ����� ��������� �����, � � ��������� ���� ������, ����������� �������� ���������.
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

	//������� ������
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
		throw std::runtime_error( "������. ���� �� ������." );
	}

	if ( std::filesystem::is_empty( wayToFile ) )
	{
		throw std::runtime_error( "���� ����." );
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
		���� �����, ��� � � ������ ������ ��������� � ������, ����� ������ �����
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
				������ ������ ��� ���������� ������� ���� ���������� ������� ������ � ������.
			*/
			if ( !( columnTables[j] & ( 1ULL << ( patternSize - 1 ) ) ) )
			{
				res.push_back( i );
				break;//���� �� ���������� ��������� ����� � ��� �� ���������
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

	//������� ������
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


	//������� ������
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
				������ ������ ��� ���������� ������� ���� ���������� ������� ������ � ������.
			*/

			if ( !( columnTables[j] & ( 1ULL << ( patternSize - 1 ) ) ) )
			{
				res.push_back( i );
				break;//���� �� ���������� ��������� ����� � ��� �� ���������
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
		throw std::runtime_error( "������. ���� �� ������." );
	}

	if ( std::filesystem::is_empty( wayToFile ) )
	{
		throw std::runtime_error( "���� ����." );
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
		throw std::runtime_error( "������. ���� �� ������." );
	}

	if ( std::filesystem::is_empty( wayToFile ) )
	{
		throw std::runtime_error( "���� ����." );
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