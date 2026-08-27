# RAMP

Resource Acquisition && Management Primitives

A small C++17 helper that tracks heap pointers and runs a destructor for each
when you remove them or when the RAMP object goes out of scope.

Version: 1.0.0-dev

## What it does

- Register pointers with `add()`
- Free one early with `remove()`
- Free all remaining on `destroy()` or in the destructor
- Choose cleanup order with `enableReverse`

Default cleanup (no custom destructor):

- `void*` or trivially destructible `T*` -> `free()` (`RAMP_FREE`)
- non-trivial `T*` -> `delete` (`RAMP_DEL`)

If you allocate a trivial type with `new`, pass an explicit destructor.

## Requirements

- C++17
- CMake 3.24+
- Ninja (optional, used by the project build scripts)

## Build setup
```bash
./gen.sh
./gen.sh link
```

## Build

```bash
./compile.sh setup
./compile.sh
```

Or manually:

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

Outputs:

- static library: `build/lib/libramp.a`
- example binary: `build/bin/ramp_exec` (also linked as `./run.sh`)

## Usage

```cpp
#include "ramp/lib/RAMP.hpp"

#include <cstdlib>

struct User
{
	char *name;
	char *surname;
	int age;
};

int main()
{
	ramp::RAMP ramp;
	ramp.enableReverse = true; /* LIFO destroy order */

	User *a = static_cast<User*>(calloc(1, sizeof(User)));
	ramp.add(a); /* uses free() for this trivial type */

	/* Custom destructor (optional):
	 * ramp.add(a, [](User *p){ RAMP_FREE(p); });
	 */

	/* Early cleanup (optional):
	 * ramp.remove(a);
	 */

	return 0; /* ~RAMP destroys anything still registered */
}
```

## API summary

| Member | Description |
|--------|-------------|
| `enableReverse` | `false`: FIFO destroy/remove search. `true`: LIFO. |
| `add(ptr)` | Register pointer; pick `free` or `delete` by type. |
| `add(ptr, dtor)` | Register pointer with an explicit destructor. |
| `remove(ptr)` | Find pointer, run its destructor, erase it. |
| `deleteElem(i)` | Destroy and erase by index. |
| `destroy()` | Destroy every registered resource. |
| `~RAMP()` | Calls `destroy()`. |

All mutating helpers return `bool` (`true` on success).

## Layout

```
inc/ramp/
  RAMPpredefines.h   macros, version, platform checks
  RAMPconfig.h	   C/C++ linkage helpers
  lib/RAMP.hpp	   RAMP class (templates inline)
src/
  lib/RAMP.cpp	   RAMP non-template members
  main.cpp		   example executable
```

