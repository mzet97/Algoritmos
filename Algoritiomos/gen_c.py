#!/usr/bin/env python3
import os
import sys
import re

base = 'E:/TI/git/Algoritmos/Algoritiomos'
c_base = f'{base}/c'
cpp_base = f'{base}/C_c_plus_plus/c'
rust_base = f'{base}/rust'

missing = []
for t in [4]:
    for e in range(14, 20):
        p = f'{c_base}/{t}/{e}/main.c'
        if not os.path.exists(p):
            missing.append((t, e))
for t in [5]:
    for e in range(1, 16):
        p = f'{c_base}/{t}/{e}/main.c'
        if not os.path.exists(p):
            missing.append((t, e))
for t in [6]:
    for e in range(1, 8):
        p = f'{c_base}/{t}/{e}/main.c'
        if not os.path.exists(p):
            missing.append((t, e))
for t in [7]:
    for e in range(1, 5):
        p = f'{c_base}/{t}/{e}/main.c'
        if not os.path.exists(p):
            missing.append((t, e))
for t in [8]:
    for e in range(1, 4):
        p = f'{c_base}/{t}/{e}/main.c'
        if not os.path.exists(p):
            missing.append((t, e))

print(f'Total missing: {len(missing)}')
for t, e in missing:
    print(f'  {t}/{e}')
