#!/bin/bash
#SBATCH -t 02:60:00
#SBATCH -N 1
#SBATCH --ntasks-per-node=2
#SBATCH -p standard96
##SBATCH -c 1
#SBATCH --mem=360G
#SBATCH --job-name='_same_Node' 
#SBATCH -o %x-%j.out # File to which STDOUT will be written
#SBATCH -e %x-%j.err # File to which STDERR will be written

module load intel/2022.2 
module load impi/2021.6

export SLURM_CPU_BIND=none

mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 2=33
mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 3=20
mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 5=15
mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 7=12
mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 11=10
mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_intel.exe -A 13=9