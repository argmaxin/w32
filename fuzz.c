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
#include <string.h>
#include <stdlib.h>

int LLVMFuzzerTestOneInput(const unsigned char *Data, size_t Size) {
  size_t enc_src_len;
  size_t enc_max_len;
  char *enc_out;
  size_t enc_written;
  int enc_res;

  size_t dec_max_len;
  char *dec_out;
  size_t dec_src_len;
  size_t dec_written;

  if (Size == 0 || Size > 4096) {
    return 0;
  }

  enc_src_len = Size;
  enc_max_len = ((Size * 8) / 5) + 64;
  enc_out = (char *)malloc(enc_max_len);
  if (!enc_out) {
    return 0;
  }

  enc_written = enc_max_len;
  enc_res = w32enc((const char *)Data, &enc_src_len, enc_out, &enc_written);

  dec_max_len = ((Size * 5) / 8) + 64;
  dec_out = (char *)malloc(dec_max_len);
  if (!dec_out) {
    free(enc_out);
    return 0;
  }

  dec_src_len = Size;
  dec_written = dec_max_len;

  w32dec((const char *)Data, &dec_src_len, dec_out, &dec_written);

  if (enc_res == 0 && enc_src_len == Size) {
    size_t rt_src_len = enc_written;
    size_t rt_max_len = Size + 64;
    char *rt_out = (char *)malloc(rt_max_len);

    if (rt_out) {
      size_t rt_written = rt_max_len;
      int rt_res = w32dec(enc_out, &rt_src_len, rt_out, &rt_written);

      if (rt_res != 0 || rt_src_len != enc_written) {
        __builtin_trap();
      }

      if (rt_written != Size || memcmp(Data, rt_out, Size) != 0) {
        __builtin_trap();
      }
      free(rt_out);
    }
  }

  free(enc_out);
  free(dec_out);
  return 0;
}
