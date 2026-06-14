/* SPDX-License-Identifier: MIT */
#if !defined(W32_H)
#define W32_H
#include <stddef.h>
#if defined(_WIN32)
#define W32_API __declspec(dllimport) extern
#elif defined(__has_attribute) && __has_attribute(__visibility__)
#define W32_API __attribute__((__visibility__("default"))) extern
#else
#define W32_API extern
#endif 

/*
 * WAX32 Implementation.
 *
 * Character Set: 0123456789ABCDEFGHJKMNPRSTVWXYZ_
 *
 * Rules for decoding:
 * - No padding required.
 * - L / I -> 1
 * - O / Q -> 0
 * - U -> V
 * - LOWER -> UPPER
 *
 * Returns:
 *  - 0: SOURCE exhausted.
 *  - ANY POSITIVE VALUE: DESTINATION exhausted.
 *  - ANY NEGATIVE VALUE: INVALID INPUT.
 *
 * Parameters:
 *  - s: source pointer
 *  - l: source length. Store source bytes consumed.
 *  - d: destination pointer
 *  - z: dst length. Store dst bytes written.
 */

W32_API int w32enc(char const *s, size_t *l, char *d, size_t *z);
W32_API int w32dec(char const *s, size_t *l, char *d, size_t *z);

#endif /* W32_H */
