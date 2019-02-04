#include "Manager.hpp"

int main()
{
	if (Manager::GetInstance().Init())
		Manager::GetInstance().Run();
}