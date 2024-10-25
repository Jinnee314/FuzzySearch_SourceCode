#include "combinatorics.hpp"
#include "extended_sampling.hpp"

#include <vector>
#include <fstream>
#include <cstdint>
#include <filesystem>

namespace fs = std::filesystem;

//������ ��������� �����, � ������� ������ - ��� ������ ��������, � ���������� ��� � ������ ���������. 0 < ���������� ��������� �������� <= numErrors.
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
	//uint64_t countReplacment = static_cast<uint64_t>(pow(countLetters, numErrors));//���������� �����
	std::vector<uint32_t> posInString(numErrors);

	/*
		� ������� posInString ������ ���������� �������
		� ������ ��������� �� ��������� ������� ������������� ����������� - ������� ������ ���� �� ��������;
	*/

	for (uint32_t i = 0; i < numErrors; ++i)
	{
		posInString[i] = i;
	}
	/*
		���������� ��� ������ � ���������� ��.
		���� � ���, ��� �� ������� numErrors ������� � �������� ������ � ��������� �� ���� �������� ��� ���������� ��������
		����� ����� ������� ������ numErrors ������� � ��� ��������� ��� ��������� ���������� �� numErrors ������� �� ���� ����� �������� ������.

		������������� �� ��������� �������� ������ ������ ��������. ������ ��� �������� � �����. ����� ���� ������ ��� ��� �������.

		��������� ������� �� ���� ��������, ����� ���������. �� ��������� ������� ���������� ��� ��������� ������� (������ ��� ��������� ����� ������), ����� ����� �� �������������
		������� ������ ������ �� ��������� � ����� ��������� ��� ������� �� ��������� �������. ����� ��������� ��� ������� �� ������������� �������, �� ����������������� ������� ������
		������ �� ��������. � ��� ����� ��������� ��� ��������� ���������� ��������. ����� ���� ������ ����� ������� � ���������.

		����� ������� �������� �� ���� �� ��������: ������� ���������� ���� ���������� �� ��������� �������, ����� ������ ������������� � ����� ������������ ���� ����������.
		����� ������� �������� ��� ��������� �������

		����� �� ����, ���� �� ������� �����, ������� �������� � ������� (�� � ����� ����� ������) �������������� �� ��� �� ������, ��� � �����, ���� � ���������� �����������.

		��� ������� �������� � ���� �� �����������
	*/

	while (posInString[0] != patternSize - numErrors + 1) //���� ������ ������� �� ��������� �� ��������� ��� �� 
	{
		for (uint32_t i = 0; i < numErrors; ++i)//�������������� ������ ������ �������� �� ���� ��������
		{
			workCopy[posInString[i]] = '�';
		}

		//��������� ����� �����
		vectorSelection[posInVectorSelection] = workCopy;
		++posInVectorSelection;

		while (workCopy[posInString[0]] != '�' + 1)//���� ������ �� ������ ������� �� ����� ���������� ������� �������� + 1
		{
			++workCopy[posInString[numErrors - 1]];//������ ������ �� ��������� ������� �� ���������
			
			vectorSelection[posInVectorSelection] = workCopy;
			++posInVectorSelection;

			if (workCopy[posInString[numErrors - 1]] == '�') //���� ������ �� ��������� ������� ����� ���������� ������� ��������
			{
				workCopy[posInString[numErrors - 1]] = '�';//������ ������ �� ��������� ������� �� ���������
				
				if (!(numErrors - 1))	
				{
					break;//���� ���������� ������ = 1, ����� ���� �� �����
				}
				
				++workCopy[posInString[numErrors - 2]];//������ ������ �� ������������� ������� �� ���������
				
				for (int32_t j = numErrors - 2; j > 0; --j)//����������� �� �������� �� ���� �������� �� ����� � ������
				{
					if (workCopy[posInString[j]] == '�' + 1)//���� �� ��������������� ������� ����������� ��������� ������ �������� + 1
					{
						workCopy[posInString[j]] = '�';//������ ��� �� ������
						
						++workCopy[posInString[j - 1]];//� ������ �� ������� �� ���� ������ �� ���������
					}
				}
				
				if (workCopy[posInString[0]] == '�' + 1) 
				{
					break;//���� ������ �� ������ ������� ����� ���������� ������ �������� + 1, �� ��������
				}

				vectorSelection[posInVectorSelection] = workCopy;
				++posInVectorSelection;
			}
		}

		++posInString[numErrors - 1];//������ ��������� �������
		if (posInString[numErrors - 1] == patternSize)//���� ��������� ������� ����� �� ������� ������
		{
			for (int32_t i = numErrors - 2, diff = 1; i > -1; --i, ++diff)//��������� ������ ��� �������
			{
				++posInString[i];
				
				if (posInString[i] != patternSize - diff)//���� i-�� ������� �� ����� ������ ���������� ���������� ��������
				{
					for (uint32_t j = i + 1; j < numErrors; ++j)//�������������� ����������� ��������� ��� �������, ����� i-��
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

//������ ��������� �����, � ������� ������ - ��� ��������(�������) ��������, � ���������� ��� � ������ ���������. 0 < ���������� �������� �������� <= numErrors.
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
	//���������� ���� ��������� �������� �� ���������� ������� <= numErrors
	uint64_t countAllErase = combinatorics::combination(patternSize, numErrors) * static_cast<uint64_t>(pow(2, numErrors));
	
	std::string workCopy = pattern, saveCopy = pattern;
	
	uint64_t countIterErase = countAllErase / bitProof; //���������� �������� �������� �����
	
	std::vector<std::uint32_t> posInString(numErrors);
	
	// � ������ ��������� �� ��������� ������� ������������� ����������� - ������� ������ ���� �� ��������;
	for (uint32_t i = 0; i < numErrors; ++i)// ������ ������� � �������� ���������
	{
		posInString[i] = i;
	}

	/*
		���� � ���, ����� ��������� ��� ��������� �������� �������, � �� ��� ��� ��������� �������� ��������.
		������� ������� ��� � � ������� ���������.
	*/

	for (uint64_t z = 0; z < countIterErase; ++z)
	{
		/*
			���� �������� ��������� �������� � ���, ��� � ������� ������������� i �������� ��� ���������� �� 1 � numErrors ��������
			��� ����� ���� ���������, ����� ���� � i ����� 1 � ������� �������, ������� ������� ����������� � ��������� ���� �����. 
		*/

		for (uint32_t i = 1; i < bitProof; ++i)
		{
			for (uint32_t j = 0; j < numErrors; ++j)//�������� ����� ���� ������ � ���������� �
			{
				if (((i >> j) & 1) == 1)
				{
					workCopy[posInString[j]] = '~'; // ���� ������ ����� �������, ������ ��� �� �������, �� ������� ����� ����������� ��������
				}
			}

			workCopy.erase(std::remove(workCopy.begin(), workCopy.end(), '~'), workCopy.end());//������ ������� ��� ���������� ������� �� ������
			
			vectorSelection[posInVectorSelection] = workCopy;
			++posInVectorSelection;
			
			workCopy = saveCopy;
		}

		//������� �������
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


//������ ��������� �����, � ������� ������ - ��� ������� ��������, � ���������� ��� � ������ ���������. 0 < ���������� ����������� �������� <= numErrors.
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
	
	std::string symbInsert(numErrors, '�');//� ���� ������ ���������� ��� �������� ���������� ��������
	
	std::vector<uint32_t> posInString(numErrors, 0);//�������� ������ �������, � ������ ��������� ������� ����� ���� �����������
	
	/*
		���������� ��� �������, ��� ������ ������� ���������� ��� ��������� ���������� ��������
		��� ������ ���������� �������� ���������� ���������� �� ����������
	*/

	while (posInString[0] != maxPos)//���� �������, ������ � �������, �� ����� ����������� ��������� �������
	{
		for (uint64_t j = 0; j < countElementsForInsert; ++j)//���������� ��� ���������� ��������
		{
			for (uint64_t z = 1; z < bitProof; ++z)//���������� ��� ���������� ����������
			{
				for (uint32_t q = 0; q < numErrors; ++q)//�������� ����� ���� ������ � ���������� �
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

			if (symbInsert[numErrors - 1] == '�' + 1)//���������� ��� ��������� ������� ��� ��, ��� � � ��������� �� �����
			{
				symbInsert[numErrors - 1] = '�';

				if (numErrors - 1)
				{
					++symbInsert[numErrors - 2];

					for (int32_t z = numErrors - 2; z > 0; --z)
					{
						if (symbInsert[z] == '�' + 1)
						{
							symbInsert[z] = '�';
							++symbInsert[z - 1];
						}
					}
				}
			}
		}

		symbInsert = std::string(numErrors, '�');

		++posInString[numErrors - 1];
		if (posInString[numErrors - 1] == maxPos)//������� ���� ��������� ������� ��� � � ������� ����������
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

//������ ��������� �����, � ������� ������ - ��� ������������ ��������, � ���������� ��� � ������ ���������. 0 < ���������� ������������ <= numErrors.
void addAllTransposition
(
	std::vector<std::string>& vectorSelection,
	uint64_t& posInVectorSelection,
	const std::string& pattern, 
	const uint32_t numErrors,
	const uint32_t countPermutationForOneTransposition
)
{
	uint64_t countExternalCycles = 0;//���������� �������� ������� for

	/*
		��� ��� ������� ����� ����� ���� ������������ �� ����� �� ������ ��� ��������� ����, ����� ���������� ������ ����� 
		����� ���� �������� countPermutationForOneTransposition �� numErrors �� ������� numErrors
	*/
	for (uint32_t i = 1; i < numErrors; ++i)
	{
		countExternalCycles += static_cast<uint64_t>(pow(countPermutationForOneTransposition, i));
	}

	vectorSelection[posInVectorSelection] = pattern;//�������� ������� �� ��������� �������� �����, ������� �������� ��� � ������ ��������� �� ������ ������
	
	uint64_t savePos = posInVectorSelection;//� ���������� ��� �������
	
	++posInVectorSelection;
	
	std::string workCopy = pattern;
	
	/*
		���� ����� � savePos � ��������� ���� ������������ � ������ ���� �������� ��������
		���������� �� � ��� ������, � ����� � ���������� ������ ��������� �� �� ��������, � ��������� ��� numErrors ���, ����� �������� ��� �����,
		������� ����� �������� numErrors ��������������
	*/

	for (uint64_t i = 0; i <= countExternalCycles; ++i)
	{
		for (int32_t j = countPermutationForOneTransposition; j > 0; --j)//��������� ������������ � ������ ���� �������� ��������
		{
			workCopy = vectorSelection[savePos + i];

			std::swap(workCopy[j], workCopy[j - 1]);

			vectorSelection[posInVectorSelection] = workCopy;
			++posInVectorSelection;
		}
	}
}


/*
	������ �������
	��� ��� ��������� ������ ������� ������� �� ���������, ����� ��� ������� ������� ������� ����� ������
	��� ����� ������� ����� ��������� ����������� ���������� ������
	��� ��� ����� � set �������� �� log(n), ����� �������� ������� �� ���� ��������, �������� set � ���������������� ��������
	�� �� ������ ��� ����� �������� �������, ������ ����������� ������ � set
*/
std::unordered_set<std::string> extended_sampling::createWrongWords
(
	std::string pattern,
	uint32_t numErrors
)
{
	uint32_t patternSize = static_cast<uint32_t>(pattern.size());
	uint32_t countLetters = '�' - '�' + 1;
	uint64_t sizeVectorSelection = 0, workInt64 = 0;

	//������� ���������� ����� � ������� ��� ���� ��������� �����, ��������, ��� �������� ����� �����������, � ���� ������������ ����������
	sizeVectorSelection = combinatorics::combination(patternSize, numErrors) * static_cast<uint64_t>(pow(countLetters, numErrors));

	//������� ���������� ����� ��� ���� ��������� �������� � ���������� � ����������
	workInt64 = combinatorics::combination(patternSize, numErrors);
	sizeVectorSelection += workInt64 * (static_cast<uint64_t>(pow(2, numErrors)) - 1); //2 - ��� ��� ������ ����� ���� ����� ��� �� �����

	//������� ���������� ����� ��� ���� ��������� ����������
	uint32_t maxPos = patternSize + 1;

	uint64_t countVariantsPos = combinatorics::combinationWithReplace(maxPos, numErrors);
	uint64_t countElementsForInsert = static_cast<uint64_t>(pow(countLetters, numErrors));
	uint64_t bitProof = static_cast<uint64_t>(pow(2, numErrors));

	sizeVectorSelection += countVariantsPos * countElementsForInsert * (bitProof - 1);

	//������� ���������� ����� ��� ���� ��������� ������������
	uint32_t countPermutationForOneTransposition = patternSize - 1;
	
	for (uint32_t i = 0; i <= numErrors; ++i)
	{
		sizeVectorSelection += static_cast<uint64_t>(pow(countPermutationForOneTransposition, i));
	}

	std::vector<std::string> vectorSelection(sizeVectorSelection);
	uint64_t posInVectorSelection = 0;
	std::string workCopy = pattern, saveCopy = pattern;

	//������
	addAllSubstitutions(vectorSelection, posInVectorSelection, pattern, numErrors, patternSize);

	//��������
	addAllErase(vectorSelection, posInVectorSelection, pattern, numErrors, patternSize, bitProof);

	//����������
	addAllInsert(vectorSelection, posInVectorSelection, pattern, numErrors, bitProof, maxPos, countElementsForInsert);

	//������������
	addAllTransposition(vectorSelection, posInVectorSelection, pattern, numErrors, countPermutationForOneTransposition);

	return std::unordered_set<std::string>(vectorSelection.begin(), vectorSelection.end());
}


