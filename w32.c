/* SPDX-License-Identifier: MIT */

/******************************************************************************\
* +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+= *
* +=                                                                        += *
* +=  MIT License                                                           += *
* +=                                                                        += *
* +=  Copyright (c) 2026 argmaxin                                           += *
* +=                                                                        += *
* +=  Permission is hereby granted, free of charge, to any person           += *
* +=  obtaining a copy of this software and associated documentation        += *
* +=  files (the "Software"), to deal in the Software without               += *
* +=  restriction, including without limitation the rights to use, copy,    += *
* +=  modify, merge, publish, distribute, sublicense, and/or sell copies    += *
* +=  of the Software, and to permit persons to whom the Software is        += *
* +=  furnished to do so, subject to the following conditions:              += *
* +=                                                                        += *
* +=  The above copyright notice and this permission notice shall be        += *
* +=  included in all copies or substantial portions of the Software.       += *
* +=                                                                        += *
* +=  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,       += *
* +=  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF    += *
* +=  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND                 += *
* +=  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS   += *
* +=  BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN    += *
* +=  ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN     += *
* +=  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE      += *
* +=  SOFTWARE.                                                             += *
* +=                                                                        += *
* +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+= *
\******************************************************************************/

#include "w32.h"
#include <stddef.h>

#undef W32_API

#if defined(__has_attribute) && __has_attribute(visibility)
#define W32_API __attribute__((__visibility__("default"))) extern
#endif

#ifdef _WIN32
#define W32_API __declspec(dllexport) extern
#endif

#ifndef W32_API
#define W32_API extern
#endif

#if defined(__has_attribute) && __has_attribute(aligned)
#define W32_ALIGNED(x) __attribute__((aligned(x)))
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#include <stdalign.h>
#define W32_ALIGNED(x) alignas(x) /* Okay for C23 too */
#else
#define W32_ALIGNED(x) /* EMPTY */
#endif

typedef unsigned char uc;

/* clang-format off */ 
#define MSB(x) ((uc)((x) | 0x80)) 
#define T_DID0 MSB(0), MSB(1), MSB(3), MSB(4), MSB(6) 
#define T_DSL0 MSB(3), MSB(6), MSB(4), MSB(7), MSB(5) 
#define T_DID1 MSB(1), MSB(2), MSB(4), MSB(5), MSB(7) 
#define T_DSL1 MSB(8), MSB(1), MSB(8), MSB(2), MSB(8) 
#define T_DSR1 MSB(2), MSB(8), MSB(1), MSB(8), MSB(0) 
#define T_DID2 MSB(0), MSB(3), MSB(0), MSB(6), MSB(0) 
#define T_DSR2 MSB(8), MSB(4), MSB(8), MSB(3), MSB(8) 
#define T_EEXP MSB(0), MSB(2), MSB(4), MSB(5), MSB(7) 
#define T_DEXP MSB(0), MSB(0), MSB(1), MSB(0), MSB(2), MSB(3), MSB(0), MSB(4) 
#define T_LTD \
   0,  1,  2,  3,  4,  5,  6,  7, \
   8,  9, -1, -1, -1, -1, -1, -1, \
  -1, 10, 11, 12, 13, 14, 15, 16, \
  17,  1, 18, 19,  1, 20, 21,  0, \
  22,  0, 23, 24, 25, 26, 26, 27, \
  28, 29, 30, -1, -1, -1, -1, 31, \
  -1, 10, 11, 12, 13, 14, 15, 16, \
  17,  1, 18, 19,  1, 20, 21,  0, \
  22,  0, 23, 24, 25, 26, 26, 27, \
  28, 29, 30, -1, -1, -1, -1, -1 

#define T_LTE \
  '0', '1', '2', '3', '4', '5', '6', '7', \
  '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', \
  'G', 'H', 'J', 'K', 'M', 'N', 'P', 'R', \
  'S', 'T', 'V', 'W', 'X', 'Y', 'Z', '_' 

#define T_SHL 5, 2, 5, 4, 1, 5, 3, 5 
#define T_SHR 3, 6, 1, 4, 7, 2, 5, 0 
#define T_ID0 0, 0, 1, 1, 2, 3, 3, 4 
#define T_ID1 0, 1, 1, 2, 3, 3, 4, 4 

W32_ALIGNED(64) static const uc w32tbl[192] = { 
  T_DID0, 
  T_DSL0, 
  T_DID1, 
  T_DSL1, 
  T_DSR1, 
  T_DID2, 
  T_DSR2, 
  T_EEXP, 
  T_DEXP, 
  T_LTD, 
  T_LTE, 
  T_SHL, 
  T_SHR, 
  T_ID0, 
  T_ID1 
}; 

