import matplotlib.pyplot as plt
import numpy as np
import csv

h = [
    0.000000000000000000,
    0.000013345461113027,
    0.000074628325145084,
    0.000221840136667604,
    0.000503089050699102,
    0.000977933487489023,
    0.001716811061404203,
    0.002798105609275848,
    0.004302721212771339,
    0.006306412706846188,
    0.008870504732950237,
    0.012031960283269912,
    0.015793983968478750,
    0.020118426124570966,
    0.024921170591889922,
    0.030071442399851649,
    0.035395584801464423,
    0.040685371133555401,
    0.045710393533975331,
    0.050233572691654620,
    0.054028424473579854,
    0.056896454751228141,
    0.058682970302180941,
    0.059289706319876699,
    0.058682970302180948,
    0.056896454751228141,
    0.054028424473579854,
    0.050233572691654640,
    0.045710393533975338,
    0.040685371133555401,
    0.035395584801464423,
    0.030071442399851669,
    0.024921170591889922,
    0.020118426124570963,
    0.015793983968478739,
    0.012031960283269916,
    0.008870504732950243,
    0.006306412706846191,
    0.004302721212771345,
    0.002798105609275851,
    0.001716811061404202,
    0.000977933487489024,
    0.000503089050699102,
    0.000221840136667604,
    0.000074628325145085,
    0.000013345461113028,
    0.000000000000000000,
]


t = [] # column 0
amp = [] # column 1
name = "sigA.csv"

with open(name) as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        amp.append(float(row[1])) # second column

buffer_size = len(h)
buffer = []
moving_avg = [0 for _ in range(buffer_size-1)]

for i in range(len(amp)):
    buffer.append(amp[i])

    # when the buffer is of size buffer size, compute average (dot product of vectors) and pop one off
    if(len(buffer) == buffer_size):
        moving_avg.append(np.dot(h, buffer)) # don't care about order, filter is 2 sided
        buffer.pop(0)



fig, (ax1, ax2) = plt.subplots(2, 1)
fig.tight_layout()

ax1.set_title(f'Time vs Amplitude - Lowpass Blackman Filter Cutoff 200Hz, Bandwidth 1000Hz')
ax1.plot(t,amp,'k')
ax1.plot(t, moving_avg, 'r')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')

Fs = 10000 # sample rate
Ts = 1.0/Fs; # sampling interval
ts = np.arange(0,t[-1],Ts) # time vector
y = amp # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Y = np.fft.fft(y)/n # fft computing and normalization
Y = Y[range(int(n/2))]

y = moving_avg # the data to make the fft from
n = len(y) # length of the signal
k = np.arange(n)
T = n/Fs
frq = k/T # two sides frequency range
frq = frq[range(int(n/2))] # one side frequency range
Z = np.fft.fft(y)/n # fft computing and normalization
Z = Z[range(int(n/2))]


ax2.set_title('Freq (Hz) vs |Y(freq)|')
ax2.loglog(frq,abs(Y),'k') # plotting the fft
ax2.loglog(frq,abs(Z),'r') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')


plt.show()