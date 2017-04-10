import argparse
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

from g2_functions import *

parser = argparse.ArgumentParser()
parser.add_argument("--file1", help="input file 1", default="scan_xy_25_25_lr_x26y18_g2_Click_List1.txt")
parser.add_argument("--file2", help="input file 2", default="scan_xy_25_25_lr_x26y18_g2_Click_List2.txt")
parser.add_argument("--bin_width", help="bin width", type=int, default=1)
parser.add_argument("--g2_size", help="G2 size", type=int, default=20000)
args = parser.parse_args()


def read_binary_file(path):

	f = open(path, mode ="r")
	data = np.fromfile(f, dtype=np.int64)
	f.close()

	return data


print "-------------"
data1 = read_binary_file(args.file1)
data2 = read_binary_file(args.file2)


print "original data 1 has length", len(data1)
print "original data 2 has length", len(data2)

# data1 = data1[:1000]
# data2 = data2[:700]

# print "data 1 has length", len(data1)
# print "data 2 has length", len(data2)


# print pd.algos.is_monotonic_int64(data1, False)[0]
# print pd.algos.is_monotonic_int64(data2, False)[0]

deltas = g2_differences(data1,data2) 
deltas = list(deltas)

