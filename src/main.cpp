#include <iostream>
#include <Scop.hpp>

int	main()
{
	try {
		Scop scop = Scop();
		scop.gameLoop();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
