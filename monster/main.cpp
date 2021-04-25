#include "mon_app.h"

int main(int argc, char** argv)
{

	// TODO(ck): Memory allocation
	App* app = new App();
	app->init();
	app->run();

	exit(EXIT_SUCCESS);
}