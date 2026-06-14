/* SPDX-License-Identifier: MIT */

/*
  Copyright (c) 2026 argmaxin

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE. 
*/

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
