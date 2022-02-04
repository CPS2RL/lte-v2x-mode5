#!/bin/sh

# input and output filenames
SUMO_CFG_FILENAME="sumo_sim_10kmh_l3/v2xsim.sumocfg"   # for 10 km/h
# SUMO_CFG_FILENAME="sumo_sim_100kmh_l3/v2xsim.sumocfg"   # for 100 km/h
OUTPUT_TCL_FILENAME="v2xsim_ns3_out.tcl"

# trace begin and end time 
# (we drop from some initial time 
# to take only steady state values)
TRACE_BEGIN_TIME="980"
TRACE_END_TIME="1000"



TMP_XML_FILENAME="sumoTrace.xml"



# create xml file
echo "Generating XML file.."
../../sumo_src_1.2.0/bin/./sumo -c $SUMO_CFG_FILENAME \
  --fcd-output $TMP_XML_FILENAME

# if trace generation sucessful:
if [ $? -eq 0 ]; then
  # create TCL file
  echo ""
  echo "Generating TCL file.."
  python ../../sumo_src_1.2.0/tools/traceExporter.py \
    --fcd-input $TMP_XML_FILENAME \
    --ns2mobility-output $OUTPUT_TCL_FILENAME \
    --begin $TRACE_BEGIN_TIME \
    --end $TRACE_END_TIME
  
  echo "Shifting trace.."
  python ns3_trace_shifter.py $OUTPUT_TCL_FILENAME

else
  echo "FAILED to generate XML file. Aborting..."
fi



echo "Removing Temorary files.."
rm -r $TMP_XML_FILENAME

echo "Script finished.."