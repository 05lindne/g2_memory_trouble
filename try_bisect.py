import argparse
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from bisect import bisect_left


df1 = pd.Series([2,10,11,12,14,20,21,22], dtype=np.float64)
df2 = pd.Series([3,4,5,8,12,14,16,23,24], dtype=np.float64)

value = 5

print "location of inserting ", value, " in dataframe df1: ", bisect_left(df1, 5)

for item in df2:

	print item 
	print "inserted at: ", bisect_left(df1, item)