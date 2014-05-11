#!/usr/bin/python3.3
#Converts 8bit split int character CSV file into human
#readable ints.  Just for testing...

import argparse, sys

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
        if len(line) == 9:
            A0 = int(((line[0] << 8) & (0xFF00)) | (line[1] & (0x00FF)))
            A1 = int(((line[3] << 8) & (0xFF00)) | (line[4] & (0x00FF)))
            B0 = int(((line[6] << 8) & (0xFF00)) | (line[7] & (0x00FF)))
            o.write(str(A0) + ',' + str(A1) + ',' + str(B0) + '\n')

if __name__=='__main__':
    main()
