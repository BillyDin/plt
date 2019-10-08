#include "State.h"
#include <iostream>

namespace state
{

State::State() : playerOneElements {1}, playerTwoElements {2} {
    std::cout << "Creating an state object\n"; 
}

} // namespace state
