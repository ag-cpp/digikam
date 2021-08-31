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

# Method 4: Neural net
library(tensorflow)
library(keras)
library(reticulate)
library(dplyr)

reticulate::py_config()
# seed to replicate the same results in others computers
tensorflow::tf$random$set_seed(1729)

data.train.pca.X <- as.matrix(data.train.pca[, -ncol(data.train.pca)])
data.train.pca.X <- array_reshape(data.train.pca.X, c(nrow(data.train.pca.X), ncol(data.train.pca.X), 1))
data.train.pca.y <- as.numeric(data.train.pca$Y) - 1
data.train.pca.y <- to_categorical(data.train.pca.y)

data.test.pca.X <- as.matrix(data.test.pca[,-ncol(data.test.pca)])
data.test.pca.X <- array_reshape(data.test.pca.X, c(nrow(data.test.pca.X), ncol(data.test.pca.X), 1))
data.test.pca.y <- as.numeric(data.test.pca$Y) - 1
data.test.pca.y <- to_categorical(data.test.pca.y)

# neural net
model <- keras_model_sequential() %>%
  layer_flatten(input_shape = c(ncol(data.train.pca.X), 1)) %>%
  layer_dense(units = 100, activation = 'relu') %>%
  layer_dropout(0.5)%>%
  layer_dense(units = 25, activation = 'relu') %>%
  layer_dropout(0.2)%>%
  layer_dense(units = 25, activation = 'relu') %>%
  layer_dropout(0.3)%>%
  layer_dense(units = 30, activation = 'relu') %>%
  layer_dropout(0.2)%>%
  #layer_dense(units = 70, activation = 'relu') %>%
  layer_dense(units = length(levels(data.test.pca$Y)), activation = 'softmax')

model %>% compile(
  loss = "categorical_crossentropy",
  optimizer = optimizer_rmsprop(),
  metrics = "accuracy"
)

train.history <- model %>% fit(data.train.pca.X,
                               data.train.pca.y,
                               batch_size = 50,
                               epochs = 1000,
                               validation_split = 0.2)

nnet.scores <- model %>% evaluate(
  data.test.pca.X, data.test.pca.y, verbose = 0
)