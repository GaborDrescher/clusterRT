#!/bin/bash -l
#
# allocate 16 nodes (64 CPUs) for 6 hours
#PBS -l nodes=8:ppn=4,walltime=00:10:00
#
# job name
#PBS -N TestRender
#
# stdout and stderr files
#PBS -o testrender.out -e testrender.err
#
# first non-empty non-comment line ends PBS options

# jobs always start in $HOME -
# change to a temporary job directory on $FASTTMP

#module add gcc/4.4.2
#module add intelmpi/3.2.2.006-gnu

#module add intel64/11.1up8
module add intel64

#mkdir ${FASTTMP}/$PBS_JOBID
#cd ${FASTTMP}/$PBS_JOBID

# run
mpirun -npernode 1 ${HOME}/rt/rt 1280 720 1200

# dont copy pictures
# scp *.ppm sidekell@faui0sr0.informatik.uni-erlangen.de:/proj/ciptmp/sidekell/rays/

# dont get rid of the temporary job dir
# rm -rf ${FASTTMP}/$PBS_JOBID
