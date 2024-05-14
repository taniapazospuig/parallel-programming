#!/bin/bash
#SBATCH --job-name=cg_cpu
#SBATCH --output=%x_%j.out
#SBATCH --error=%x_%j.err
#SBATCH --partition=ippd-gpu
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --gres=gpu:1
#SBATCH --time=00:01:35

module load NVHPC

make cpu || exit 1
./cg_cpu.x

