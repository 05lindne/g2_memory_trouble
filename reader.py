import argparse
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from bisect import bisect_left

parser = argparse.ArgumentParser()
parser.add_argument("--file1", help="input file 1")
parser.add_argument("--file2", help="input file 2")
parser.add_argument("--bin_width", type=int, default=1, help="input file 2")
parser.add_argument("--g2_size", type=int, default=20000, help="input file 2")
args = parser.parse_args()


def read_binary_file(path):

	f = open(path, mode ="r")
	data = np.fromfile(f, dtype=np.int64)
	f.close()

	return data

def make_pandas_frame(data1, data2):

	df_1 = pd.DataFrame({"data1" : data1}, dtype=np.float64)
	df_2 = pd.DataFrame({"data2" : data2}, dtype=np.float64)

	return pd.concat([df_1, df_2], axis=1)

def correlate(eventlist1, eventlist2, g2size, len_eventlist1, len_eventlist2, scale):


	print "hi"

	return

print "-------------"
data1 = read_binary_file(args.file1)
data2 = read_binary_file(args.file2)


print "original data 1 has length", len(data1)
print "original data 2 has length", len(data2)

print "data 1 has length", len(data1)
print "data 2 has length", len(data2)


# df = make_pandas_frame(data1,data2)

# print df.head()
# print df.tail()
# print df.dtypes


df1 = pd.DataFrame({"data1" : data1}, dtype=np.float64)
df2 = pd.DataFrame({"data2" : data2}, dtype=np.float64)

scale = 1.0/args.bin_width

correlate(df1, df2, args.g2_size, len(df1), len(df2), scale)

