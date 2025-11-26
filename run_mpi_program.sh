#!/bin/bash
scp -i ~/.ssh/id_ed25519_vm ./cellular_automaton_openmpi.c sysadmin@master:/home/sysadmin/cellular_automaton
ssh -i ~/.ssh/id_ed25519_vm sysadmin@master "cd cellular_automaton; mpicc -o cellular_automaton_openmpi cellular_automaton_openmpi.c ;./replicate_program.sh ; mpiexec -np $3 --hostfile hosts.txt ./cellular_automaton_openmpi $1 $2 $3"
