#! /usr/bin/python

import re
import os
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
            'size'   : 28}

    mpl.rc('font', **font)

def generate_boxplot(data,
                     labels,
                     filename,
                     plot_title,
                     xlabel,
                     ylabel,
                     ymax,
                     ymin,
                     index_range):

    indexes = np.arange(index_range)
    fig     = plt.figure(1, figsize=(18, 10))
    ax      = fig.add_subplot(111)
    width   = .5

    ax.boxplot(data, labels = labels)

    ax.set_title(plot_title)
    ax.set_xlabel(xlabel)
    #ax.set_xticks(indexes + (.5 * width))
    ax.set_ylabel(ylabel)
    #ax.set_xticklabels(labels, rotation = '0')

    ax.set_ylim([ymin - 0.01, ymax + 0.01])
    plt.tight_layout()

    fig.savefig("{0}.eps".format(filename), format = 'eps', dpi = 2000)

    plt.clf()

def load_data(run_names, target_file, runs):
    data = []
    ymax = -1
    ymin = float('Inf')

    for name in run_names:
        name_data = []

        for run in range(1, runs + 1):
            file      = open("{0}/{1}/{2}".format(name, run, target_file))
            run_data  = file.readlines()
            new_value = float(run_data[-1].split()[1]) / float(run_data[0].split()[1])

            if new_value > ymax:
                ymax = new_value

            if new_value < ymin:
                ymin = new_value

            name_data.append(new_value)
            file.close()

        data.append((name, name_data))

    return data, ymax, ymin

if __name__ == '__main__':
    config_matplotlib()

    run_names = ["target_area_900/ram",
                 "target_area_900/cache",
                 "target_area_900/main-memory"]

    target_file = "best_over_time.log"
    runs = 8

    data, ymax, ymin = load_data(run_names, target_file, runs)
    print(data, ymax, ymin)

    generate_boxplot([d[1] for d in data],
                     [d[0].split("/")[1] for d in data],
                     "target_area_900",
                     "Relative CACTI Area after 15 minutes of Tuning, 8 Runs",
                     "Target CACTI ``cache type\'\'",
                     "Value Relative to Starting Point",
                     ymax,
                     ymin,
                     len(run_names))
