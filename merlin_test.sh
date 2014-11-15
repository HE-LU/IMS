#!/bin/bash
make clean; make --makefile=Makefile.merlin
./src/main
echo "╔═════════════════════════════════╗"
echo "║          OUTPUT BEGIN           ║"
echo "╠═════════════════════════════════╣"
echo "║               666               ║"
echo "╚═════════════════════════════════╝"
cat out

