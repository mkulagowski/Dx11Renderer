#include "Manager.hpp"

int main()
{
	if (Manager::getInstance().init())
		Manager::getInstance().run();
}