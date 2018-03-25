#!/bin/bash

pkill -f places_server
pkill -f airports_server

echo "run_all: Building.."
make clean
make

echo "run_all: Launching airports_server.."
./airports_server &
sleep 0.5

echo "run_all: Launching places_server.."
./places_server localhost &
sleep 0.5

echo "run_all: Executing test query.."
./client localhost seattle wa

echo "run_all: Note: airports_server and places_server are still running."