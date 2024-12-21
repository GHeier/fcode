import ast
### Variables ###

#[CONTROL]
category = 'test'
calculation = 'test'
outdir = './output'
prefix = 'sample'
verbosity = 'high'
datfile_in = 'input.dat'
datfile_out = 'output.dat'

#[SYSTEM]
interaction = 'none'
dimension = 3
ibrav = 0
nbnd = 1
fermi_energy = 0.0
Temperature = 0.0
onsite_U = 0.0

#[MESH]
k_mesh = [10, 10, 10]
q_mesh = [10, 10, 10]
w_pts = 100

#[CELL]
cell = [[1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]

#[BRILLOUIN_ZONE]
brillouin_zone = [[6.283185307179586, 0.0, 0.0], [0.0, 6.283185307179586, 0.0], [0.0, 0.0, 6.283185307179586]]

#[BANDS]
band = []
band.append('fermi_gas')
eff_mass = []
eff_mass.append(1.0)
t0 = []
t0.append(1.0)
t1 = []
t1.append(0.0)
t2 = []
t2.append(0.0)
t3 = []
t3.append(0.0)
t4 = []
t4.append(0.0)
t5 = []
t5.append(0.0)
t6 = []
t6.append(0.0)
t7 = []
t7.append(0.0)
t8 = []
t8.append(0.0)
t9 = []
t9.append(0.0)
t10 = []
t10.append(0.0)

#[SUPERCONDUCTOR]
method = 'none'
FS_only = True
bcs_cutoff_frequency = 0.05
num_eigenvalues_to_save = 1
frequency_pts = 5

#[RESPONSE]
dynamic = False
### End Variables ###

### Functions ###

def BZ_from_cell(cell):
    import numpy as np
    a1 = np.array(cell[0])
    a2 = np.array(cell[1])
    a3 = np.array(cell[2])
    b1 = 2*np.pi*np.cross(a2, a3) / np.dot(a1, np.cross(a2, a3))
    b2 = 2*np.pi*np.cross(a3, a1) / np.dot(a2, np.cross(a3, a1))
    b3 = 2*np.pi*np.cross(a1, a2) / np.dot(a3, np.cross(a1, a2))
    return [b1, b2, b3]

def load_config():
    import sys
    import os

    current_file = os.path.abspath(__file__)
    input_file = current_file[:-27] + "build/bin/input.cfg"
    key, value, section = "", "", ""
    got_dimension = False
    index = 0
    with open(input_file, 'r') as f:
        for line in f:
            if "=" not in line and "[" in line and "]" in line:
                section = line.strip()[1:-1]
                index = 0
                continue
            if "=" in line:
                key, value = line.split('=', 1)
            key = key.strip()
            value = value.strip()
            # Set the variable

#[CONTROL]
            if "category" in key:
                global category
                category = value
            if "calculation" in key:
                global calculation
                calculation = value
            if "outdir" in key:
                global outdir
                outdir = value
            if "prefix" in key:
                global prefix
                prefix = value
            if "verbosity" in key:
                global verbosity
                verbosity = value
            if "datfile_in" in key:
                global datfile_in
                datfile_in = value
            if "datfile_out" in key:
                global datfile_out
                datfile_out = value

#[SYSTEM]
            if "interaction" in key:
                global interaction
                interaction = value
            if "dimension" in key:
                global dimension
                dimension = int(value)
                got_dimension = True
            if "ibrav" in key:
                global ibrav
                ibrav = int(value)
            if "nbnd" in key:
                global nbnd
                nbnd = int(value)
            if "fermi_energy" in key:
                global fermi_energy
                fermi_energy = float(value)
            if "Temperature" in key:
                global Temperature
                Temperature = float(value)
            if "onsite_U" in key:
                global onsite_U
                onsite_U = float(value)

#[MESH]
            if "k_mesh" in key:
                global k_mesh
                k_mesh = [int(value.split()[i]) for i in range(3)]
            if "q_mesh" in key:
                global q_mesh
                q_mesh = [int(value.split()[i]) for i in range(3)]
            if "w_pts" in key:
                global w_pts
                w_pts = int(value)

#[CELL]
            if section == "CELL" and index < 3:
                global cell
                cell.append([float(line.split()[i]) for i in range(3)])
                index += 1

#[BRILLOUIN_ZONE]
            if section == "BRILLOUIN_ZONE" and index < 3:
                global brillouin_zone
                brillouin_zone.append([float(line.split()[i]) for i in range(3)])
                index += 1

#[BANDS]
            if "band" in key:
                global band
                band.append(value)
            if "eff_mass" in key:
                global eff_mass
                eff_mass.append(float(value))
            if "t0" in key:
                global t0
                t0.append(float(value))
            if "t1" in key:
                global t1
                t1.append(float(value))
            if "t2" in key:
                global t2
                t2.append(float(value))
            if "t3" in key:
                global t3
                t3.append(float(value))
            if "t4" in key:
                global t4
                t4.append(float(value))
            if "t5" in key:
                global t5
                t5.append(float(value))
            if "t6" in key:
                global t6
                t6.append(float(value))
            if "t7" in key:
                global t7
                t7.append(float(value))
            if "t8" in key:
                global t8
                t8.append(float(value))
            if "t9" in key:
                global t9
                t9.append(float(value))
            if "t10" in key:
                global t10
                t10.append(float(value))

#[SUPERCONDUCTOR]
            if "method" in key:
                global method
                method = value
            if "FS_only" in key:
                global FS_only
                FS_only = bool(value)
            if "bcs_cutoff_frequency" in key:
                global bcs_cutoff_frequency
                bcs_cutoff_frequency = float(value)
            if "num_eigenvalues_to_save" in key:
                global num_eigenvalues_to_save
                num_eigenvalues_to_save = int(value)
            if "frequency_pts" in key:
                global frequency_pts
                frequency_pts = int(value)

#[RESPONSE]
            if "dynamic" in key:
                global dynamic
                dynamic = bool(value)
            # Finished setting variables
        if not brillouin_zone:
            print("Error: Brillouin zone not specified.")
            brillouin_zone = BZ_from_cell(cell)
        if len(band) != nbnd and nbnd != 1:
            print("Error: Number of bands does not match number of bands specified in input.")
            sys.exit(1)
        nbnd = len(band)

