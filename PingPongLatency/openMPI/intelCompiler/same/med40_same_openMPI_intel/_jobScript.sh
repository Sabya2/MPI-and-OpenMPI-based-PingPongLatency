#!/bin/bash
#SBATCH -t 03:00:00
#SBATCH -N 1
#SBATCH --ntasks-per-node=2
#SBATCH -p medium40:shared
#SBATCH --mem=160G
#SBATCH --job-name='_same_Node' 
#SBATCH -o %x-%j.out # File to which STDOUT will be written
#SBATCH -e %x-%j.err # File to which STDERR will be written

module load intel/2022.2 
module load openmpi/intel/4.1.4

export SLURM_CPU_BIND=none

mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_intel.exe -A 2=36
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_intel.exe -A 3=22
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_intel.exe -A 5=15
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_intel.exe -A 7=12
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_intel.exe -A 11=10
mpirun /home/gzextschak/Code_Files/PingPongLatency/openMPI/pingPongV3_openMPI_intel.exe -A 13=9
