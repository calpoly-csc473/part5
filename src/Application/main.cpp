
#include "Application.hpp"


int main(int argc, char ** argv)
{
	Application app;
	app.ReadArguments(argc, argv);

	return app.Run();
}
