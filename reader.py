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

def make_pandas_frame(data1, data2):

	df_1 = pd.DataFrame({"data1" : data1}, dtype=np.float64)
	df_2 = pd.DataFrame({"data2" : data2}, dtype=np.float64)

	return pd.concat([df_1, df_2], axis=1)



print "-------------"
data1 = read_binary_file(args.file1)
data2 = read_binary_file(args.file2)


print "original data 1 has length", len(data1)
print "original data 2 has length", len(data2)

# data1 = data1[:1000]
# data2 = data2[:700]

print "data 1 has length", len(data1)
print "data 2 has length", len(data2)

df = make_pandas_frame(data1,data2)

df['diff'] = df["data1"]-df["data2"]

df["d1"] = df["data1"] - min(df["data1"])
df["d2"] = df["data2"] - min(df["data1"])
df["delta"] = df.d1 - df.d2

print df.head()

print df.tail()
print df.dtypes

print "is sorted:", pd.algos.is_monotonic_float64(df.data1.values, False)[0]
print "is sorted:", pd.algos.is_monotonic_float64(df.data2.values, False)[0]
print "is sorted:", pd.algos.is_monotonic_float64(df.delta.values, False)[0]

bin_width_f = args.bin_width

print args

scale = 1.0/bin_width_f;
g2size = args.g2_size
# result = Correlate_float(g2, list1, list2, g2size, list1_size, list2_size, scale);

result = Correlate_float(df["d1"], df["d2"].dropna(), g2size, len(df.d1), len(df["d2"].dropna()), scale);
result.to_csv("result.csv")

print result.describe()
print result.value_counts()

