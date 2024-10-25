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
		������ ������� ��������� ���������� ����������� ��� ������
		��������� ������� - ������.

		�����������: d(x, y) - ���������� ����������� ����� �������� x � y.
		
		���� � ���, ��� �� ��������� �������� �������, � ������� �� �������(i, j) ���������
		���������� �������� (����) ��� �������� �� �������� ������ i ����� �
		� �������� ������ j ����� y. � ������ ������ ���� ���� �������,
		�� ������� (leftSize, rightSize), ����� �����, ���������� ����������� �����������.
	*/
	
	uint32_t leftSize{ static_cast< uint32_t >( left.size() ) };
	uint32_t rightSize{ static_cast< uint32_t >( right.size() ) };

	/*
		�������� ���������� ������������ ���������������� ������������ ����� �������,
		������� ������� ������������ ������� ������� �����,
		����������� � ����� ������ ������������ ������ ������� ������� � ������� ������ �����.
		�������������, �������� ��������������� ����� �� ������ ������� � �������� ������� ������� �����
		�������������� �������.
	*/
	if ( leftSize > rightSize )
	{
		return LevenshteinDistance( right, left );
	}

	/*
		��� ���������� ���������� �����������
		� ������ ���������� ������� ������ 2 ������ �������.
	*/

	std::vector<int32_t> prevLine( rightSize + 1 );
	std::vector<int32_t> currentLine( rightSize + 1 );

	for ( uint32_t i{ 0 }; i <= rightSize; ++i )
	{
		currentLine[i] = i;
	}

	// "����" �������.
	int32_t insertCost{ 0 };

	// "����" ��������.
	int32_t deleteCost{ 0 };

	// "����" ������
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
				�������� ����������� �� ���� � �������� ���������� (���������� ������� ��������
				��� �������������� ������ �������� � ������) ����� ����� ����������.
			*/
			currentLine[j] = std::min( { insertCost, deleteCost, replaceCost } );
		}
	}

	return currentLine.back();
}

uint32_t metrics::LevenshteinDamerauDistance( const std::string& left, const std::string& right )
{
	/*
		�������� ���������� ���������� ����������� - ������� ����������
		��������� ���������� ���������� ���������� �� ����������� �������������
		����� ������������ �������� � �������� ������� ��������.
	*/
	
	uint32_t leftSize{ static_cast< uint32_t >( left.size() ) };
	uint32_t rightSize{ static_cast< uint32_t >( right.size() ) };

	if ( leftSize > rightSize )
	{
		return LevenshteinDamerauDistance( right, left );
	}
	
	/*
		���� ������������ ����������, �� ���������� ������ ������������ ����,
		����� ��� ����� �� �����������. ����� ���� ������������� ����� ������ �������.
	*/
	const uint32_t MAXCOST{ rightSize + 2 };

	// � ������ ������ � ������ ����������� ������� 3 ������� �������

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

			// ������������ ����������� ������ ���� ��� �������� � �������� ������� ���������� �����.
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
