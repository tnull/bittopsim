#ifndef CONSTANTS
#define CONSTANTS

#define LOG(msg) std::cout << BTCTopologySimulation::getSimClock() << ": " << msg << std::endl
/**
 * The maximum number of peers a new Node gets via bootstrapping
 */
const int MAXSEEDPEERS = 20;

/**
 * The maximum number of peers a new Node connects to
 */
const int MAXCONNECTEDPEERS = 8;

#endif // CONSTANTS
