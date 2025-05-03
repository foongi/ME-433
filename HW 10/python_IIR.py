import matplotlib.pyplot as plt
import numpy as np
import csv


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




new_val_weight = 0.025
old_val_weight = 1 - new_val_weight
average = amp[0] # set average to start with 1st value

moving_avg = []

for i in range(len(amp)):

    average = new_val_weight * amp[i] + old_val_weight * average 
    moving_avg.append(average)


fig, (ax1, ax2) = plt.subplots(2, 1)
fig.tight_layout()

ax1.set_title(f'Time vs Amplitude - Weights: A = {old_val_weight}, B = {new_val_weight}')
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

    