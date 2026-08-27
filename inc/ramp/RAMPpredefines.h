/**
 * @file RAMPpredefines.h
 * @brief Project-wide macros for versioning, string helpers, and platform detects.
 *
 * Part of pcannonProjectStandards.
 * STD Information: 20250723 - 1.0S
 */

#ifndef INCLUDE_C_RAMPPREDEFINES_H_
#define INCLUDE_C_RAMPPREDEFINES_H_

#include <cstdlib>

/** @name Project setup */
/** @{ */

/**
 * @brief Default C language standard year used by the project template.
 */
#define RAMP_DEFAULT_C_STD			201112L

/** @} */

/** @name Versioning */
/** @{ */

/** @brief Library major version component. */
#define RAMP_VERSION_MAJOR            0

/** @brief Library minor version component. */
#define RAMP_VERSION_MINOR            0

/** @brief Library patch version component. */
#define RAMP_VERSION_PATCH            1

/** @brief Project standards revision number. */
#define RAMP_VERSION_STD              1

/**
 * @brief Version lifecycle state string.
 *
 * Typical values: `"dev"`, `"beta"`, `"build"`.
 */
#define RAMP_VERSION_STATE          "dev"

/**
 * @brief Packed version integer from major/minor/patch/state.
 *
 * Layout: `(state << 24) | (major << 16) | (minor << 8) | patch`.
 */
#define RAMP_VERSION                ((RAMP_VERSION_MAJOR<<16)|(RAMP_VERSION_MINOR<<8)|(RAMP_VERSION_PATCH)|(RAMP_VERSION_STATE << 24))

/**
 * @brief Build a packed version integer from explicit components.
 *
 * @param RAMP_VERSION_MAJOR Major component.
 * @param RAMP_VERSION_MINOR Minor component.
 * @param RAMP_VERSION_PATCH Patch component.
 * @param RAMP_VERSION_STATE State / build-channel component.
 * @return Packed version matching #RAMP_VERSION layout.
 */
#define RAMP_VERSION_CHECK(RAMP_VERSION_MAJOR, RAMP_VERSION_MINOR, RAMP_VERSION_PATCH, RAMP_VERSION_STATE) \
    (((RAMP_VERSION_MAJOR)<<16)|((RAMP_VERSION_MINOR)<<8)|(RAMP_VERSION_PATCH)|((RAMP_VERSION_STATE) << 24))

/** @} */

/** @name Macro utilities */
/** @{ */

/**
 * @brief Stringify a macro argument without expanding it.
 * @param x Token to stringify.
 */
#define RAMP_STRINGIFY(x) #x

/**
 * @brief Expand then stringify a macro argument.
 * @param x Macro or token to expand and stringify.
 */
#define RAMP_TOSTRING(x) RAMP_STRINGIFY(x)

/**
 * @brief Call `free()` on @p x and set the pointer expression to `NULL`.
 * @param x Pointer expression to free and null.
 */
#define RAMP_FREE(x) \
	do { \
		free(x); x = NULL; \
	} while (0); \

/**
 * @brief Call `delete` on @p x and set the pointer expression to `nullptr`.
 * @param x Pointer expression to delete and null (C++).
 */
#define RAMP_DEL(x) \
	do { \
		delete x; x = nullptr; \
	} while (0); \

/** @} */

/**
 * @brief Development-build flag; defaults to `1` when not defined by the build.
 */
#ifndef RAMP_DEV
#   define RAMP_DEV      1
#endif

/** @name Platform detection */
/** @{ */

#if defined(WIN32) || defined(_WIN32)
/**
 * @brief Defined when building for Win32.
 */
#	define RAMP_OS_WIN32
#elif defined(__APPLE__) || defined(__MACH__) || defined(Macintosh)
/**
 * @brief Defined when building for macOS / Mach-based Apple platforms.
 */
#	define RAMP_OS_MACOS
#elif defined(__linux__)
/**
 * @brief Defined when building for Linux.
 */
#	define RAMP_OS_UNIX_LINUX
#elif defined(__unix) || defined(__unix__)
/**
 * @brief Defined when building for a generic Unix (non-Linux, non-FreeBSD).
 */
#	define RAMP_OS_UNIX
#elif defined(__FreeBSD__)
/**
 * @brief Defined when building for FreeBSD.
 */
#	define RAMP_OS_FREEBSD
#else
#	error "Current platform might not supported"
#endif /* defined(WIN32) - platform check */

/** @} */

#endif  /* INCLUDE_C_RAMPPREDEFINES_H_ */
