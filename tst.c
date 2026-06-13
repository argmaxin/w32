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
#include "test.h"
#include "w32.h"
#include <stddef.h>

#define TEST_ITERS 1 << 28

static unsigned int w32_test_prng(unsigned int *state) {
  unsigned int x = *state;
  x ^= x << 13;
  x ^= x >> 17;
  x ^= x << 5;
  return *state = x;
}

static long test_exhaustive_roundtrip(void) {
  unsigned int prng_state = 0x1337BEEF;
  char src[5];
  char enc[8];
  char dec[5];
  int i, j;
  for (i = 0; i < TEST_ITERS; ++i) {
	size_t src_len = 5, enc_len = 8, dec_len, dst_len;
    int enc_res, dec_res;
	for (j = 0; j < 5; ++j) {
      src[j] = (char)(w32_test_prng(&prng_state) & 0xFF);
    }

    enc_res = w32enc(src, &src_len, enc, &enc_len);

    TEST_ASSERT(enc_res == 0);
    TEST_ASSERT(src_len == 5);
    TEST_ASSERT(enc_len == 8);

    dec_len = 8;
    dst_len = 5;
    dec_res = w32dec(enc, &dec_len, dec, &dst_len);

    TEST_ASSERT(dec_res == 0);
    TEST_ASSERT(dec_len == 8);
    TEST_ASSERT(dst_len == 5);

    TEST_ASSERT(src[0] == dec[0]);
    TEST_ASSERT(src[1] == dec[1]);
    TEST_ASSERT(src[2] == dec[2]);
    TEST_ASSERT(src[3] == dec[3]);
    TEST_ASSERT(src[4] == dec[4]);
  }

  return TEST_OK;
}

static long test_poison_string_resistance(void) {
  unsigned int prng_state = 0xDEADBEEF;
  const char base32_chars[] = "0123456789ABCDEFGHJKMNPRSTVWXYZ_";
  char enc[8];
  char dec[5];
  int i, j;
  for (i = 0; i < TEST_ITERS; ++i) {
	int poison_idx, dec_res;
	char poison_char;
	size_t dec_len = 8, dst_len = 5;

    for (j = 0; j < 8; ++j) {
      enc[j] = base32_chars[w32_test_prng(&prng_state) % 32];
    }

    poison_idx = (int)(w32_test_prng(&prng_state) % 8);

    do {
      poison_char = (char)(w32_test_prng(&prng_state) & 0xFF);
    } while (poison_char == '_' || (poison_char >= '0' && poison_char <= '9') ||
             (poison_char >= 'A' && poison_char <= 'Z') ||
             (poison_char >= 'a' && poison_char <= 'z'));

    enc[poison_idx] = poison_char;
    dec_res = w32dec(enc, &dec_len, dec, &dst_len);
    TEST_ASSERT(dec_res == -1);
  }

  return TEST_OK;
}

static long test_remainder_bounds_matrix(void) {
  unsigned int prng_state = 0xCAFEBABE;
  char src[5];
  char enc[16];
  char dec[5];

  size_t rem, i, j;

  for (rem = 1; rem < 5; ++rem) {
    for (i = 0; i < TEST_ITERS; ++i) {
      size_t src_len, enc_len, dec_len, dst_len;
	  int enc_res, dec_res;
	  for (j = 0; j < rem; ++j) {
        src[j] = (char)(w32_test_prng(&prng_state) & 0xFF);
      }

      src_len = rem;
      enc_len = 16;
      enc_res = w32enc(src, &src_len, enc, &enc_len);

      TEST_ASSERT(enc_res == 0);
      TEST_ASSERT(src_len == rem);

      dec_len = enc_len;
      dst_len = 5;
      dec_res = w32dec(enc, &dec_len, dec, &dst_len);

      TEST_ASSERT(dec_res == 0);
      TEST_ASSERT(dec_len == enc_len);
      TEST_ASSERT(dst_len == rem);

      for (j = 0; j < rem; ++j) {
        TEST_ASSERT(src[j] == dec[j]);
      }
    }
  }

  return TEST_OK;
}

TEST_GROUP(wax32_codec_suite) {
  struct tests *t = test_group_begin("wax32_codec_suite");

  test_add(t, "exhaustive_roundtrip", test_exhaustive_roundtrip);
  test_add(t, "poison_string_resistance", test_poison_string_resistance);
  test_add(t, "remainder_bounds_matrix", test_remainder_bounds_matrix);

  test_group_end(t);
  return t;
}

int main(int argc, char** argv) {
	return test_run_all(argc, argv);
}
