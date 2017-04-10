import argparse
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt


parser = argparse.ArgumentParser()
parser.add_argument("--file", help="g2 results file", default="result_1.csv")
args = parser.parse_args()


s = pd.Series.from_csv(args.file)

print s.describe()
s.plot(kind="line", ls = "-", alpha=0.5)
plt.show()
