# WAX32

This is an implementation of `WAX32` (a base32 variant) under the terms of `MIT` license.
Check accompanying `LICENSE.MIT.txt` for more information.

## Security

- This implementation is not fit for cryptographic usage. Please avoid relying on it for sensitive applications.
- This implementation does not support "overlapping buffers" or any other violation of strict aliasing as per ISO C.
Please do not rely on it for reliable execution.

## Disclaimer - AI USAGE

The following files have been generated/augmented via assistance of AI models:
- `test.c`
- `test.h`

Therefore they are distributed under the terms of `CC0-1.0`.
Check accompanying `LICENSE.CC0.txt` for more information.

## Prerequisites

- an `ISO/IEC 9899:1989` or later compliant (freestanding) toolchain 
- a `POSIX` environment (with `C` development support) [recommended]

## Configuring

Modify the `config.mk` file as needed, and if needed, `Makefile`.

## Building

Run:
- `make shared` for shared library
- `make static` for static library
- `make fuzz` for fuzzer
- `make test` for testing
- `make bench` for bench-marking
- `make smt` for SMT checking
- `make clean` for cleaning-up

## Contribution

As of now, feature/target backend contributions are not accepted. You are advised to fork this repo.
This includes things similar but not limited to: SIMD variant(s), runtime integration(s), JIT integration(s).

However if the concern is of implementation/security posture, you are requested to contribute in any manner you deem fit.
