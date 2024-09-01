Needed module for this directory are 

# to get the tree of any directory


module load impi/2021.6 intel/2022.2 slurm
Compile the pingPong.c for both MPI and openMP using the intel compiler

commmand line code for gcc compilation, MPI and openMP both 
    -- mpiicc -o pingPongV3_MPI.exe pingPongV3.c
    -- mpiicc -qopenmp -o pingPongV3_openMp.exe pingPongV3.c


    -- Slurm job script for different and same nodes
    -- partitions used are medium40:shared and standard96:shared
        -- sbatch _jobscript.sh         #commandline to submit the job
        -- common batch prerequisites: SBATCH -t, --job-name, -o,-e, -c, #!/bin/bash

            #SBATCH -N 2
            #SBATCH --ntasks-per-node=1
            #SBATCH --job-name='diff_Node' 
            -ppn 1

            #SBATCH -N 1
            #SBATCH --ntasks-per-node=2
            #SBATCH --job-name='same_Node' 
            -ppn 2
