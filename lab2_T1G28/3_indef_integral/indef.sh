#!/bin/bash
#SBATCH --job-name=indef_integral_seq
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=1
#SBATCH --time=00:10:00
#SBATCH --output=indef_integral_seq.out
#SBATCH --error=indef_integral_seq.err

module load OpenMPI/4.1.2-GCC-10.2.0-with-slurm

make seq || exit 1

mpirun ./indef_integral_seq 1000 10