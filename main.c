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
#include <stdio.h>
#include <string.h>

#define BUF_SIZE 3840
#define WAX_VER "1.0.0"


const char HELP_TXT[] =
    "wax32 encoder/decoder version: " WAX_VER ".\n"
    "(C) argmaxin 2026\n"
    "Licensed under the terms of MIT License.\n"
    "\n"
    "Usage: wax32 [OPTION]... [FILE]\n"
    "WAX32 encode or decode FILE (or standard input), to standard output.\n"
    "\n"
    "When no FILE is provided, or if FILE is '-', read stdin.\n"
    "\n"
    "\t-d,--decode\tdecode data\n"
    "\t-h,--help\tshows this page\n";


static int do_encode(FILE *in, FILE *out) {
  char in_buf[BUF_SIZE];
  char out_buf[((BUF_SIZE * 8) / 5) + 64];
  size_t read_len;

  while ((read_len = fread(in_buf, 1, BUF_SIZE, in)) > 0) {
    size_t src_len = read_len;
    size_t dst_len = sizeof(out_buf);

    if (w32enc(in_buf, &src_len, out_buf, &dst_len) != 0) {
      fprintf(stderr, "Error: Encoding failed\n");
      return 1;
    }

    if (fwrite(out_buf, 1, dst_len, out) != dst_len) {
      return 1;
    }
  }
  return 0;
}


static int do_decode(FILE *in, FILE *out) {
  char in_buf[BUF_SIZE];
  char out_buf[BUF_SIZE + 64];
  size_t read_len;

  while ((read_len = fread(in_buf, 1, BUF_SIZE, in)) > 0) {
    size_t src_len = read_len;
    size_t dst_len = sizeof(out_buf);

    if (w32dec(in_buf, &src_len, out_buf, &dst_len) != 0) {
      fprintf(stderr, "Error: Invalid WAX32 payload\n");
      return 1;
    }

    if (fwrite(out_buf, 1, dst_len, out) != dst_len) {
      return 1;
    }
  }
  return 0;
}


int main(int argc, char **argv) {
  int decode_mode = 0;
  FILE *input = stdin;

  if (argc > 1) {
    if (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help")) {
      printf(HELP_TXT);
      return 0;
    }
    if (!strcmp(argv[1], "-d") || !strcmp(argv[1], "--decode")) {
      decode_mode = 1;
      if (argc > 2 && strcmp(argv[2], "-")) {
        input = fopen(argv[2], "rb");
      }
    } else if (strcmp(argv[1], "-")) {
      input = fopen(argv[1], "rb");
    }
  }

  if (!input) {
    fprintf(stderr, "Error: Cannot open input file\n");
    return 1;
  }

  if (decode_mode)
    return do_decode(input, stdout);
    
  return do_encode(input, stdout);
}
