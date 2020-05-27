#!/usr/bin/env python3

import re
import sys
from cleanup import prepare_code
from parser import parse_cleaned_code

START_ADDRESS = 0x200

if len(sys.argv) < 2 or len(sys.argv) > 3 :
    print('Expecting assembler file to read from, and optional target file name')
    exit(1)

source_file_name = sys.argv[1]
target_file_name = sys.argv[2] if len(sys.argv) == 3 else 'a.out'

assembler_code = ''
with open(source_file_name, 'r') as file:
    assembler_code = file.read()

print('Original (from file ' + source_file_name + '):\n', assembler_code)

cleaned_code = prepare_code(START_ADDRESS, assembler_code)

print('Cleaned:\n', cleaned_code)

parsed = parse_cleaned_code(cleaned_code)

if not parsed:
    exit(1)

print('Parsed:\n', parsed)
print('Parsed hex:\n', '{ ' + ', '.join([hex(p) for p in parsed]) + ' }')

with open(target_file_name, 'wb') as file:
    file.write(bytearray(parsed))

print('Output written to ' + target_file_name)

exit(0)