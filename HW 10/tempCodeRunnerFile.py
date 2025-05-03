import matplotlib.pyplot as plt
import numpy as np
import csv

h = [
    0.000000000000000000,
    0.000000000000000000,
    0.000933133390871660,
    0.003072659611081322,
    0.002247335654119409,
    -0.008485830499059089,
    -0.026385960548946374,
    -0.026914878709614190,
    0.024137420946633840,
    0.132295007784289986,
    0.249075152599523175,
    0.300051919542200618,
    0.249075152599523175,
    0.132295007784289986,
    0.024137420946633844,
    -0.026914878709614211,
    -0.026385960548946378,
    -0.008485830499059094,
    0.002247335654119411,
    0.003072659611081328,
    0.000933133390871660,
    0.000000000000000000,
    0.000000000000000000,
]
t = [] # column 0
amp = [] # column 1
name = "sigC.csv"

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

ax1.set_title(f'Time vs Amplitude - Lowpass Blackman Filter Cutoff 1500Hz, Bandwidth 2000Hz')
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