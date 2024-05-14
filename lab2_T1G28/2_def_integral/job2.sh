#!/bin/bash
#SBATCH -J mpi_comms
#SBATCH --output=out_%j.out
#SBATCH --error=out_%j.err
#SBATCH -p ippd-cpu
#SBATCH --time=00:02:00
#SBATCH --nodes=1
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=1

module load GCC/10.2.0
module load OpenMPI/4.1.2-GCC-10.2.0-with-slurm

export OMP_NUM_THREADS=1

make >> make.out || exit 1 # Exit if make fails

# Large L = 10000
#./def_integral_seq 10 10000 # few points
#./def_integral_seq 100 10000 # more points
#./def_integral_seq 1000 10000 # many points

# Large L = 10000
#mpirun -n 4 ./def_integral_par 10 10000 # few points
#mpirun -n 4 ./def_integral_par 100 10000 # more points
#mpirun -n 4 ./def_integral_par 1000 10000 # many points

# Larger L = 1000000
#./def_integral_seq 10 100000 # few points
#./def_integral_seq 100 100000 # more points
#./def_integral_seq 1000 100000 # many points

# Larger L = 1000000
#mpirun -n 4 ./def_integral_par 10 100000 # few points
#mpirun -n 4 ./def_integral_par 100 100000 # more points
#mpirun -n 4 ./def_integral_par 1000 100000 # many points

# Strong scaling
./def_integral_seq 1000000000 1000
mpirun -n 4 ./def_integral_par 1000000000 1000