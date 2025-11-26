#!/bin/bash
scp -i ~/.ssh/id_ed25519_vm ./cellular_automaton_openmpi.c sysadmin@master:/home/sysadmin/cellular_automaton
ssh -i ~/.ssh/id_ed25519_vm sysadmin@master "cd cellular_automaton; mpicc -o cellular_automaton_openmpi_O3 cellular_automaton_openmpi.c -O3;./replicate_programO3.sh ; mpiexec -np $3 --hostfile hosts.txt ./cellular_automaton_openmpi_O3 $1 $2 $3"
