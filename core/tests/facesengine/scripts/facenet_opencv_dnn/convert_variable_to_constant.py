#!/usr/bin/python3

# ============================================================
#
# This file is a part of digiKam project
# https://www.digikam.org
#
# Date        : 2019-08-11
# Description : faces engine test scripts
#
# Copyright (C) 2021 by Nghia Duong <minhnghiaduong997 at gmail dot com>
#
# This program is free software; you can redistribute it
# and/or modify it under the terms of the GNU General
# Public License as published by the Free Software Foundation;
# either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# ============================================================ */

import tensorflow as tf
from tensorflow.core.framework import graph_pb2
import copy
import pdb


# reference:
# https://stackoverflow.com/questions/43332342/is-it-possible-to-replace-placeholder-with-a-constant-in-an-existing-graph


INPUT_GRAPH_DEF_FILE = './models/20180402-114759/20180402-114759.pb'
OUTPUT_GRAPH_DEF_FILE = './models/graph-opt.pb'
target_node_name = 'phase_train'  # name of node that will be convert

# ---------------------------------------------------------------------------------------------------------------------


# load our graph
def load_graph(filename):
    graph_def = tf.compat.v1.GraphDef()
    with tf.compat.v2.io.gfile.GFile(filename, 'rb') as f:
        graph_def.ParseFromString(f.read())
    return graph_def

tf.compat.v1.disable_eager_execution()
graph_def = load_graph(INPUT_GRAPH_DEF_FILE)
c = tf.constant(False, dtype=bool, shape=[], name=target_node_name)

# Create new graph, and rebuild it from original one
# replacing phase train node def with constant

new_graph_def = graph_pb2.GraphDef()
for node in graph_def.node:
    if node.name == target_node_name:
        new_graph_def.node.extend([c.op.node_def])
    else:
        new_graph_def.node.extend([copy.deepcopy(node)])

# save new graph
with tf.compat.v2.io.gfile.GFile(OUTPUT_GRAPH_DEF_FILE, "wb") as f:
    f.write(new_graph_def.SerializeToString())
