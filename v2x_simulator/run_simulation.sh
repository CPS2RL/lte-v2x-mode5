#!/bin/bash


show_welcome_message()
{

  echo
  echo "################################################"
  echo "LTE-V2X Simulator Version 0.417"
  echo "Usage: run the simulation as follows."
  echo "1. For printing messgae outputs in console: ./run_simulation 0"
  echo "2. For printing in log file: ./run_simulation 1"
  echo "Note: log files will be saved in v2xlog.txt"
  echo "################################################"
  echo


}   # end of show_welcome_message


show_error_message()
{

  echo "Invalid arguments. Use the following:"
  echo "1. For printing outputs in console: ./run_simulation 0"
  echo "2. For printing in log file: ./run_simulation 1"
  echo "Note: log files will be saved in v2xlog.txt"
  echo

}   # end of show_error_message

# main script
if [ "$#" -ne 1 ]; then
  show_welcome_message
  exit 0
else
  if [ "$1" = 0 ]; then
    echo "Running simulation (console traces)..."
    ./waf --run lte-v2x 
    exit 0
  elif [ "$1" = 1 ]; then
    echo "Running simulation (traces are in logfile)..."
    ./waf --run lte-v2x > v2xlog.txt 2>&1
    echo "Simulation finished! Simulation logs are saved in v2xlog.txt"
    exit 0
  else
    show_error_message
  fi
fi
