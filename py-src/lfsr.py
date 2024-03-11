#! /usr/bin/env python3

""" LFSR の定義ファイル

:file: lfsr.py
:author: Yusuke Matsunaga (松永 裕介)
:copyright: Copyright (C) 2024 Yusuke Matsunaga, All rights reserved.
"""

class LFSR:
    """LFSR(Linear Feedback Shift Register) を表すクラス
    """
    
    def __init__(self, n, taps):
        self._bits = [ 1 for _ in range(n) ]
        self._taps = taps

    def set(self, bits):
        """値を設定する．

        :param list[int(0 or 1)] bits: 値の配列(リスト)
        """
        n = len(bits)
        assert n == len(self._bits)
        for i in range(n):
            v = bits[i]
            assert v == 0 or v == 1
            self._bits[i] = v

    def shift(self):
        """次の値を生成する．
        """
        v0 = 1
        for i in self._taps:
            v0 ^= self._bits[i]
        n = len(self._bits)
        for i in range(n - 1, 0, -1):
            self._bits[i] = self._bits[i - 1]
        self._bits[0] = v0

    @property
    def bits(self):
        """値を得る．
        """
        return self._bits[:]


if __name__ == '__main__':

    #n = 100
    #lfsr = LFSR(n, [99, 62])
    #n = 3
    #lfsr = LFSR(n, [2, 1])
    #n = 16
    #lfsr = LFSR(n, [15, 14, 12, 3])
    n = 32
    lfsr = LFSR(n, [31, 21, 1, 0])
    lfsr.set([ 1 if i == 0 else 0 for i in range(n)])

    bit_set = set()
    while True:
        pat = tuple(lfsr.bits)
        if pat in bit_set:
            break
        bit_set.add(pat)
        #print(f'{lfsr.bits}')
        lfsr.shift()

    print(f'Total {len(bit_set)} patterns')
