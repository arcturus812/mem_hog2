#!/usr/bin/python
import sys
import random
import re

def convert_to_bytes(size_str):
    match = re.match(r'^(\d+)\s*(B|KB|MB|GB)?$', size_str, re.IGNORECASE)
    if not match:
        raise ValueError('Invalid size: {}'.format(size_str))
    size, unit = match.groups()
    size = int(size)
    if unit is None or unit.lower() == 'b':
        return size
    elif unit.lower() == 'kb':
        return size * 1024
    elif unit.lower() == 'mb':
        return size * 1024 * 1024
    elif unit.lower() == 'gb':
        return size * 1024 * 1024 * 1024
    else:
        raise ValueError('Invalid unit: {}'.format(unit))

def generate_random_numbers(max_idx, count):
    if not isinstance(max_idx, int) or not isinstance(count, int):
        print("Error: Arguments must be integers")
        return

    if max_idx <= 0 or count <= 0:
        print("Error: Arguments must be positive integers")
        return


    f_name = "randidx_" + str(sys.argv[1]) + "_" + str(sys.argv[2]) + ".dat"
    with open(f_name, "w") as f:
        for i in range(count):
            random_number = random.randint(0, max_idx-1)
            f.write(str(random_number) + "\n")

def main():
    if len(sys.argv) < 3:
        print("usage: ./gen_rand.py <index_max;10b 10kb, 10mb, 10gb> <#_to_generate>")
        print("example, if size of memory that should be randomly access is 100MB")
        print("\tand number of random number is 10000")
        print("\t./gen_rand.py 100mb 10000");
        return
    byte_size = convert_to_bytes(sys.argv[1])
    max_idx = byte_size/4 # it should be divided int size(4 byte)
    print("create rand_idx.dat for random memory size " + sys.argv[1])
    print("data size(number of idx) is " + str(sys.argv[2]))
    generate_random_numbers(max_idx, int(sys.argv[2]))
    print("done")

if __name__ == '__main__':
    main()
