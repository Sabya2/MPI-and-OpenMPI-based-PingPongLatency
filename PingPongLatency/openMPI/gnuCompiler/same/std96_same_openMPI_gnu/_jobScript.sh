#!/bin/bash
#SBATCH -t 03:20:00
#SBATCH -N 1
#SBATCH --ntasks-per-node=2
#SBATCH -p standard96
#SBATCH --mem=360G
#SBATCH --job-name='_same_Node' 
#SBATCH -o %x-%j.out # File to which STDOUT will be written
#SBATCH -e %x-%j.err # File to which STDERR will be written

module load gcc/9.3.0 
module load openmpi/gcc.9/4.1.4

export SLURM_CPU_BIND=none

mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 2=37
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 3=23
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 5=15
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 7=13
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 11=10
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_gnu.exe -A 13=9