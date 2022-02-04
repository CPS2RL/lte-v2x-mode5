(Trace File for all KPI):

(unit: meter, dB)

# this is a comment
# ArrivalSF; TXSF; CHID; TXID; RXID; (TXPOS.x,y,z); (TXSPEED.x,y,z); (RXPOS.x,y,z); (RXSPEED.x,y,z);  DIST; SINR


ArrivalSF; TXSF; CHID; TXID; RXID; DIST; SINR


# for reselction:
vid; reselction SF

# for CBR and Arrival rate
Logging SF; vid; cbr; arrival rate


# for PER
# PER is 0 or 1 (PER=1 means failed, PER=0 means successful)
ArrivalSF; TXSF; TXID; RXID; DIST; PER
