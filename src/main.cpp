#include <iostream>
#include <Scop.hpp>

int	main(int ac, char **av) {
	try {
		Scop scop = Scop(ac, av);
		scop.gameLoop();
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}
}
