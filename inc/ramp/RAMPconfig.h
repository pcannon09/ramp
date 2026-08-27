/**
 * @file RAMPconfig.h
 * @brief C / C++ linkage helpers for shared headers.
 *
 * Expands to `extern "C" { ... }` when compiled as C++, and to nothing in C,
 * so the same header can expose a C-compatible API from both languages.
 */

#ifndef INCLUDE_C_RAMP_CONFIGCONFIG_H_
#define INCLUDE_C_RAMP_CONFIGCONFIG_H_

#ifdef __cplusplus
/**
 * @brief Opens an `extern "C"` block when compiling as C++.
 */
# 	define RAMP_CONFIG_CPP_OPEN 		extern "C" {
/**
 * @brief Closes an `extern "C"` block when compiling as C++.
 */
# 	define RAMP_CONFIG_CPP_CLOSE 		}
#else
/**
 * @brief No-op in C; pair with #RAMP_CONFIG_CPP_CLOSE.
 */
# 	define RAMP_CONFIG_CPP_OPEN
/**
 * @brief No-op in C; pair with #RAMP_CONFIG_CPP_OPEN.
 */
# 	define RAMP_CONFIG_CPP_CLOSE
#endif

RAMP_CONFIG_CPP_OPEN



RAMP_CONFIG_CPP_CLOSE 

#endif  // INCLUDE_C_RAMP_CONFIGCONFIG_H_
