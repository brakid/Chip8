import re

def is_label(line):
    return line.startswith(':')

def count_commands(label, classified_lines):
    counter = 0
    for (line, islabel) in classified_lines:
        if not islabel:
            counter = counter + 1
        elif islabel and get_label(line) == label:
            return counter
        
def get_label(value):
    return value[1:]

def get_command(value):
    return value.strip()

def replace_label(value, labels_to_addresses):
    if value in labels_to_addresses:
        return hex(labels_to_addresses[value])
    else:
        return value

def replace_labels(command, labels_to_addresses):
    splitted_command = command.split(' ')
    return ' '.join([replace_label(command_part, labels_to_addresses) for command_part in splitted_command])
    
def replace_labels_from_code(start_address, code):
    classified_lines = [(line, is_label(line)) for line in code]
    
    labels = [label for (label, islabel) in classified_lines if islabel]
    commands = [get_command(command) for (command, islabel) in classified_lines if not islabel]
    
    labels_to_addresses = {}
    
    for label in labels:
        label_value = get_label(label)
        counter = count_commands(label_value, classified_lines)
        labels_to_addresses[label_value] = start_address + 2 * counter

    return [replace_labels(command, labels_to_addresses) for command in commands]

def prepare_code(start_address, assembler_code):
    no_empty_lines_assembler_code = [line for line in assembler_code.split('\n') if len(line) > 0]
    no_comments_assembler_code = [re.sub(r';.*', '', line) for line in no_empty_lines_assembler_code]

    return replace_labels_from_code(start_address, no_comments_assembler_code)