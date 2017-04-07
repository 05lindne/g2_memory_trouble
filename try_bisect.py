import argparse
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from bisect import bisect_left


df1 = pd.Series([5,10,11,12,14,20,21,22,26], dtype=np.float64)
df2 = pd.Series([3,4,6,8,12,14,16,23,24,25], dtype=np.float64)

value = 5

print "location of inserting ", value, " in dataframe df2: ", bisect_left(df2, 5)


for item in df1:

	print "old df2: "
	print df2

	insertion_point = bisect_left(df2, item)

	print "value ", item, " inserted at: ", insertion_point

	# cut away data lower than insertion point
	df2 =  df2[insertion_point:]
	df2 = df2.reset_index(drop=True)
	bin = df2 - item

	print "bins: "
	print bin
	print "new df2: "
	print df2