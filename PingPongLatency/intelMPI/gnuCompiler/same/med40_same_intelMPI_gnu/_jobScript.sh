#!/bin/bash
#SBATCH -t 04:20:00
#SBATCH -N 1
#SBATCH --ntasks-per-node=2
#SBATCH -p medium40:shared
#SBATCH --mem=170G
#SBATCH --job-name='_same_Node' 
#SBATCH -o %x-%j.out # File to which STDOUT will be written
#SBATCH -e %x-%j.err # File to which STDERR will be written

module load gcc/9.3.0 
module load impi/2021.6

export SLURM_CPU_BIND=none

mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 2=33
mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 3=20
mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 5=15
mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 7=12
mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 11=10
mpirun -ppn 2 /home/gzextschak/Code_Files/PingPongLatency/intelMPI/pingPongV3_intelMPI_gnu.exe -A 13=9