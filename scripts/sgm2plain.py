#!/usr/bin/python3
#
# sgm2plain.py
# converting sgm format to plain format
#
# author: Playinf
# email: playinf@stu.xmu.edu.cn
#

import os
import sys

def print_version():
    print('sgm2plain: converting sgm format to plain format')
    print('\tversion: 1.0')
    print('\tchange date: 09/06/2013')

def print_help():
    print('sgm2plain input [output] | -h | -v | --help | --version ')
    print('\t-h | --help\t\tshow help message')
    print('\t-v | --version\t\tshow version information')

def convert_file(in_name, out_name):
    in_file = open(in_name)
    out_file = open(out_name, 'w')

    for line in in_file:
        pos1 = line.find('>')
        pos2 = line.rfind('<')

        if pos2 <= pos1 or pos1 == -1 or pos2 == -1:
            continue

        pos1 = pos1 + 1

        out_line = line[pos1:pos2]
        out_line = out_line.strip()
        out_file.write(out_line + '\n')

    print('complete!\n')
    in_file.close()
    out_file.close()

if __name__ == '__main__':
    arg_len = len(sys.argv)
    in_file_name = ''
    out_file_name = ''

    # check for arguments
    if arg_len == 1:
        print_help()
        exit()
    elif arg_len == 2:
        in_arg = sys.argv[1]
        if in_arg == '-h' or in_arg == '--help':
            print_help()
            exit()
        elif in_arg == '-v' or in_arg == '--version':
            print_version()
            exit()
        else:
            in_file_name = sys.argv[1]
            out_file_name = in_file_name + '.plain'
    elif arg_len == 3:
        in_file_name = sys.argv[1]
        out_file_name = sys.argv[2]
    else:
        print_help()
        exit()

    convert_file(in_file_name, out_file_name)