#undef MSB
/* clang-format on */

#define W32_LTD(x) w32tbl[(x) & 0x7f]
#define W32_DID0(x) (w32tbl[0 + (x)] & 0x7f)
#define W32_DSL0(x) (w32tbl[5 + (x)] & 0x7f)
#define W32_DID1(x) (w32tbl[10 + (x)] & 0x7f)
#define W32_DSL1(x) (w32tbl[15 + (x)] & 0x7f)
#define W32_DSR1(x) (w32tbl[20 + (x)] & 0x7f)
#define W32_DID2(x) (w32tbl[25 + (x)] & 0x7f)
#define W32_DSR2(x) (w32tbl[30 + (x)] & 0x7f)
#define W32_EEXP(x) (w32tbl[35 + (x)] & 0x7f)
#define W32_DEXP(x) (w32tbl[40 + (x)] & 0x7f)
#define W32_LTE(x) w32tbl[128 + (x)]
#define W32_SHL(x) w32tbl[160 + (x)]
#define W32_SHR(x) w32tbl[168 + (x)]
#define W32_ID0(x) w32tbl[176 + (x)]
#define W32_ID1(x) w32tbl[184 + (x)]

W32_API int w32enc(char const *s, size_t *l, char *d, size_t *z) {
  size_t i = 0, j = 0;
  size_t src_len = *l, dst_len = *z;
  size_t rem;
  while (i + 5 <= src_len && j + 8 <= dst_len) {

    int k;

    for (k = 0; k < 8; ++k) {
      uc val = (uc)((((uc)s[i + W32_ID0(k)] << W32_SHL(k)) |
                     ((uc)s[i + W32_ID1(k)] >> W32_SHR(k))) &
                    0x1f);
      d[j + k] = (char)W32_LTE(val);
    }

    i += 5;
    j += 8;
  }

  rem = src_len - i;
  if (rem > 0 && rem < 5) {
    size_t need = W32_EEXP(rem);
    uc tin[5] = {0};
    size_t k;

    if (dst_len - j >= need) {
      for (k = 0; k < rem; ++k) {
        tin[k] = (uc)s[i + k];
      }

      for (k = 0; k < need; ++k) {
        uc val = (uc)(((tin[W32_ID0(k)] << W32_SHL(k)) |
                       (tin[W32_ID1(k)] >> W32_SHR(k))) &
                      0x1f);
        d[j + k] = (char)W32_LTE(val);
      }

      i += rem;
      j += need;
    }
  }

  *l = i;
  *z = j;
  return (i == src_len) ? 0 : 1;
}

W32_API int w32dec(char const *s, size_t *l, char *d, size_t *z) {
  size_t i = 0, j = 0;
  size_t src_len = *l, dst_len = *z;
  size_t rem;
  while (i + 8 <= src_len && j + 5 <= dst_len) {
    uc v[8];
    uc err = 0;
    int k;

    for (k = 0; k < 8; ++k) {
      uc c = (uc)s[i + k];
      err |= c;
      v[k] = W32_LTD(c);
      err |= v[k];
    }

    if (err & 0x80) {
      *l = i;
      *z = j;
      return -1;
    }

    for (k = 0; k < 5; ++k) {
      d[j + k] = (char)(uc)((v[W32_DID0(k)] << W32_DSL0(k)) |
                            (v[W32_DID1(k)] << W32_DSL1(k)) |
                            (v[W32_DID1(k)] >> W32_DSR1(k)) |
                            (v[W32_DID2(k)] >> W32_DSR2(k)));
    }

    i += 8;
    j += 5;
  }

  rem = src_len - i;
  if (rem > 0 && rem < 8) {
    uc tin[8] = {0};
    uc err = 0;
    size_t k;
    size_t need = W32_DEXP(rem);

    if (rem == 1 || rem == 3 || rem == 6) {
      *l = i;
      *z = j;
      return -1;
    }

    if (dst_len - j >= need) {
      for (k = 0; k < rem; ++k) {
        uc c = (uc)s[i + k];
        uc val = W32_LTD(c);
        err |= c;
        tin[k] = val;
        err |= val;
      }

      if (err & 0x80) {
        *l = i;
        *z = j;
        return -1;
      }

      for (k = 0; k < need; ++k) {
        d[j + k] = (char)(uc)((tin[W32_DID0(k)] << W32_DSL0(k)) |
                              (tin[W32_DID1(k)] << W32_DSL1(k)) |
                              (tin[W32_DID1(k)] >> W32_DSR1(k)) |
                              (tin[W32_DID2(k)] >> W32_DSR2(k)));
      }

      i += rem;
      j += need;
    }
  }

  *l = i;
  *z = j;
  return (i == src_len) ? 0 : 1;
}
