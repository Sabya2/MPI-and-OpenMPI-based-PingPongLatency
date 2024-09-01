#!/bin/bash
#SBATCH -t 03:20:00
#SBATCH -N 2
#SBATCH --ntasks-per-node=1
#SBATCH -p medium40:shared
##SBATCH -c 1
#SBATCH --mem=170G
#SBATCH --job-name='_diff_Node'
#SBATCH -o %x-%j.out # File to which STDOUT will be written
#SBATCH -e %x-%j.err # File to which STDERR will be written

module load intel/2022.2 
module load impi/2021.6 

export SLURM_CPU_BIND=none

mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 2=33
mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 3=20
mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 5=15
mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 7=12
mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 11=10
mpirun -ppn 1 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 13=9