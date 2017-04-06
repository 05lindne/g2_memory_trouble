import argparse
import pandas as pd
import numpy as np


parser = argparse.ArgumentParser()
parser.add_argument("--file1", help="input file 1")
parser.add_argument("--file2", help="input file 2")
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

print "data 1 has length", len(data1)
print "data 2 has length", len(data2)

df = make_pandas_frame(data1,data2)

df['diff'] = df["data1"]-df["data2"]

print df.head()
print df.tail()
print df.dtypes
