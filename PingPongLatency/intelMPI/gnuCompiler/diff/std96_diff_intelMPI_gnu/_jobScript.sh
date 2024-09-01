#!/bin/bash
#SBATCH -t 03:40:00
#SBATCH -N 2
#SBATCH --ntasks-per-node=1
#SBATCH -p standard96
#SBATCH --mem=360G
#SBATCH --job-name='_diff_Node'
#SBATCH -o %x-%j.out # File to which STDOUT will be written
#SBATCH -e %x-%j.err # File to which STDERR will be written

module load sw.skl
module load gcc/9.3.0 
module load impi/2021.6

export SLURM_CPU_BIND=none

mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 2=33
mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 3=20
mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 5=15
mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 7=12
mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 11=10
mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 13=9