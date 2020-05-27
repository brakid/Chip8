import re

NIBBLE = '(0x[0-9A-Fa-f])'
BYTE = '(0x[0-9A-Fa-f]{1,2})'
ADDRESS = '(0x[0-9A-Fa-f]{3})'
WORD = '(0x[0-9A-Fa-f]{1,4})'
REGISTER = 'V([0-9A-Fa-f])'

system_call_pattern = re.compile('SYS {}'.format(ADDRESS))
clear_screen_pattern = re.compile('CLS')
return_pattern = re.compile('RET')
jump_address_pattern = re.compile('JP {}'.format(ADDRESS))
call_pattern = re.compile('CALL {}'.format(ADDRESS))
skip_equal_register_byte_pattern = re.compile('SE {}, {}'.format(REGISTER, BYTE))
skip_not_equal_register_byte_pattern = re.compile('SNE {}, {}'.format(REGISTER, BYTE))
skip_equal_register_register_pattern = re.compile('SE {}, {}'.format(REGISTER, REGISTER))
load_register_byte_pattern = re.compile('LD {}, {}'.format(REGISTER, BYTE))
add_register_byte_pattern = re.compile('ADD {}, {}'.format(REGISTER, BYTE))
load_register_register_pattern = re.compile('LF {}, {}'.format(REGISTER, REGISTER))
or_register_register_pattern = re.compile('OR {}, {}'.format(REGISTER, REGISTER))
and_register_register_pattern = re.compile('AND {}, {}'.format(REGISTER, REGISTER))
xor_register_register_pattern = re.compile('XOR {}, {}'.format(REGISTER, REGISTER))
add_register_register_pattern = re.compile('ADD {}, {}'.format(REGISTER, REGISTER))
subtract_register_register_pattern = re.compile('SUB {}, {}'.format(REGISTER, REGISTER))
shift_right_register_pattern = re.compile('SHR {}(, {})+'.format(REGISTER, REGISTER))
subtract_reversed_register_register_pattern = re.compile('SUBN {}, {}'.format(REGISTER, REGISTER))
shift_left_register_pattern = re.compile('SHL {}(, {})+'.format(REGISTER, REGISTER))
skip_not_equal_register_register_pattern = re.compile('SNE {}, {}'.format(REGISTER, REGISTER))
load_i_register_address_pattern = re.compile('LD I, {}'.format(ADDRESS))
jump_register_address_pattern = re.compile('JP V0, {}'.format(ADDRESS))
random_register_byte_pattern = re.compile('RND {}, {}'.format(REGISTER, BYTE))
display_register_register_nibble = re.compile('DRW {}, {}, {}'.format(REGISTER, REGISTER, NIBBLE))
skip_key_pressed_register_pattern = re.compile('SKP {}'.format(REGISTER))
skip_key_not_pressed_register_pattern = re.compile('SKNP {}'.format(REGISTER))
load_register_delay_timer_pattern = re.compile('LD {}, DT'.format(REGISTER))
load_register_key_pattern = re.compile('LD {}, K'.format(REGISTER))
load_delay_timer_register_pattern = re.compile('LD DT, {}'.format(REGISTER))
load_sound_timer_register_pattern = re.compile('LD ST, {}'.format(REGISTER))
add_i_register_register_pattern = re.compile('ADD I, {}'.format(REGISTER))
load_digit_register_pattern = re.compile('LD F, {}'.format(REGISTER)) # The data should be stored in the interpreter area of Chip-8 memory (0x000 to 0x1FF)
load_bcd_register_pattern = re.compile('LD B, {}'.format(REGISTER))
load_i_register_register_pattern = re.compile('LD \[I\], {}'.format(REGISTER))
load_register_i_register_pattern = re.compile('LD {}, \[I\]'.format(REGISTER))

# additional commands to allow writing data:
byte_data_pattern = re.compile('.byte {}'.format(BYTE))
word_data_pattern = re.compile('.word {}'.format(WORD))

