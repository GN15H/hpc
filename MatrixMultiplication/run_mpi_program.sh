#!/bin/bash
scp -i ~/.ssh/id_ed25519_vm ./matrix_mult_openmpi.c sysadmin@master:/home/sysadmin/matrixmult
ssh -i ~/.ssh/id_ed25519_vm sysadmin@master "cd matrixmult; mpicc -o matrix_mult_openmpi matrix_mult_openmpi.c ;./replicate_program.sh ; mpiexec -np $2 --hostfile hosts.txt ./matrix_mult_openmpi $1 $2"
