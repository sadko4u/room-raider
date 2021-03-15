import pathlib
import audiofile
from tqdm import tqdm
import numpy as np
import csv


max_latency_s = 1

current_path = pathlib.Path(__file__).parent
tmp_path = current_path.parent.joinpath('tmp')
test_sig_path = tmp_path.joinpath('test.wav')
ir_root_path = current_path.parent.joinpath('impulse_responses')
sim_path = current_path.parent.joinpath('simulations')

x, fs = audiofile.read(test_sig_path)

ir_paths = [p for p in ir_root_path.rglob('*.wav')]
ir_idx = [n for n in range(len(ir_paths)) if n % 2 == 0]

meta_file_name = 'meta.csv'
meta_path = sim_path.joinpath(meta_file_name)
with open(meta_path, 'w') as csv_file:
    csv_writer = csv.writer(
        csv_file,
        delimiter=',',
        quotechar='|',
        quoting=csv.QUOTE_MINIMAL
    )

    for i in tqdm(ir_idx):
        left_ir_path = ir_paths[i].absolute().as_posix()
        h_l, h_fs = audiofile.read(left_ir_path)
        assert h_fs == fs
        assert h_l.shape == (h_l.size, )

        right_ir_path = ir_paths[i + 1].absolute().as_posix()
        h_r, h_fs = audiofile.read(right_ir_path)
        assert h_fs == fs
        assert h_r.shape == (h_r.size,)

        latency = max_latency_s * np.random.rand() * fs

        c_size = x.size + max(h_l.size, h_r.size) - 1

        shifter = np.exp(-1j * 2 * np.pi * latency * np.arange(c_size // 2 + 1) / c_size)
        X = np.fft.rfft(x, n=c_size)
        y = np.zeros((2, c_size))

        y[0, :] = np.fft.irfft(X * np.fft.rfft(h_l, n=c_size) * shifter, n=c_size)
        y[1, :] = np.fft.irfft(X * np.fft.rfft(h_r, n=c_size) * shifter, n=c_size)

        filename = str(i) + '.wav'

        audiofile.write(sim_path.joinpath(filename).absolute().as_posix(), y, fs)
        csv_writer.writerow([sim_path.joinpath(filename).absolute().as_posix(), left_ir_path, right_ir_path])
