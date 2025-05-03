import matplotlib.pyplot as plt
import numpy as np
import csv

h = [
    0.000000000000000000,
    0.000403024633051974,
    0.002819679017019636,
    0.009882797436660439,
    0.024747538710707180,
    0.049221882231288448,
    0.081481385627892991,
    0.115145384747837726,
    0.140962518173749385,
    0.150671578843584397,
    0.140962518173749385,
    0.115145384747837781,
    0.081481385627893047,
    0.049221882231288448,
    0.024747538710707197,
    0.009882797436660460,
    0.002819679017019640,
    0.000403024633051974,
    0.000000000000000000,
]

t = [] # column 0
amp = [] # column 1
name = "sigD.csv"

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

ax1.set_title(f'Time vs Amplitude - Lowpass Blackman Filter Cutoff 500Hz, Bandwidth 2000Hz')
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