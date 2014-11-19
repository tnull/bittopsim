#include <iostream>

using namespace std;

bool AppInit( int argc, char* argv[] )
{
	cout << "Starting bctopologysim" << endl;
	return true;
}

int main(int argc, char** argv) 
{
	if (!AppInit(argc, argv)) return -1;
	return 0;
}
