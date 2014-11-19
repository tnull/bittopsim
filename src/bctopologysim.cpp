/**
 * @mainpage bctopologysim
 *
 * Bitcoin topology simulator
 * @author Elias Rohrer
 */

/**
 * @file bctopologysim.cpp
 *
 * @brief This is where everything starts, the application is initialzied and run.
 * 
 */

#include <iostream>

using namespace std;

/**
 * @brief AppInit is where the application starts
 */
bool AppInit( int argc, char* argv[] )
{
	cout << "Starting bctopologysim" << endl;
	return true;
}

/**
 * @brief main is just there to call AppInit
 */
int main(int argc, char** argv) 
{
	if (!AppInit(argc, argv)) return -1;
	return 0;
}
