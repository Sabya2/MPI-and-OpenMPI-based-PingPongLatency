#!/bin/bash
#SBATCH -t 03:20:00
#SBATCH -N 2
#SBATCH --ntasks-per-node=1
#SBATCH -p medium40:shared
#SBATCH --mem=170G
#SBATCH --job-name='_diff_Node'
#SBATCH -o %x-%j.out # File to which STDOUT will be written
#SBATCH -e %x-%j.err # File to which STDERR will be written

module load gcc/9.3.0
module load openmpi/gcc.9/4.1.4

export SLURM_CPU_BIND=none

mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 2=36
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 3=22
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 5=15
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 7=12
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 11=10
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 13=9
