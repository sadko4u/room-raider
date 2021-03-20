import pathlib
from tqdm import tqdm
import soundfile
import numpy as np
import matplotlib.pyplot as plt


current_path = pathlib.Path(__file__).parent
ir_root_path = current_path.parent.joinpath('impulse_responses')

sample_rates = []
ir_durations = []
ir_max_values = []
ir_min_values = []

for p in tqdm(ir_root_path.rglob('*.wav')):
    h, fs = soundfile.read(p)

    assert h.shape == (h.size, )

    sample_rates.append(fs)
    ir_durations.append(h.size / fs)
    ir_max_values.append(np.max(h))
    ir_min_values.append(np.min(h))

plt.hist(ir_max_values)
plt.hist(ir_min_values)
plt.title('IR Amplitude Stats')
plt.xlabel('Peak IR Value')
plt.ylabel('Counts')
plt.grid(True)
plt.legend(['Positive', 'Negative'])
plt.show()

plt.hist(ir_durations)
plt.title('IR Duration Stats')
plt.xlabel('IR Duration')
plt.ylabel('Counts')
plt.grid(True)
plt.show()

plt.hist(sample_rates)
plt.title('Sample Rates')
plt.xlabel('Sample Rates')
plt.ylabel('Counts')
plt.grid(True)
plt.show()

print('Total IR: ' + str(len(ir_durations)))
print('Max IR Duration: ' + str(np.max(ir_durations)) + ' s')
print('Max Sample Rate: ' + str(np.max(sample_rates)) + ' Hz')
