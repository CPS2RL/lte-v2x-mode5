"""
Created on 7/29/2019 10:50 AM
@author: Monowar Hasan
"""

import gt_dist_vs_prr_by_per as kpi_prr
import gt_dist_vs_pir_ia_e2ed_by_per as kpi_pir_ia_e2ed


def generate_all_kpi_trace(sim_duration, input_trace_filename, output_filename_prr,
                           output_path_pir, output_percentile_filename_pir,
                           output_path_ia, output_percentile_filename_ia,
                           output_cdf_path_e2ed, output_percentile_filename_e2ed,
                           optional_txt):

    # calculate dist-vs-avg-prr
    kpi_prr.calculate_and_write_prr(input_trace_filename=input_trace_filename, output_filename=output_filename_prr,
                                    optional_txt=optional_txt)

    # calculate rest of the KPIs
    kpi_pir_ia_e2ed.extract_pir_ia_e2ed_data(sim_duration=sim_duration, trace_filename=input_trace_filename,
                                             output_path_pir= output_path_pir,
                                             output_percentile_filename_pir=output_percentile_filename_pir,
                                             output_path_ia=output_path_ia,
                                             output_percentile_filename_ia=output_percentile_filename_ia,
                                             output_cdf_path_e2ed=output_cdf_path_e2ed,
                                             output_percentile_filename_e2ed=output_percentile_filename_e2ed,
                                             optional_txt=optional_txt)


