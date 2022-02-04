"""
Created on 6/27/2019 4:00 PM
@author: Monowar Hasan
"""

import dataparser as dp


if __name__ == '__main__':

    # root_trace_dir = "../v2x_simulator/v2x_traces/kpi_traces/"
    # in_trace_file_10Hz = "10MHz_HARQ_DIS_CC_ENB_CBR_T2_V500_50KMH_5s.txt"
    # in_trace_file_20Hz = "20MHz_HARQ_DIS_CC_ENB_CBR_T2_V500_50KMH_5s.txt"
    #
    # out_trace_file_10Hz = "10MHz_HARQ_DIS_CC_ENB_CBR_T2_V500_50KMH_3s.txt"
    # out_trace_file_20Hz = "20MHz_HARQ_DIS_CC_ENB_CBR_T2_V500_50KMH_3s.txt"
    #
    # in_fname_10Hz = root_trace_dir + in_trace_file_10Hz
    # in_fname_20Hz = root_trace_dir + in_trace_file_20Hz
    #
    # out_fname_10Hz = root_trace_dir + out_trace_file_10Hz
    # out_fname_20Hz = root_trace_dir + out_trace_file_20Hz

    # arrival_time_th_1 = 2000
    # arrival_time_th_2 = 3000

    # dp.data_trimmer(in_fname_20Hz, out_fname_20Hz, arrival_time_th_1, arrival_time_th_2)

    arrival_time_th_1 = 4000
    arrival_time_th_2 = 5000

    dp.data_trimmer(input_filename='10MHz_HARQ_DIS_RC_DIS_PC_ENB_CBR_T2_10KMH_5s_2200TX_per.txt',
                    output_filename='10MHz_HARQ_DIS_RC_DIS_PC_ENB_CBR_T2_10KMH_5s_2200TX_per_trimmed.txt',
                    arrival_time_th_1= arrival_time_th_1,
                    arrival_time_th_2= arrival_time_th_2)

    dp.data_trimmer(input_filename='10MHz_HARQ_DIS_RC_DIS_PC_DIS_CBR_T2_10KMH_5s_2200TX_per.txt',
                    output_filename='10MHz_HARQ_DIS_RC_DIS_PC_DIS_CBR_T2_10KMH_5s_2200TX_per_trimmed.txt',
                    arrival_time_th_1= arrival_time_th_1,
                    arrival_time_th_2= arrival_time_th_2)





    print "Script finished!"