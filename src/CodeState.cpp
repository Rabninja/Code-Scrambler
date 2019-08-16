#include <vector>
#include <iostream>
#include "CodeState.h"

namespace
{
	std::vector<std::string> states;
}

void pushCodeState(std::string_view state)
{
	std::cout << "[STARTING]:  " << state << std::endl;
	states.push_back(std::string{ state });
}

void popCodeState()
{
	std::cout << "[COMPLETED]: " << states.back() << std::endl << std::endl;
	states.pop_back();
}
