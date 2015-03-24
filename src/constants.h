#ifndef CONSTANTS
#define CONSTANTS

#include <iomanip>

#define LOG(msg) std::cout << std::setw(20) << Simulation::getSimClock() << ": " << msg << std::endl

/*!
 * The maximum number of peers a new Node gets via bootstrapping
 */
const unsigned int MAXSEEDPEERS = 20;

/*!
 * The maximum number of peers a new Node can have to
 */
const unsigned int MAXCONNECTEDPEERS = 125;

/*!
 * The maximum number of outbound connections a client creates
 */
const unsigned int MAXOUTBOUNDPEERS = 8;

#endif // CONSTANTS
