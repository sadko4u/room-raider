import pathlib
import numpy as np
import matplotlib.pyplot as plt


current_path = pathlib.Path(__file__).parent
tmp_path = current_path.parent.joinpath('tmp')

data = np.load(tmp_path.joinpath('study_performance.npz'))

all_rms_err = np.concatenate([data['rms_err_l'], data['rms_err_r']])

counts, bins = np.histogram(all_rms_err, bins=100, density=True)
centers = 0.5 * np.diff(bins) + bins[:-1]

plt.plot(centers, counts)
plt.grid(True)
plt.xlabel('RMS Error')
plt.ylabel('Probability Density Function')
plt.show()

time = np.arange(192000) / 96000
plt.plot(time, data['err_avg'], label='Mean')
std = plt.plot(time, data['err_avg'] + np.sqrt(data['err_var']), label='Mean +/- Standard Deviation')
plt.plot(time, data['err_avg'] - np.sqrt(data['err_var']), color=std[0].get_color())
plt.grid(True)
plt.legend()
plt.xlabel('Time [s]')
plt.ylabel('Error Statistics')
plt.show()

print(all_rms_err.size)
