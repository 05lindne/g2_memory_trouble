import argparse
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from bisect import bisect_left
from collections import Counter


data1 = np.array([2,10,11,12,13,15,20], dtype=np.float64)
data2 = np.array([3,4,4,5,8,12], dtype=np.float64)

value = 5
scale = 1
offset = 15

counter = Counter()

for item in data1:

	# print "data1:", item, data1
	# print "data2:", data2
	
	insertion_index = bisect_left(data2, item - float(offset)/scale)


	if insertion_index < len(data2):

		# print "at index", insertion_index, "we have ",item, "<=", data2[insertion_index]
		# print "at index", insertion_index, "we have ",item
		# print item, " to position ", insertion_index

		# consider exclusively the data from the insertion index onward
		# data with index j < insertio_index the inequality (data2[j] - data1[item])*scale + offset >= 0 is violated
		data2 =  data2[insertion_index:]
		# print data2
		# print "pruned data:", data2

		delta = (data2 - item)*scale + offset

		print "deltas: ", delta

		counter.update(delta)

		# print counter

	else:
		break

# print list(counter.elements())
# print counter
