import pathlib
import audiofile
from scipy import signal
import numpy as np
import matplotlib.pyplot as plt

tmp_path = pathlib.Path(__file__).parent.parent.joinpath('tmp')
sweep_file = tmp_path.joinpath('test.wav')

win_length = 0.01
overlap = 0.75
df = 1

x, fs = audiofile.read(sweep_file)

f, t, X = signal.stft(
    x=x,
    fs=fs,
    window='hann',
    nperseg=int(win_length * fs),
    noverlap=int(win_length * fs * overlap),
    nfft=int(fs / df)
)

X_level_source = np.copy(X)
X_level_source[X_level_source == 0] = 10**(-200/20)

plt.imshow(
    20 * np.log10(np.abs(X_level_source)),
    aspect='auto',
    origin='lower',
    extent=[np.min(t), np.max(t), np.min(f), np.max(f)],
    vmin=-200,
    vmax=0
)
plt.xlabel('Time [s]')
plt.ylabel('Frequency [Hz]')
plt.colorbar()
plt.show()
