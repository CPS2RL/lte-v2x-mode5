"""
Created on 8/2/2019 10:15 AM
@author: Monowar Hasan
"""

import pandas as pd


class CongestionControlExpclass:

    #  the class contains all information different experiments for congestion control experiments

    def __init__(self, val_j3161, val_j3161_10mhz, val_j3161_pc, val_j3161_pc_10mhz,
                 val_j3161_nrc_pc, val_j3161_nrc_pc_10mhz, val_j3161_nrc_npc, val_j3161_nrc_npc_10mhz):
        self.val_j3161 = val_j3161
        self.val_j3161_10mhz = val_j3161_10mhz
        self.val_j3161_pc = val_j3161_pc
        self.val_j3161_pc_10mhz = val_j3161_pc_10mhz
        self.val_j3161_nrc_pc = val_j3161_nrc_pc
        self.val_j3161_nrc_pc_10mhz = val_j3161_nrc_pc_10mhz
        self.val_j3161_nrc_npc = val_j3161_nrc_npc
        self.val_j3161_nrc_npc_10mhz = val_j3161_nrc_npc_10mhz


class HARQExpclass:

    #  the class contains all information different experiments for HARQ experiments

    def __init__(self, val_j3161, val_j3161_10mhz,
                 val_j3161_no_harq, val_j3161_10mhz_no_harq):
        self.val_j3161 = val_j3161
        self.val_j3161_10mhz = val_j3161_10mhz
        self.val_j3161_no_harq = val_j3161_no_harq
        self.val_j3161_10mhz_no_harq = val_j3161_10mhz_no_harq


class FixedNCBRExpclass:

    #  the class contains all information different experiments for fixed/variable T2

    def __init__(self, val_j3161, val_j3161_10mhz,
                 val_j3161_t2_20, val_j3161_t2_50, val_j3161_t2_100,
                 val_j3161_10mhz_t2_20, val_j3161_10mhz_t2_50, val_j3161_10mhz_t2_100):
        self.val_j3161 = val_j3161
        self.val_j3161_10mhz = val_j3161_10mhz
        self.val_j3161_t2_20 = val_j3161_t2_20
        self.val_j3161_t2_50 = val_j3161_t2_50
        self.val_j3161_t2_100 = val_j3161_t2_100
        self.val_j3161_10mhz_t2_20 = val_j3161_10mhz_t2_20
        self.val_j3161_10mhz_t2_50 = val_j3161_10mhz_t2_50
        self.val_j3161_10mhz_t2_100 = val_j3161_10mhz_t2_100


class PKeepExpclass:

    #  the class contains all information different experiments for pKeep experiments

    def __init__(self, val_j3161, val_j3161_10mhz,
                 val_j3161_pkeep_20, val_j3161_10mhz_pkeep_20):
        self.val_j3161 = val_j3161
        self.val_j3161_10mhz = val_j3161_10mhz
        self.val_j3161_pkeep_20 = val_j3161_pkeep_20
        self.val_j3161_10mhz_pkeep_20 = val_j3161_10mhz_pkeep_20


# get kpi data (from postprocessing files)
def get_kpi_val_from_file(filename):

    print "Reading", filename, "and return as a Panda dataframe..."
    df = pd.read_csv(filename, delimiter=";", header=None)

    rawdata = df.values  # change to numpy array

    return rawdata[:, 2]  # col 2 is the kpi val


# get e2e delay data (from postprocessing files)
def get_e2ed_from_file(filename):

    print "Reading", filename, "and return as a Panda dataframe..."
    df = pd.read_csv(filename, delimiter=",")

    rawdata = df.values  # change to numpy array

    return rawdata  # col 0 is x, col 1 is y
