"""
Created on 8/2/2019 10:10 AM
@author: Monowar Hasan
"""

import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np

import expclasses as ec


def plot_ia_rc_cc(data_class, output_pdf_name):
    # create plot

    # change font to Arial
    plt.rcParams["font.family"] = "Arial"
    plt.rcParams['font.size'] = 25
    plt.rcParams['legend.fontsize'] = 18
    plt.rcParams['axes.titlesize'] = 25
    plt.rcParams['ytick.labelsize'] = 18
    plt.rcParams['xtick.labelsize'] = 18
    plt.rcParams['figure.figsize'] = 10, 6

    amin = 20
    amax = 520
    distgap = 20

    distminlist = list(range(amin, amax + distgap, distgap))

    # dist_string = ["[" + str(d) + "," + str(d+distgap) + ")" for d in distminlist]
    # print dist_string


    base_lw = 3
    common_lw = 1.5

    base_mw = 1.8
    common_mw = 1.2

    base_ms = 8
    common_ms = 6

    base_opacity = 0.7
    common_opacity = 0.7

    base_color = 'xkcd:dark purple'
    common_color_20 = 'xkcd:pine'
    common_color_10 = 'xkcd:brick orange'

    base_ls = '-'
    common_ls_20 = '-'
    common_ls_10 = '--'

    base_mfill = 'none'
    common_mfill_20 = 'none'
    common_mfill_10 = 'full'

    plt.plot(distminlist, data_class.val_j3161,
             linewidth=base_lw,
             marker='o',
             linestyle=base_ls,
             color=base_color,
             alpha=base_opacity,
             fillstyle=base_mfill,
             markeredgewidth=base_mw,
             markersize=base_ms,
             label='J3161')

    plt.plot(distminlist, data_class.val_j3161_pc,
             linewidth=common_lw,
             marker='s',
             linestyle=common_ls_20,
             color=common_color_20,
             alpha=common_opacity,
             fillstyle=common_mfill_20,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_PC')

    # plt.plot(distminlist, data_class.val_j3161_nrc_pc,
    #          linewidth=common_lw,
    #          marker='d',
    #          linestyle=common_ls_20,
    #          color=common_color_20,
    #          alpha=common_opacity,
    #          fillstyle=common_mfill_20,
    #          markeredgewidth=common_mw,
    #          markersize=common_ms,
    #          label='J3161_NRC_PC')

    # plt.plot(distminlist, data_class.val_j3161_nrc_npc,
    #          linewidth=common_lw,
    #          marker='v',
    #          linestyle=common_ls_20,
    #          color=common_color_20,
    #          alpha=common_opacity,
    #          fillstyle=common_mfill_20,
    #          markeredgewidth=common_mw,
    #          markersize=common_ms,
    #          label='J3161_NRC_NPC')

    plt.plot(distminlist, data_class.val_j3161_10mhz,
             linewidth=common_lw,
             marker='o',
             linestyle=common_ls_10,
             color=common_color_10,
             alpha=common_opacity,
             fillstyle=common_mfill_10,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_10MHz')

    plt.plot(distminlist, data_class.val_j3161_pc_10mhz,
             linewidth=common_lw,
             marker='s',
             linestyle=common_ls_10,
             color=common_color_10,
             alpha=common_opacity,
             fillstyle=common_mfill_10,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_PC_10MHz')

    # plt.plot(distminlist, data_class.val_j3161_nrc_pc_10mhz,
    #          linewidth=common_lw,
    #          marker='d',
    #          linestyle=common_ls_10,
    #          color=common_color_10,
    #          alpha=common_opacity,
    #          fillstyle=common_mfill_10,
    #          markeredgewidth=common_mw,
    #          markersize=common_ms,
    #          label='J3161_NRC_PC_10MHz')

    # plt.plot(distminlist, data_class.val_j3161_nrc_npc_10mhz,
    #          linewidth=common_lw,
    #          marker='v',
    #          linestyle=common_ls_10,
    #          color=common_color_10,
    #          alpha=common_opacity,
    #          fillstyle=common_mfill_10,
    #          markeredgewidth=common_mw,
    #          markersize=common_ms,
    #          label='J3161_NRC_NPC_10MHz')

    # plt.xticks(distminlist, dist_string, rotation=80)

    plt.ylim([100, 1500])

    plt.xlabel("Vehicle-to-Vehicle Distance (m)")
    plt.ylabel("95th Percentile IA (ms)")

    plt.legend(loc='lower left', ncol=2,
               bbox_to_anchor=(0.02, 0.97), frameon=False)

    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=":", color='black', alpha=0.4)

    plt.savefig(output_pdf_name, pad_inches=0.1, bbox_inches='tight')
    plt.show()


# harq vs no harq
def plot_ia_harq_no_harq(data_class, output_pdf_name):
    # create plot

    # change font to Arial
    plt.rcParams["font.family"] = "Arial"
    plt.rcParams['font.size'] = 25
    plt.rcParams['legend.fontsize'] = 18
    plt.rcParams['axes.titlesize'] = 25
    plt.rcParams['ytick.labelsize'] = 18
    plt.rcParams['xtick.labelsize'] = 18
    plt.rcParams['figure.figsize'] = 10, 6

    amin = 20
    amax = 520
    distgap = 20

    distminlist = list(range(amin, amax + distgap, distgap))

    # dist_string = ["[" + str(d) + "," + str(d+distgap) + ")" for d in distminlist]
    # print dist_string


    base_lw = 3
    common_lw = 1.5

    base_mw = 1.8
    common_mw = 1.2

    base_ms = 8
    common_ms = 6

    base_opacity = 0.7
    common_opacity = 0.7

    base_color = 'xkcd:dark purple'
    common_color_20 = 'xkcd:pine'
    common_color_10 = 'xkcd:brick orange'

    base_ls = '-'
    common_ls_20 = '-'
    common_ls_10 = '--'

    base_mfill = 'none'
    common_mfill_20 = 'none'
    common_mfill_10 = 'full'

    plt.plot(distminlist, data_class.val_j3161,
             linewidth=base_lw,
             marker='o',
             linestyle=base_ls,
             color=base_color,
             alpha=base_opacity,
             fillstyle=base_mfill,
             markeredgewidth=base_mw,
             markersize=base_ms,
             label='J3161')

    plt.plot(distminlist, data_class.val_j3161_10mhz,
             linewidth=common_lw,
             marker='o',
             linestyle=common_ls_10,
             color=common_color_10,
             alpha=common_opacity,
             fillstyle=common_mfill_10,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_10MHz')

    plt.plot(distminlist, data_class.val_j3161_no_harq,
             linewidth=common_lw,
             marker='x',
             linestyle=common_ls_20,
             color='xkcd:very dark green',
             alpha=common_opacity,
             fillstyle=common_mfill_20,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_NO_HARQ')

    plt.plot(distminlist, data_class.val_j3161_10mhz_no_harq,
             linewidth=common_lw,
             marker='d',
             linestyle=common_ls_10,
             color='xkcd:marine blue',
             alpha=common_opacity,
             fillstyle=common_mfill_10,
             markeredgewidth=common_mw,
             markersize=common_ms,
             label='J3161_10MHz_NO_HARQ')

    # plt.xticks(distminlist, dist_string, rotation=80)

    plt.ylim([100, 1000])

    plt.xlabel("Vehicle-to-Vehicle Distance (m)")
    plt.ylabel("95th Percentile IA (ms)")

    plt.legend(loc='lower left', ncol=2,
               bbox_to_anchor=(0.02, 0.97), frameon=False)

    ax = plt.gca()
    ax.set_axisbelow(True)
    plt.grid(linestyle=":", color='black', alpha=0.4)

    plt.savefig(output_pdf_name, pad_inches=0.1, bbox_inches='tight')
    plt.show()


def get_congestion_control_data():

    val_j3161 = ec.get_kpi_val_from_file("../v2x_trace_for_plot/distVS95ia_J3161_10KMH.txt")
    val_j3161_10mhz = ec.get_kpi_val_from_file("../v2x_trace_for_plot/distVS95ia_J3161_10MHz_10KMH.txt")
    val_j3161_pc = ec.get_kpi_val_from_file("../v2x_trace_for_plot/distVS95ia_J3161_PC_10KMH.txt")
    val_j3161_pc_10mhz = ec.get_kpi_val_from_file("../v2x_trace_for_plot/distVS95ia_J3161_PC_10MHz_10KMH.txt")
    val_j3161_nrc_pc = None
    val_j3161_nrc_pc_10mhz = None
    val_j3161_nrc_npc = None
    val_j3161_nrc_npc_10mhz = None

    cc_class = ec.CongestionControlExpclass(val_j3161, val_j3161_10mhz, val_j3161_pc,
                                            val_j3161_pc_10mhz, val_j3161_nrc_pc, val_j3161_nrc_pc_10mhz,
                                            val_j3161_nrc_npc, val_j3161_nrc_npc_10mhz)

    return cc_class


def get_harq_no_harq_data():
    val_j3161 = ec.get_kpi_val_from_file(
        "../v2x_trace_for_plot/distVS95ia_J3161_10KMH.txt")
    val_j3161_10mhz = ec.get_kpi_val_from_file(
        "../v2x_trace_for_plot/distVS95ia_J3161_10MHz_10KMH.txt")
    val_j3161_no_harq = ec.get_kpi_val_from_file(
        "../v2x_trace_for_plot/distVS95ia_J3161_NO_HARQ_10KMH.txt")
    val_j3161_10mhz_no_harq = ec.get_kpi_val_from_file(
        "../v2x_trace_for_plot/distVS95ia_J3161_10MHz_NO_HARQ_10KMH.txt")

    h_class = ec.HARQExpclass(val_j3161=val_j3161, val_j3161_10mhz=val_j3161_10mhz,
                                 val_j3161_no_harq=val_j3161_no_harq, val_j3161_10mhz_no_harq=val_j3161_10mhz_no_harq)

    return h_class


if __name__ == '__main__':

    rc_pc_class = get_congestion_control_data()
    plot_ia_rc_cc(data_class=rc_pc_class, output_pdf_name='dist_vs_ia_rc_pc_10kmh.pdf')

    harq_class = get_harq_no_harq_data()
    plot_ia_harq_no_harq(data_class=harq_class, output_pdf_name='dist_vs_ia_harq_no_harq_10kmh.pdf')




    print "Plotting PIR finished"

