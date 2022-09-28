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
volts = np.array(list(lut.values()))

table = np.array([volts, temps]).T
table = table[table[:, 0].argsort()]

import pandas as pd

df = pd.DataFrame(table, columns=["Voltage (mV)", "Temperature (deg C)"])
df = df.sort_values(by="Temperature (deg C)")

df.to_csv("seebeck_table.csv", index=False)

# Calculate Size

int_sz = 4
float_sz = 4

print(f"Size of table: {len(df) * (int_sz + float_sz)} bytes")

# generate C code

adc_fsr_v = 2.4
pga = 1
actual_fsr = adc_fsr_v / pga

adc_res = 2**16
volts_per_lsb = actual_fsr / adc_res

volts_lsb = volts / volts_per_lsb

with open("k_type_lut.h", "w") as f:
    f.write("static const int16_t k_type_lut[] = {")
    for i in range(len(volts_lsb)):
        f.write(f"{int(volts_lsb[i])}, ")
    f.write("};")
