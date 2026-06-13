#!/usr/bin/env python3
# SPDX-License-Identifier: MIT

# (C) 2026 argmaxin
# Distributed under the terms of MIT license.
# Check LICENSE.MIT.txt accompanying this source
# for additional information.

import sys
from typing import List, Final
from z3 import BitVec, BitVecVal, Solver, If, Or, LShR, unsat

class Wax32Prover:
    W32_TAB: Final[List[int]] = [
        0x80, 0x81, 0x83, 0x84, 0x86, 0x83, 0x86, 0x84, 0x87, 0x85,
        0x81, 0x82, 0x84, 0x85, 0x87, 0x88, 0x81, 0x88, 0x82, 0x88,
        0x82, 0x88, 0x81, 0x88, 0x80, 0x80, 0x83, 0x80, 0x86, 0x80,
        0x88, 0x84, 0x88, 0x83, 0x88, 0x80, 0x82, 0x84, 0x85, 0x87,
        0x80, 0x80, 0x81, 0x80, 0x82, 0x83, 0x80, 0x84, 0, 1,
        2, 3, 4, 5, 6, 7, 8, 9,
        -1, -1, -1, -1, -1, -1, -1,
        10, 11, 12, 13, 14, 15, 16, 17,
        1, 18, 19, 1, 20, 21, 0,
        22, 0, 23, 24, 25, 26, 26, 27,
        28, 29, 30, -1, -1, -1, -1, 31,
        -1, 10, 11, 12, 13, 14, 15, 16,
        17, 1, 18, 19, 1, 20, 21, 0,
        22, 0, 23, 24, 25, 26, 26, 27,
        28, 29, 30, -1, -1, -1, -1, -1,
        ord('0'), ord('1'), ord('2'), ord('3'), ord('4'), ord('5'), ord('6'), ord('7'),
        ord('8'), ord('9'), ord('A'), ord('B'), ord('C'), ord('D'), ord('E'), ord('F'),
        ord('G'), ord('H'), ord('J'), ord('K'), ord('M'), ord('N'), ord('P'), ord('R'),
        ord('S'), ord('T'), ord('V'), ord('W'), ord('X'), ord('Y'), ord('Z'), ord('_'),
        5, 2, 5, 4, 1, 5, 3, 5,
        3, 6, 1, 4, 7, 2, 5, 0,
        0, 0, 1, 1, 2, 3, 3, 4,
        0, 1, 1, 2, 3, 3, 4, 4
    ]

    def __init__(self, timeout_ms: int = 15000):
        self.tab = [b & 0xFF for b in self.W32_TAB]
        self.timeout_ms = timeout_ms

    def _get_solver(self) -> Solver:
        s = Solver()
        s.set("timeout", self.timeout_ms)
        return s

    def encode(self, src: List[BitVec], out_len: int) -> List[BitVec]:
        enc = []
        for k in range(out_len):
            id0, id1 = self.tab[176 + k], self.tab[184 + k]
            shl, shr = self.tab[160 + k], self.tab[168 + k]
            val = (src[id0] << BitVecVal(shl, 8)) | LShR(src[id1], BitVecVal(shr, 8))
            enc.append(val & BitVecVal(0x1F, 8))
        return enc

    def decode(self, enc: List[BitVec], out_len: int) -> List[BitVec]:
        dst = []
        for j in range(out_len):
            did0, did1, did2 = self.tab[0 + j] & 0x7F, self.tab[10 + j] & 0x7F, self.tab[25 + j] & 0x7F
            dsl0, dsl1 = self.tab[5 + j] & 0x7F, self.tab[15 + j] & 0x7F
            dsr1, dsr2 = self.tab[20 + j] & 0x7F, self.tab[30 + j] & 0x7F

            c0 = enc[did0] << dsl0
            c1 = If(dsl1 != 8, enc[did1] << dsl1, BitVecVal(0, 8))
            c2 = If(dsr1 != 8, LShR(enc[did1], BitVecVal(dsr1, 8)), BitVecVal(0, 8))
            c3 = If(dsr2 != 8, LShR(enc[did2], BitVecVal(dsr2, 8)), BitVecVal(0, 8))

            dst.append(c0 | c1 | c2 | c3)
        return dst

    def check_alphabet(self) -> bool:
        """Verifies mapping between 5-bit integers and the ASCII table."""
        for s in range(32):
            lte = self.tab[128 + s]
            ltd = self.tab[lte & 0x7F]
            if (ltd & 0x80) or (ltd != s):
                return False
        return True

    def check_bounds(self) -> bool:
        """Verifies that the encoder output strictly conforms to [0, 31]."""
        s = self._get_solver()
        x = [BitVec(f"x_{i}", 8) for i in range(5)]
        y = self.encode(x, 8)
        
        s.add(Or([v > 31 for v in y]))
        return s.check() == unsat

    def check_invertibility(self) -> bool:
        """Verifies D(E(x)) == x across all valid block lengths (1-5 bytes)."""
        for k in range(1, 6):
            s = self._get_solver()
            n = self.tab[35 + k] & 0x7F if k < 5 else 8
            
            x = [BitVec(f"x_{k}_{i}", 8) if i < k else BitVecVal(0, 8) for i in range(5)]
            y = self.encode(x, n)
            
            y_padded = [y[i] if i < n else BitVecVal(0, 8) for i in range(8)]
            x_prime = self.decode(y_padded, 5)

            s.add(Or([x[i] != x_prime[i] for i in range(k)]))
            if s.check() != unsat:
                return False
        return True

def main():
    prover = Wax32Prover()
    
    checks = [
        ("Alphabet Mapping", prover.check_alphabet),
        ("Encoder Bounds", prover.check_bounds),
        ("Codec Invertibility", prover.check_invertibility)
    ]
    
    print("Running WAX32 SMT Proofs...")
    for name, check in checks:
        print(f"[{name}] ", end="", flush=True)
        if check():
            print("PASS")
        else:
            print("FAIL")
            sys.exit(1)
            
    print("All proofs passed.")

if __name__ == "__main__":
    main()