if __name__ == '__main__':

    g_sim_duration = 4500  # [Simulation Duration -> ms]

    
	# ============================== 
	# RC-CC basic
	# ============================== 
	
	# J3161
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_10KMH.txt",
                           optional_txt="J3161")

    # J3161_10MHz
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_10MHz_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_10MHz_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_10MHz_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_10MHz_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_10MHz_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_10MHz_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_10MHz_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_10MHz_10KMH.txt",
                           optional_txt="J3161_10MHz")


    # J3161_PC
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_PC_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_PC_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_PC_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_PC_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_PC_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_PC_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_PC_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_PC_10KMH.txt",
                           optional_txt="J3161_PC")



    # J3161_PC_10MHz
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                         input_trace_filename="J3161_PC_10MHz_10KMH_per.txt",
                         output_filename_prr="distVSprr_J3161_PC_10MHz_10KMH.txt",
                         output_path_pir="dist_vs_pir_cdf/J3161_PC_10MHz_10KMH/",
                         output_percentile_filename_pir="distVS95pir_J3161_PC_10MHz_10KMH.txt",
                         output_path_ia="dist_vs_ia_cdf/J3161_PC_10MHz_10KMH/",
                         output_percentile_filename_ia="distVS95ia_J3161_PC_10MHz_10KMH.txt",
                         output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_PC_10MHz_10KMH/",
                         output_percentile_filename_e2ed= "distVS95e2ed_J3161_PC_10MHz_10KMH.txt",
                         optional_txt="J3161_PC_10MHz")

    # J3161_NRC_PC
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                             input_trace_filename="J3161_NRC_PC_10KMH_per.txt",
                             output_filename_prr="distVSprr_J3161_NRC_PC_10KMH.txt",
                             output_path_pir="dist_vs_pir_cdf/J3161_NRC_PC_10KMH/",
                             output_percentile_filename_pir="distVS95pir_J3161_NRC_PC_10KMH.txt",
                             output_path_ia="dist_vs_ia_cdf/J3161_NRC_PC_10KMH/",
                             output_percentile_filename_ia="distVS95ia_J3161_NRC_PC_10KMH.txt",
                             output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_NRC_PC_10KMH/",
                             output_percentile_filename_e2ed= "distVS95e2ed_J3161_NRC_PC_10KMH.txt",
                             optional_txt="J3161_NRC_PC")



    # J3161_NRC_PC_10MHz
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                             input_trace_filename="J3161_NRC_PC_10MHz_10KMH_per.txt",
                             output_filename_prr="distVSprr_J3161_NRC_PC_10MHz_10KMH.txt",
                             output_path_pir="dist_vs_pir_cdf/J3161_NRC_PC_10MHz_10KMH/",
                             output_percentile_filename_pir="distVS95pir_J3161_NRC_PC_10MHz_10KMH.txt",
                             output_path_ia="dist_vs_ia_cdf/J3161_NRC_PC_10MHz_10KMH/",
                             output_percentile_filename_ia="distVS95ia_J3161_NRC_PC_10MHz_10KMH.txt",
                             output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_NRC_PC_10MHz_10KMH/",
                             output_percentile_filename_e2ed= "distVS95e2ed_J3161_NRC_PC_10MHz_10KMH.txt",
                             optional_txt="J3161_NRC_PC_10MHz")

    
	# J3161_NRC_NPC
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                             input_trace_filename="J3161_NRC_NPC_10KMH_per.txt",
                             output_filename_prr="distVSprr_J3161_NRC_NPC_10KMH.txt",
                             output_path_pir="dist_vs_pir_cdf/J3161_NRC_NPC_10KMH/",
                             output_percentile_filename_pir="distVS95pir_J3161_NRC_NPC_10KMH.txt",
                             output_path_ia="dist_vs_ia_cdf/J3161_NRC_NPC_10KMH/",
                             output_percentile_filename_ia="distVS95ia_J3161_NRC_NPC_10KMH.txt",
                             output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_NRC_NPC_10KMH/",
                             output_percentile_filename_e2ed= "distVS95e2ed_J3161_NRC_NPC_10KMH.txt",
                             optional_txt="J3161_NRC_NPC")
							 
	# J3161_NRC_NPC_10MHz
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                             input_trace_filename="J3161_NRC_NPC_10MHz_10KMH_per.txt",
                             output_filename_prr="distVSprr_J3161_NRC_NPC_10MHz_10KMH.txt",
                             output_path_pir="dist_vs_pir_cdf/J3161_NRC_NPC_10MHz_10KMH/",
                             output_percentile_filename_pir="distVS95pir_J3161_NRC_NPC_10MHz_10KMH.txt",
                             output_path_ia="dist_vs_ia_cdf/J3161_NRC_NPC_10MHz_10KMH/",
                             output_percentile_filename_ia="distVS95ia_J3161_NRC_NPC_10MHz_10KMH.txt",
                             output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_NRC_NPC_10MHz_10KMH/",
                             output_percentile_filename_e2ed= "distVS95e2ed_J3161_NRC_NPC_10MHz_10KMH.txt",
                             optional_txt="J3161_NRC_NPC_10MHz")
	
	
	# ============================== 
	# NO HARQ
	# ============================== 

    # J3161_NO_HARQ
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                             input_trace_filename="J3161_NO_HARQ_10KMH_per.txt",
                             output_filename_prr="distVSprr_J3161_NO_HARQ_10KMH.txt",
                             output_path_pir="dist_vs_pir_cdf/J3161_NO_HARQ_10KMH/",
                             output_percentile_filename_pir="distVS95pir_J3161_NO_HARQ_10KMH.txt",
                             output_path_ia="dist_vs_ia_cdf/J3161_NO_HARQ_10KMH/",
                             output_percentile_filename_ia="distVS95ia_J3161_NO_HARQ_10KMH.txt",
                             output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_NO_HARQ_10KMH/",
                             output_percentile_filename_e2ed= "distVS95e2ed_J3161_NO_HARQ_10KMH.txt",
                             optional_txt="J3161_NO_HARQ")

    # J3161_10MHz_NO_HARQ
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                               input_trace_filename="J3161_10MHz_NO_HARQ_10KMH_per.txt",
                               output_filename_prr="distVSprr_J3161_10MHz_NO_HARQ_10KMH.txt",
                               output_path_pir="dist_vs_pir_cdf/J3161_10MHz_NO_HARQ_10KMH/",
                               output_percentile_filename_pir="distVS95pir_J3161_10MHz_NO_HARQ_10KMH.txt",
                               output_path_ia="dist_vs_ia_cdf/J3161_10MHz_NO_HARQ_10KMH/",
                               output_percentile_filename_ia="distVS95ia_J3161_10MHz_NO_HARQ_10KMH.txt",
                               output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_10MHz_NO_HARQ_10KMH/",
                               output_percentile_filename_e2ed= "distVS95e2ed_J3161_10MHz_NO_HARQ_10KMH.txt",
                               optional_txt="J3161_NO_HARQ_10MHz")
				

	# Fixed T2
	
	# J3161_T2_20
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_T2_20_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_T2_20_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_T2_20_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_T2_20_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_T2_20_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_T2_20_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_T2_20_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_T2_20_10KMH.txt",
                           optional_txt="J3161_T2_20")
						   
	# J3161_T2_50
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_T2_50_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_T2_50_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_T2_50_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_T2_50_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_T2_50_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_T2_50_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_T2_50_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_T2_50_10KMH.txt",
                           optional_txt="J3161_T2_50")
	
	# J3161_T2_100
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_T2_100_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_T2_100_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_T2_100_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_T2_100_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_T2_100_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_T2_100_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_T2_100_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_T2_100_10KMH.txt",
                           optional_txt="J3161_T2_100")
						   
	# J3161_10MHz_T2_20
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_10MHz_T2_20_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_10MHz_T2_20_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_10MHz_T2_20_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_10MHz_T2_20_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_10MHz_T2_20_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_10MHz_T2_20_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_10MHz_T2_20_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_10MHz_T2_20_10KMH.txt",
                           optional_txt="J3161_10MHz_T2_20")
	
	
	# J3161_10MHz_T2_50
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_10MHz_T2_50_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_10MHz_T2_50_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_10MHz_T2_50_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_10MHz_T2_50_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_10MHz_T2_50_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_10MHz_T2_50_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_10MHz_T2_50_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_10MHz_T2_50_10KMH.txt",
                           optional_txt="J3161_10MHz_T2_50")
	
	
	
	# J3161_10MHz_T2_100
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_10MHz_T2_100_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_10MHz_T2_100_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_10MHz_T2_100_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_10MHz_T2_100_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_10MHz_T2_100_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_10MHz_T2_100_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_10MHz_T2_100_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_10MHz_T2_100_10KMH.txt",
                           optional_txt="J3161_10MHz_T2_100")
	
	# ============================== 
	# Different pKeep
	# ============================== 
	
	
	
	# J3161_pKeep_20
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_pKeep_20_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_pKeep_20_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_pKeep_20_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_pKeep_20_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_pKeep_20_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_pKeep_20_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_pKeep_20_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_pKeep_20_10KMH.txt",
                           optional_txt="J3161_pKeep_20")
						   
	# J3161_10MHz_pKeep_20
    generate_all_kpi_trace(sim_duration=g_sim_duration,
                           input_trace_filename="J3161_10MHz_pKeep_20_10KMH_per.txt",
                           output_filename_prr="distVSprr_J3161_10MHz_pKeep_20_10KMH.txt",
                           output_path_pir="dist_vs_pir_cdf/J3161_10MHz_pKeep_20_10KMH/",
                           output_percentile_filename_pir="distVS95pir_J3161_10MHz_pKeep_20_10KMH.txt",
                           output_path_ia="dist_vs_ia_cdf/J3161_10MHz_pKeep_20_10KMH/",
                           output_percentile_filename_ia="distVS95ia_J3161_10MHz_pKeep_20_10KMH.txt",
                           output_cdf_path_e2ed="dist_vs_e2ed_cdf/J3161_10MHz_pKeep_20_10KMH/",
                           output_percentile_filename_e2ed= "distVS95e2ed_J3161_10MHz_pKeep_20_10KMH.txt",
                           optional_txt="J3161_10MHz_pKeep_20")



    print "All KPI Generation Script Finished!!"