def extract_variables(regex, string):
    match = regex.match(string)
    if match:
        return (True, match.groups())
    else:
        return (False, None)
    
def parse_address(address):
    address = int(address, 16)
    low_byte = address & 0xFF
    high_byte = (address >> 8) & 0x0F
    return [high_byte, low_byte]

def parse_register(register):
    register = int(register, 16)
    return register & 0xF

def parse_byte(byte):
    byte = int(byte, 16)
    return byte & 0xFF

def parse_nibble(nibble):
    nibble = int(nibble, 16)
    return nibble & 0xF

def parse_word(word):
    word = int(word, 16)
    low_byte = word & 0xFF
    high_byte = (word >> 8) & 0xFF
    return [high_byte, low_byte]
    
def parse_command(command):
    (match, variables) = extract_variables(system_call_pattern, command)
    if match and len(variables) == 1:
        [high_nibble, low_byte] = parse_address(variables[0])
        return [0x00 | high_nibble, low_byte]
    
    (match, _) = extract_variables(clear_screen_pattern, command)
    if (match):
        return [0x00, 0xE0]
    
    (match, _) = extract_variables(return_pattern, command)
    if (match):
        return [0x00, 0xEE]
    
    (match, variables) = extract_variables(jump_address_pattern, command)
    if match and len(variables) == 1:
        [high_nibble, low_byte] = parse_address(variables[0])
        return [0x10 | high_nibble, low_byte]
    
    (match, variables) = extract_variables(call_pattern, command)
    if match and len(variables) == 1:
        [high_nibble, low_byte] = parse_address(variables[0])
        return [0x20 | high_nibble, low_byte]
    
    (match, variables) = extract_variables(skip_equal_register_byte_pattern, command)
    if match and len(variables) == 2:
        register = parse_register(variables[0])
        byte = parse_byte(variables[1])
        return [0x30 | register, byte]
    
    (match, variables) = extract_variables(skip_not_equal_register_byte_pattern, command)
    if match and len(variables) == 2:
        register = parse_register(variables[0])
        byte = parse_byte(variables[1])
        return [0x40 | register, byte]
    
    (match, variables) = extract_variables(skip_equal_register_register_pattern, command)
    if match and len(variables) == 2:
        register1 = parse_register(variables[0])
        register2 = parse_register(variables[1])
        return [0x50 | register1, register2 << 4 | 0x00]
    
    (match, variables) = extract_variables(load_register_byte_pattern, command)
    if match and len(variables) == 2:
        register = parse_register(variables[0])
        byte = parse_byte(variables[1])
        return [0x60 | register, byte]
    
    (match, variables) = extract_variables(add_register_byte_pattern, command)
    if match and len(variables) == 2:
        register = parse_register(variables[0])
        byte = parse_byte(variables[1])
        return [0x70 | register, byte]
    
    (match, variables) = extract_variables(load_register_register_pattern, command)
    if match and len(variables) == 2:
        register1 = parse_register(variables[0])
        register2 = parse_register(variables[1])
        return [0x80 | register1, register2 << 4 | 0x00]
    
    (match, variables) = extract_variables(or_register_register_pattern, command)
    if match and len(variables) == 2:
        register1 = parse_register(variables[0])
        register2 = parse_register(variables[1])
        return [0x80 | register1, register2 << 4 | 0x01]
    
    (match, variables) = extract_variables(and_register_register_pattern, command)
    if match and len(variables) == 2:
        register1 = parse_register(variables[0])
        register2 = parse_register(variables[1])
        return [0x80 | register1, register2 << 4 | 0x02]
    
    (match, variables) = extract_variables(xor_register_register_pattern, command)
    if match and len(variables) == 2:
        register1 = parse_register(variables[0])
        register2 = parse_register(variables[1])
        return [0x80 | register1, register2 << 4 | 0x03]
    
    (match, variables) = extract_variables(add_register_register_pattern, command)
    if match and len(variables) == 2:
        register1 = parse_register(variables[0])
        register2 = parse_register(variables[1])
        return [0x80 | register1, register2 << 4 | 0x04]
    
    (match, variables) = extract_variables(subtract_register_register_pattern, command)
    if match and len(variables) == 2:
        register1 = parse_register(variables[0])
        register2 = parse_register(variables[1])
        return [0x80 | register1, register2 << 4 | 0x05]
    
    (match, variables) = extract_variables(shift_right_register_pattern, command)
    if match and len(variables) >= 0:
        register = parse_register(variables[0])
        return [0x80 | register1, 0x06]
    
    (match, variables) = extract_variables(subtract_reversed_register_register_pattern, command)
    if match and len(variables) == 2:
        register1 = parse_register(variables[0])
        register2 = parse_register(variables[1])
        return [0x80 | register1, register2 << 4 | 0x07]
    
    (match, variables) = extract_variables(shift_left_register_pattern, command)
    if match and len(variables) >= 0:
        register = parse_register(variables[0])
        return [0x80 | register1, 0x0E]
    
    (match, variables) = extract_variables(skip_not_equal_register_register_pattern, command)
    if match and len(variables) == 2:
        register1 = parse_register(variables[0])
        register2 = parse_register(variables[1])
        return [0x90 | register1, register2 << 4 | 0x00]
    
    (match, variables) = extract_variables(load_i_register_address_pattern, command)
    if match and len(variables) == 1:
        [high_nibble, low_byte] = parse_address(variables[0])
        return [0xA0 | high_nibble, low_byte]
    
    (match, variables) = extract_variables(jump_register_address_pattern, command)
    if match and len(variables) == 1:
        [high_nibble, low_byte] = parse_address(variables[1])
        return [0xB0 | high_nibble, low_byte]
    
    (match, variables) = extract_variables(random_register_byte_pattern, command)
    if match and len(variables) == 2:
        register = parse_register(variables[0])
        byte = parse_byte(variables[1])
        return [0xC0 | register, byte]
    
    (match, variables) = extract_variables(display_register_register_nibble, command)
    if match and len(variables) == 3:
        register1 = parse_register(variables[0])
        register2 = parse_register(variables[1])
        nibble = parse_nibble(variables[2])
        return [0xD0 | register1, register2 << 4 | nibble]
    
    (match, variables) = extract_variables(skip_key_pressed_register_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xE0 | register, 0x9E]
    
    (match, variables) = extract_variables(skip_key_not_pressed_register_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xE0 | register, 0xA1]
    
    (match, variables) = extract_variables(load_register_delay_timer_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xF0 | register, 0x07]
    
    (match, variables) = extract_variables(load_register_key_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xF0 | register, 0x0A]
    
    (match, variables) = extract_variables(load_delay_timer_register_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xF0 | register, 0x15]
    
    (match, variables) = extract_variables(load_sound_timer_register_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xF0 | register, 0x18]
    
    (match, variables) = extract_variables(add_i_register_register_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xF0 | register, 0x1E]
    
    (match, variables) = extract_variables(load_digit_register_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xF0 | register, 0x29]
    
    (match, variables) = extract_variables(load_bcd_register_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xF0 | register, 0x33]
    
    (match, variables) = extract_variables(load_i_register_register_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xF0 | register, 0x55]
    
    (match, variables) = extract_variables(load_register_i_register_pattern, command)
    if match and len(variables) == 1:
        register = parse_register(variables[0])
        return [0xF0 | register, 0x65]
    
    # handling byte and word data
    (match, variables) = extract_variables(byte_data_pattern, command)
    if match and len(variables) == 1:
        byte = parse_byte(variables[0])
        return [byte]

    (match, variables) = extract_variables(word_data_pattern, command)
    if match and len(variables) == 1:
        [high_byte, low_byte] = parse_word(variables[0])
        return [high_byte, low_byte]
    
    return ['Failure', command]

def parse_cleaned_code(cleaned_code):
    parsed = [part for command in cleaned_code for part in parse_command(command)]
    
    if 'Failure' in parsed:
        print('Failing while parsing the code:', parsed)
        return False
    
    return parsed