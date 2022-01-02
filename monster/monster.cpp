#include "mon_app.h"

/*
@file:		monster.cpp
@author:	Cole Fidler-Knapp
@date:		April 2021
@desc:		3D Engine for learning game development from scratch

	___  ________ _   _  _____ _____ ___________      _____ _   _ _____ _____ _   _  _____
	|  \/  |  _  | \ | |/  ___|_   _|  ___| ___ \    |  ___| \ | |  __ \_   _| \ | ||  ___|
	| .  . | | | |  \| |\ `--.  | | | |__ | |_/ /    | |__ |  \| | |  \/ | | |  \| || |__
	| |\/| | | | | . ` | `--. \ | | |  __||    /     |  __|| . ` | | __  | | | . ` ||  __|
	| |  | \ \_/ / |\  |/\__/ / | | | |___| |\ \     | |___| |\  | |_\ \_| |_| |\  || |___
	\_|  |_/\___/\_| \_/\____/  \_/ \____/\_| \_|    \____/\_| \_/\____/\___/\_| \_/\____/
*/

int main(int argc, char** argv)
{

	// TODO(ck): Memory allocation
	App* app = new App();
	app->init();
	app->run();

	exit(EXIT_SUCCESS);
}