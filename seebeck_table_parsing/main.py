def lineparse(line, neg, lut):
    vals = line.split()
    base_temp = int(vals[0])
    vals = vals[1:]
    vals = list(map(float, vals))

    temps = [base_temp + (i * (-1 if neg else 1)) for i in range(0, 11)]

    for i in range(len(vals)):
        temp = temps[i]
        val = vals[i]
        lut[temp] = val


lut = {}
with open("negative_parts.txt", "r") as f:
    for line in f:
        lineparse(line, True, lut)

with open("positive_parts.txt", "r") as f:
    for line in f:
        lineparse(line, False, lut)

# from matplotlib import pyplot as plt

# first_n = 350

# plt.plot(list(lut.keys())[:first_n], list(lut.values())[:first_n])
# plt.show()

import numpy as np

temps = np.array(list(lut.keys()))
vals = np.array(list(lut.values()))

table = np.array([temps, vals]).T

import pandas as pd

df = pd.DataFrame(table, columns=["Temperature", "Seebeck Coefficient"])

df.to_csv("seebeck_table.csv", index=False)
