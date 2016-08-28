#!/usr/bin/env python
# -*- coding: utf-8 -*-
"""
A program to visualize the clemson grade distribution
Copyright © 2016 Robert Underwood
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in the
documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY Robert Underwood ''AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL Robert Underwood BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


The views and conclusions contained in the software and documentation
are those of the authors and should not be interpreted as representing
official policies, either expressedor implied, of Robert Underwood.
"""

from argparse import ArgumentParser
import numpy as np
import pandas as pd
import seaborn as sns


def prepare_data(filename):
    """take the filename and create a pandas dataframe"""
    excel_file = pd.ExcelFile(filename)
    raw_data = excel_file.parse(excel_file.sheet_names[0])
    raw_data.replace('#', 0, inplace=True)
    raw_data['is_honors'].replace(np.NaN, False, inplace=True)
    raw_data['is_honors'].replace('H', True, inplace=True)
    raw_data['A'] = raw_data['enrolled'] * raw_data['A']
    raw_data['B'] = raw_data['enrolled'] * raw_data['B']
    raw_data['C'] = raw_data['enrolled'] * raw_data['C']
    raw_data['D'] = raw_data['enrolled'] * raw_data['D']
    raw_data['F'] = raw_data['enrolled'] * raw_data['F']
    raw_data['withdraw'] = raw_data['enrolled'] * raw_data['withdraw']
    raw_data['incomplete'] = raw_data['enrolled'] * raw_data['incomplete']
    raw_data['pass_grade'] = raw_data['enrolled'] * raw_data['pass_grade']
    raw_data['fail_grade'] = raw_data['enrolled'] * raw_data['fail_grade']
    raw_data['special'] = raw_data['enrolled'] * raw_data['special']
    raw_data['course_level'] = (raw_data['course_number'] // 1000) * 1000
    raw_data['difficulty'] = (raw_data['A']*4 + raw_data['B']*3 + raw_data['C']*2 + raw_data['D']) / raw_data['enrolled']
    raw_data['difficulty_rounded'] = (raw_data['difficulty'] // .5) / 2
    raw_data = raw_data.query('A != 0 and B != 0 and C != 0 and D != 0 and F !=0')
    return raw_data

def parse_args():
    """parse arguments"""
    parser = ArgumentParser()
    parser.add_argument("file")

    return parser.parse_args()

def main():
    """main routine"""
    args = parse_args()
    sns.set(style="whitegrid", color_codes=True)
    raw_data = prepare_data(args.file)
    sns.stripplot(x=raw_data.course_number,y=raw_data.enrolled, hue=raw_data.course)
    sns.plt.show()
    sns.stripplot(x=raw_data.course, y=raw_data.course_level, hue=raw_data.difficulty_rounded, jitter=True, palette=sns.color_palette("coolwarm",6))
    sns.plt.show()





if __name__ == "__main__":
    main()
