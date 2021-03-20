import pathlib
import csv
from tqdm import tqdm
import soundfile
import subprocess
import numpy as np


current_path = pathlib.Path(__file__).parent
sim_path = current_path.parent.joinpath('simulations')
meta_path = sim_path.joinpath('meta.csv')
exec_path = current_path.parent.joinpath('Debug', 'room-raider')
tmp_path = current_path.parent.joinpath('tmp', 'tmp.wav')

expected_size = 192000
err_avg = np.zeros(expected_size)
err_var = np.zeros(expected_size)
err_max = -np.inf * np.ones(expected_size)
err_min = np.inf * np.ones(expected_size)
counter = 1

rms_err_l = []
rms_err_r = []

with open(meta_path, mode='r') as csv_file:
    csv_reader = csv.reader(csv_file, delimiter=',')

    for r in tqdm(csv_reader):

        ref_file_path = sim_path.joinpath(r[0] + '_r.wav')
        sys_file_path = sim_path.joinpath(r[0] + '_s.wav')

        target_l_path = pathlib.Path(r[1])
        target_r_path = pathlib.Path(r[2])

        h_l, fs_l = soundfile.read(target_l_path)
        h_r, fs_r = soundfile.read(target_r_path)

        assert fs_l == fs_r
        assert h_l.shape == (h_l.size,)
        assert h_r.shape == (h_r.size,)
        assert h_l.size == h_r.size
        assert h_l.size == expected_size

        h_l = h_l / np.max(np.abs(h_l))
        h_r = h_r / np.max(np.abs(h_r))

        success = subprocess.call(
            [
                exec_path.absolute().as_posix(),
                'mtest',
                '--verbose',
                '--nofork',
                '--debug', 'room_raider.main',
                '--args',
                '-d',
                '-sr', str(fs_l),
                '-i', sys_file_path.absolute().as_posix(),
                '-r', ref_file_path.absolute().as_posix(),
                '-o', tmp_path.absolute().as_posix()
            ],
            stdout=subprocess.DEVNULL
        )

        assert success == 0

        g, fs = soundfile.read(tmp_path, stop=h_l.size)

        assert fs == fs_l
        assert g.shape == (h_l.size, 2)

        err_l = np.array(g[:, 0] - h_l)
        temp_avg = err_avg
        err_avg += (err_l - err_avg) / counter
        err_var += (err_l - err_avg) * (err_l - temp_avg)
        counter += 1

        err_r = np.array(g[:, 1] - h_r)
        temp_avg = err_avg
        err_avg += (err_r - err_avg) / counter
        err_var += (err_r - err_avg) * (err_r - temp_avg)
        counter += 1

        err_max = np.maximum(err_max, err_l)
        err_max = np.maximum(err_max, err_r)

        err_min = np.minimum(err_min, err_l)
        err_min = np.minimum(err_min, err_r)

        rms_err_l.append(np.sqrt(np.mean(err_l[-1]**2)))
        rms_err_r.append(np.sqrt(np.mean(err_r[-1] ** 2)))

err_var /= (counter - 1)

np.savez(
    tmp_path.parent.joinpath('study_performance'),
    err_avg=err_avg,
    err_var=err_var,
    err_max=err_max,
    err_min=err_min,
    rms_err_l=np.array(rms_err_l),
    rms_err_r=np.array(rms_err_r)
)
