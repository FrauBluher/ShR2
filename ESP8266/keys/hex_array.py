#!/usr/bin/env python2
# ============================================================
# File: hex-array.py [input files]
# Description: inputs a file, and formats it in a hex array
# Created by Henry Crute
# 3/5/2015
# ============================================================

#12 hex digits on a line

import sys



def read_file(filename):
	i = 0
	with open(filename) as f:
		while True:
			c = f.read(1)
			if not c:
				print "\nEnd of file ", filename, " ", i, " lines"
				break
			elif c != ' ' and c != '\n':
				i += 1
				if ((i % 12) == 0):
					sys.stdout.write("0x%x, \n" % ord(c))
				else:
					sys.stdout.write("0x%x, " % ord(c))
				

def main():
	i = 0
	for arg in sys.argv:
		if (i == 0):
			i += 1
			continue
		read_file(arg)
		i += 1
	return

if __name__ == "__main__":
	main()
