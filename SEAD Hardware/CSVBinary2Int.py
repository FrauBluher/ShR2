#!/usr/bin/python3.3
#Converts 8bit split int character CSV file into human
#readable ints.  Just for testing...

import sys
from bitstring import BitArray

__author__ = "Pavlo Manovi"
__copyright__ ="Copyright (C) 2014 Pavlo Manovi"
__license__ = "The MIT License (MIT)"

def main():
    infile = input("Enter your input filename: ")
    outfile = input("Enter your output filename: ")
    f = open(infile, "rb")
    o = open(outfile, "w")

    f.seek(0)
    for line in f:
        if len(line) == 6:
            x = int(((line[0] << 8) & (0xFF00)) | (line[1] & (0x00FF)))
            y = int(((line[3] << 8) & (0xFF00)) | (line[4] & (0x00FF)))
            a = "{0:b}".format(x)
            b = "{0:b}".format(y)
            A0 = BitArray(bin=str(a))
            A1 = BitArray(bin=str(b))

            o.write(str(a) + ',' + str(b) + '\n')
        if len(line) != 6:
            o.write("0,0\n")

if __name__=='__main__':
    main()
