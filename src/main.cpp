/**
 * @file main.cpp
 * @brief Minimal RAMP usage example.
 *
 * Builds as `ramp_exec`. Demonstrates `calloc` + #ramp::RAMP::add;
 * cleanup runs in @ref ramp::RAMP::~RAMP.
 */

#include "ramp/lib/RAMP.hpp"

#include <cstdlib>

/**
 * @brief Trivial demo POD registered with RAMP via `calloc` / `free`.
 */
struct User
{
	char *name;    /**< Given name (unused in the example). */
	char *surname; /**< Family name (unused in the example). */

	int age;       /**< Age (unused in the example). */
};

/**
 * @brief Example entry point: register one heap `User` and rely on ~RAMP.
 *
 * @param argc Argument count (unused).
 * @param argv Argument vector (unused).
 * @return Always `0`.
 */
int main(int argc, char **argv)
{
	(void)argc;
	(void)argv;

	ramp::RAMP ramp;
	ramp.enableReverse = true;

	User *a = static_cast<User*>(calloc(1, sizeof(User)));

	ramp.add(a);

	return 0;
}
