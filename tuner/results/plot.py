#! /usr/bin/python

import re
import os
import os.path
import subprocess
import matplotlib as mpl

import math
import numpy

mpl.use('agg')

import matplotlib.pyplot as plt
import numpy as np

def config_matplotlib():
    plt.rc('text', usetex = True)
    plt.rc('font', family = 'serif')

    font = {'family' : 'serif',
            'size'   : 38}

    mpl.rc('font', **font)

def generate_line_plot(x_data,
                       y_data,
                       filename,
                       plot_title,
                       xlabel,
                       ylabel,
                       ymax,
                       ymin):

    fig     = plt.figure(1, figsize=(18, 10))
    ax      = fig.add_subplot(111)

    ax.plot(x_data, y_data, '-o')

    ax.set_title(plot_title)
    ax.set_xlabel(xlabel)

    ax.set_ylabel(ylabel)

    ax.set_ylim([ymin - 0.0005, ymax + 0.0005])
    plt.tight_layout()

    fig.savefig("{0}.eps".format(filename), format = 'eps', dpi = 2000, bbox_inches = 'tight')

    plt.clf()

def generate_boxplot(data,
                     labels,
                     filename,
                     plot_title,
                     xlabel,
                     ylabel,
                     ymax,
                     ymin,
                     index_range,
                     label_rot = '45'):

    indexes = np.arange(index_range)
    fig     = plt.figure(1, figsize=(18, 10))
    ax      = fig.add_subplot(111)
    width   = .5

    ax.boxplot(data, labels = labels)

    ax.set_title(plot_title)
    ax.set_xlabel(xlabel)

    ax.set_ylabel(ylabel)
    ax.set_xticklabels(labels, rotation = label_rot)
    ax.axhline(y = 1, color="r")

    ax.set_ylim([ymin - 0.07, ymax + 0.07])
    plt.tight_layout()

    fig.savefig("{0}.eps".format(filename), format = 'eps', dpi = 2000, bbox_inches = 'tight')

    plt.clf()

def run_cacti_config(filepath, target_path):
    subprocess.run("../../cacti_hp/cacti -infile {0} > {1}".format(filepath, target_path),
                   shell = True,
                   check = False)

def load_data(run_names, target_file, runs, best_ids):
    data = []
    partial_data = []

    default_file = open("{0}/default_cache_output.log".format(run_names[0].split("/")[0]))
    default_data = default_file.readlines()
    default_file.close()

    default_access_time_value = float(default_data[0].split("  Access time (ns): ")[1])
    default_min_power_value = float(default_data[1].split("  power : ( ")[1].split(",")[0])
    default_max_power_value = float(default_data[1].split("  power : ( ")[1].split(",")[1].split(")")[0])
    default_area_value = float(default_data[2].split("  Area: ")[1])

    run_id = 0

    for name in run_names:
        area_data        = []
        min_power_data   = []
        max_power_data   = []
        access_time_data = []

        for run in range(1, runs + 1):
            file      = open("{0}/{1}/{2}".format(name, run, target_file))
            run_data  = file.readlines()
            file.close()

            if not os.path.isfile("{0}/{1}/config_output.log".format(name, run)):
                run_cacti_config("{0}/{1}/final_tuned.cfg".format(name, run),
                                 "{0}/{1}/config_output.log".format(name, run))

            tuned_file   = open("{0}/{1}/config_output.log".format(name, run))
            tuned_data = tuned_file.readlines()
            tuned_file.close()

            new_access_time_value = float(tuned_data[0].split("  Access time (ns): ")[1]) / default_access_time_value
            new_min_power_value = float(tuned_data[1].split("  power : ( ")[1].split(",")[0]) / default_min_power_value
            new_max_power_value = float(tuned_data[1].split("  power : ( ")[1].split(",")[1].split(")")[0]) / default_max_power_value
            new_area_value = float(tuned_data[2].split(" Area: ")[1]) / default_area_value

            area_data.append(new_area_value)
            min_power_data.append(new_min_power_value)
            max_power_data.append(new_max_power_value)
            access_time_data.append(new_access_time_value)

            if run == best_ids[run_id]:
                generate_line_plot([float(d.split()[0]) for d in run_data],
                                   [float(d.split()[1]) for d in run_data],
                                   "{0}_{1}_best".format(name.split("/")[0], name.split("/")[1]),
                                   "Best Tuned ``{0}\'\'CACTI {1} during 15 minutes of Tuning".format(name.split("/")[1], name.split("/")[0].split("_")[1].title()),
                                   "Time (s)",
                                   name.split("/")[0].split("_")[1].title(),
                                   max([float(d.split()[1]) for d in run_data]),
                                   min([float(d.split()[1]) for d in run_data]))

        data.append(("{0}-area".format(name), area_data))
        data.append(("{0}-min-p".format(name), min_power_data))
        data.append(("{0}-max-p".format(name), max_power_data))
        data.append(("{0}-acc-t".format(name), access_time_data))

        partial_data.append(("area", area_data))
        partial_data.append(("min-p", min_power_data))
        partial_data.append(("max-p", max_power_data))
        partial_data.append(("acc-t", access_time_data))

        generate_boxplot([d[1] for d in partial_data],
                         [d[0] for d in partial_data],
                         "{0}_{1}".format(name.split("/")[0], name.split("/")[1]),
                         "Relative ``{0}\'\' CACTI Metrics after 15 minutes of Tuning, 8 Runs".format(name.split("/")[1]),
                         "Target CACTI ``cache type\'\'",
                         "Value Relative to Starting Point",
                         max([max(d[1]) for d in partial_data]),
                         min([min(d[1]) for d in partial_data]),
                         len(run_names),
                         label_rot = '0')

        partial_data = []
        run_id += 1

    ymax = max([max(d[1]) for d in data])
    ymin = min([min(d[1]) for d in data])

    return data, ymax, ymin

if __name__ == '__main__':
    config_matplotlib()

    target_name = "target_area_900_1"

    run_names = ["{0}/ram".format(target_name),
                 "{0}/cache".format(target_name),
                 "{0}/main-memory".format(target_name)]

    best_run_ids = [1, 7, 1]

    target_file = "best_over_time.log"
    runs = 8

    data, ymax, ymin = load_data(run_names, target_file, runs, best_run_ids)

    generate_boxplot([d[1] for d in data],
                     [d[0].split("/")[1] for d in data],
                     "{0}".format(run_names[0].split("/")[0]),
                     "Relative CACTI Metrics after 15 minutes of Tuning, 8 Runs",
                     "Target CACTI ``cache type\'\'",
                     "Value Relative to Starting Point",
                     ymax,
                     ymin,
                     len(run_names))
