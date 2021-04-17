#include "mon_app.h"

int main(int argc, char** argv)
{
	App* app = new App();
	app->init();
	app->run();


	exit(EXIT_SUCCESS);
}