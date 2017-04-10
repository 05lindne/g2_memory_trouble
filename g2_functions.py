import numpy as np
import pandas as pd


def Correlate_float(cl1, cl2, sizeg2, n1, n2, scale):
	# Calculate g2 function out of the list of clicks from detectors 1 and 2

	bin_i=0
	i2s = 0
	TwoTimesSizeg2 = 2*sizeg2
	offset = sizeg2
	
	g2 = np.zeros(TwoTimesSizeg2+1, dtype=np.int64)

	# print n1, n2

	for i1 in xrange(n1-1):
		for i2 in xrange(i2s,n2-1):

			# print i1, i2

			bin_f = (cl2[i2]-cl1[i1])*scale + offset

			# print cl1[i1], cl2[i2], bin_f, i2s, i1, n1, i2, n2
			
			if bin_f < 0.0:
				i2s=i2    # mark at which position we stopped seeing garbage

			else:
			
				bin_i = int(bin_f)
				if bin_i<=TwoTimesSizeg2:
				
					g2[bin_i]+=1

				else:
					break
	
	return pd.Series(g2)

def Correlate_float(cl1, cl2, sizeg2, n1, n2, scale):
	# Calculate g2 function out of the list of clicks from detectors 1 and 2

	bin_i=0
	i2s = 0
	TwoTimesSizeg2 = 2*sizeg2
	offset = sizeg2
	
	g2 = np.zeros(TwoTimesSizeg2+1, dtype=np.int64)

	# print n1, n2

	for i1 in xrange(n1-1):
		
		a = cl1[i1]


		for i2 in xrange(i2s,n2-1):

			# print i1, i2

			bin_f = (cl2[i2]-cl1[i1])*scale + offset

			# print cl1[i1], cl2[i2], bin_f, i2s, i1, n1, i2, n2
			
			if bin_f < 0.0:
				i2s=i2    # mark at which position we stopped seeing garbage

			else:
			
				bin_i = int(bin_f)
				if bin_i<=TwoTimesSizeg2:
				
					g2[bin_i]+=1

				else:
					break
	
	return pd.Series(g2)

def g2_differences(data1, data2, scale=1, offset=0):

	from bisect import bisect_left
	from collections import Counter
	from itertools import izip

	try:

		it = iter(data1)
		it.next()
		all(b >= a for a, b in izip(data1, it))

	except Exception as e:

		print "Data1 is not in sorted order."

	try:

		it = iter(data2)
		it.next()
		all(b >= a for a, b in izip(data1, it))

	except Exception as e:

		print "Data2 is not in sorted order."

	counter = Counter()

	for item in data1:

		# print "data1:", item, data1
		# print "data2:", data2
		
		insertion_index = bisect_left(data2, item - float(offset)/scale)

		if insertion_index < len(data2):

			# print "at index", insertion_index, "we have ",item, "<=", data2[insertion_index]

			# consider exclusively the data from the insertion index onward
			# data with index j < insertio_index the inequality (data2[j] - data1[item])*scale + offset >= 0 is violated
			data2 =  data2[insertion_index:]
			
			# print "pruned data:", data2

			# compute the difference for all remaining entries in data2
			delta = data2 - item

			# print "deltas: ", delta

			# add the result to the counter
			counter.update(delta)

		else:
			break

	return counter.elements()