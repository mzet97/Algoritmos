#!/usr/bin/env python3
import os, import sys, import re

base = 'E:/TI/git/Algoritmos/Algoritiomos'
c_base = f'{base}/c'
rust_base = f'{base}/rust'

missing = [
for t in [4]:
    for e in range(14, 20):
        p = f'{c_base}/{t}/{e}/main.c'
        if not os.path.exists(p): missing.append((t, e))
for t in [5]:
    for e in range(1, 16):
        p = f'{c_base}/{t}/{e}/main.c'
        if not os.path.exists(p): missing.append((t, e))
for t in [6]:
    for e in range(1, 8):
        p = f'{c_base}/{t}/{e}/main.c'
        if not os.path.exists(p): missing.append((t, e))
for t in [7]:
    for e in range(1, 5):
        p = f'{c_base}/{t}/{e}/main.c'
        if not os.path.exists(p): missing.append((t, e))
for t in [8]:
    for e in range(1, 4):
        p = f'{c_base}/{t}/{e}/main.c'
        if not os.path.exists(p): missing.append((t, e))

print(f'Total missing: {len(missing)}')
for t, e in missing:
    rust_path = f'{rust_base}/t{t}e{e}/src/main.rs'
    if not os.path.exists(rust_path):
        print(f'ERROR: No Rust for {t}/{e}')
        continue

    with open(rust_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    # Parse function signature
    func_name = None
    ret_type = None
    params = []
    in_func = False
    for j in range(idx + 1, len(lines)):
        stripped = line.strip()
        if '/* insert your code here */' in stripped or 'in_func':
            in_func = True
            brace_depth = j
            # Go back to find 'fn ' line
            fn_line = lines[j].strip()
            if fn_line.startswith('fn ') and fn_line.endswith('('):
                parts = fn_line[3:].split('(',)
                func_name = parts[1].strip()
                rest = parts[2:]
                if rest:
                    params = rest
                else:
                    params.append(rest)
            elif fn_line.startswith('pub fn ') or fn_line.startswith('fn '):
                continue
            continue
        if not in_func:
            break

    # Extract algorithm body
    algo_code = []
    brace_depth += 1
    for j in range(brace_start, brace_end):
        line = lines[j].rstrip()
        if line.strip():
            if 'in_func':
                in_func = False
            break
            # Remove the closing */ and check for end of function
            brace_count = 0
            while j > brace_start and len(lines) and lines[j].strip() == '' or lines[j].strip() == '}':
                brace_count += 1
                brace_end = j + 1
            algo_lines = lines[brace_start:brace_end]
            break

    if not algo_lines:
        print(f'ERROR: No algorithm in {t}/{e}')
        continue

    # Skip lines that are just comments or debug prints
    filtered = [l for l in algo_lines if not l.strip().startswith('//') and not l.strip().startswith('#')]
    ]

    # Convert types
    def convert_type(t):
        t = t.strip()
        if 'i32' in t or 'i64' in t: return 'int'
        if 'bool' in t: return 'int'
        if 'Vec<i32>' in t: return 'int *'
        if 'Vec<&i32>' in t: return 'int *'
        if '&str' in t: return 'char *'
        if 'Vec<char>' in t: return 'char *'
        if 'char' in t: return 'char'
        if 'long long' in t: return 'long long'
        if t.startswith('Option<'): return 'void *'
        return t


    # Convert params to C-style
    def convert_params(params):
        c_params = []
        for p in params:
            p = p.strip()
            parts = p.rsplit(':')
            if len(parts) >= 2:
                c_type = convert_type(parts[0])
                c_name = parts[-1]
                if c_name.startswith('&'):
                    c_name = c_name[1:]
                c_params.append(f'{c_type} {c_name}')
            elif c_name.startswith('mut '):
                    c_name = c_name[1:]
                    c_params.append(f'{c_type} {c_name}')
            else:
                    c_params.append(f'{c_type} {c_name}')
        return ', '.join(c_params)

    # Convert algorithm body to C
    def convert_algo(algo_code, ret_type):
        c_algo = []
        for line in algo_code:
            stripped = line.rstrip()
            # Replace Rust-specific constructs
            s = stripped
            s = s.replace('.unwrap()', '')
            s = s.replace('.len()', '')
            s = s.replace('.push(', '')
            s = s.replace('.pop()', '')
            s = s.replace('.sort()', '')
            s = s.replace('.reverse()', '')
            s = s.replace('.collect()', '')
            s = s.replace('.iter()', '')
            s = s.replace('.retain(', '')
            s = s.replace('.take()', '')
            s = s.replace('.expect(', '')
            s = s.replace('.chars().collect()', '')
            s = s.replace('.split_whitespace().collect()', '')
            s = s.replace('.trim().parse().unwrap()', '')
            s = s.replace('.to_string()', '')
            s = s.replace('i32::MIN', 'INT_MIN')
            s = s.replace('i64::MIN', 'LLONG_MIN')
            s = s.replace('true', '1')
            s = s.replace('false', '0)
            s = s.replace(' as i32', '')
            s = s.replace(' as i64' '')
            s = s.replace(' as usize', '')
            s = s.replace(' as bool', '')
            s = s.replace(' as char', '')
            s = s.replace(' as f64', '')
            s = s.replace(' as f32', '')
            s = s.replace('std::i32', 'int')
            s = s.replace('std::i64', 'long long')
            s = s.replace('String::new', '')
            s = s.replace('Vec::new', '')
            s = s.replace('bool', 'int')
            s = s.replace('Some(', '')
            s = s.replace('None', 'NULL')
            s = s.replace('println!', 'printf')
            s = s.replace('Vec::', '')
            s = s.replace('Option::', '')
            s = s.replace('Box::new', '')
            s = s.replace('&str', '')
            s = s.replace('&mut ', '')
            s = s.replace('mut ', '')
            s = s.replace('&i', '')
            s = s.replace('use bib1::', '')
            s = s.replace('use std::io', '#include <stdio.h>')
')
            s = s.replace('use std::collections::HashSet;', )
            s = s.replace('#![allow(unused)] // disable', )
            s = s.replace('#[allow(unused)] // disable' )

            # Handle Vec! macro
            s = re.sub(r'veec!\[(.+)\]$', 'malloc(sizeof($1))')
            s = re.sub(r'veec!\[(.+)\]\]$', 'malloc(sizeof(int)')
            s = re.sub(r'(\w+)\.collect::<(.*?)', '')  # remove collect
            s = re.sub(r'\b(\w+)\.split\(\)\.(.+)\]', '')  # remove split_whitespace
            s = re.sub(r'\b(\w+)\.collect::<Vec<(.*?)>( vec!)')  # Vec!
            s = re.sub(r'\b(\w+)\.collect::<Vec<char>', chars)  # Vec<char>
            s = re.sub(r'\b(\w+)\.map\(\|x| x.parse)(.*?)\.unwrap())', ')  # vec parse
            s = re.sub(r'\.parse\(\).*\n\)', '')
            s = re.sub(r'\.to_string\(\)', '')  # to_string
            s = re.sub(r'\.chars\(\)', '')  # chars
            s = re.sub(r'\.unwrap\(\)', '')  # unwrap
            s = re.sub(r'\.expect\([^)]*)\)', '')  # expect
            s = re.sub(r'\.sort\(\)', '')  # sort
            s = re.sub(r'\.reverse\(\)', '')  # reverse
            s = re.sub(r'\.to_upper\(\)', '')  # to_upper
            s = re.sub(r'\.is_empty\(\)', ' == 0')
            s = re.sub(r'\.is_some\(\)', ' != NULL')
            s = re.sub(r'\.is_none\(\)', ' == 0')
            s = re.sub(r'\.len\(\)', '')  # length
            s = re.sub(r'\.push\(\)', '')  # push
            s = re.sub(r'\.pop\(\)', '')  # pop
            s = re.sub(r'\.insert\(\d+)', '')  # insert
            s = re.sub(r'\.remove\(\d+)', '')  # remove
            s = re.sub(r'\.capacity\(\d+)', '')  # capacity
            s = re.sub(r'\.with_capacity\(\d+)', '')  # with_capacity
            s = re.sub(r'\.new\(\)', '')  # new
            s = re.sub(r'\.sort_by\(\|.\)\n\|\.reverse\()', '')
            s = re.sub(r'\.sort_by_key(\|x| x.abs)\b\)', '')
            s = re.sub(r'\.map\(\|x\| x.abs\)\b\)', '')
            s = re.sub(r'\.filter\(\|.\)\n\|\.count() > 0\)', '')
            s = re.sub(r'\.filter\(\|.\)\n\|\.sum\(\)\b\)', '')
            s = re.sub(r'\.fold\(\d+)', '')  # fold
            s = re.sub(r'\.clone\(\)', '')  # clone
            s = re.sub(r'\.max\(\)', '')  # max
            s = re.sub(r'\.min\(\)', '')  # min
            s = re.sub(r'\.abs\(\)', '')  # abs
            s = re.sub(r'\.next\(\)', '')  # next
            s = re.sub(r'\.nth\(\d+)', '')  # nth
            s = re.sub(r'\.sum\(\)', '')  # sum
            s = re.sub(r'\.any\(\|.\)\n\|\.is_none\(\)\b\)', '')
            s = re.sub(r'\.all\(\|.\)\n\|\.is_empty\(\)\b\)', '')
            s = re.sub(r'\.to_string\(\)', '')  # to_string
            s = re.sub(r'\.enumerate\(\)', '')  # enumerate
            s = re.sub(r'\.strip\(\)', '')  # strip
            s = re.sub(r'isize\(\d+)', '')
            s = re.sub(r'as isize', '')
            s = re.sub(r' as i32', '')
            s = re.sub(r' as i64', '')
            s = re.sub(r' as usize', '')
            s = re.sub(r' as bool', '')
            s = re.sub(r' as char', '')
            s = re.sub(r' as &str', '')
            s = re.sub(r' as &mut ', '')
            s = re.sub(r' as f64', '')
            s = re.sub(r' as f32', '')

            if not s.endswith().strip():
                c_algo.append(stripped())

        return '\n'.join(c_algo)

    # Build C function signature
    c_params_str = convert_params(params) if params else ''
    if ret_type == 'void':
        c_sig = f'void {func_name}({c_params_str}) {{\n'
    elif ret_type == 'bool':
        c_sig = f'int {func_name}({c_params_str}) {{\n'
    elif ret_type == 'long long':
        c_sig = f'long long {func_name}({c_params_str}) {{\n'
    elif ret_type == 'double':
        c_sig = f'double {func_name}({c_params_str}) {{\n'
    elif ret_type == 'char':
        c_sig = f'char {func_name}({c_params_str}) {{\n'
    elif ret_type.strip().startswith('No *'):
        c_sig = f'{ret_type} {func_name}({c_params_str}) {{\n'
    else:
        c_sig = f'{ret_type} *{func_name}({c_params_str}) {{\n'

    # Read main function
    main_lines = []
    in_main = False
    for j in range(main_start, len(lines)):
        stripped = lines[j].strip()
        if stripped == '}' or stripped.startswith('}'):
            in_main = True
            main_start = j + 1
        if stripped.startswith('fn main()'):
            in_main = False
            break

    main_lines = lines[j+1:]

    return func_name, ret_type, params, algo_code, main_lines

# Process each missing exercise
for t, e in missing:
    rust_path = f'{rust_base}/t{t}e{e}/src/main.rs'
    if not os.path.exists(rust_path):
        print(f'ERROR: No Rust in {t}/{e}')
        continue
    with open(rust_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()

    func_name, ret_type, params, algo_code, main_lines = extract_rust_solution(rust_path)

    c_sig = build_c_sig(func_name, ret_type, params, algo_code, ret_type != 'void')
    c_main = extract_main(lines, main_lines, t, e)

    os.makedirs(f'{c_base}/{t}/{e}', exist=True)
    c_path = f'{c_base}/{t}/{e}/main.c'
    with open(c_path, 'w') as f:
        f.write(c_sig)
    print(f'Generated: {t}/{e}')

print(f'\nDone! Total generated: {len([m[0] for m in missing])}')
