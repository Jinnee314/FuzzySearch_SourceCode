#pragma once

#include <string>
#include <unordered_set>

namespace extended_sampling 
{
	std::unordered_set< std::string > createWrongWords
	(
		std::string request,
		uint32_t numErrors
	);
}