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

# Method 1: SVM
# 1.1: SVM linear kernel
library("foreach")
library("doParallel")
library("kernlab")

# Choose hyper-param C by cross-validation
CC <- c(0.001,0.01,0.1,1,10,100,1000)
N <- length(CC)
M <- 10 # number of repetition of cross-validation

numCores <- detectCores() - 1
registerDoParallel(numCores)

svm.linear <- foreach (i=1:N, .combine=c) %do% {
  error <- foreach (k=1:M, .combine=c) %dopar% {
    error <- cross(ksvm(Y~., data=data.train.pca, type="C-svc", kernel="vanilladot", C=CC[i], cross=5))
  }
  
  res <- mean(error)
}

plot(CC, svm.linear, type="b", log="x", xlab="C", ylab="CV error")

#svm.linear.C <- CC[which.min(svm.linear)]
svm.linear.C <- 0.1

# Test error with SVM linear kernel
svm.linear.fit <- ksvm(Y~., data=data.train.pca, type="C-svc", kernel="vanilladot", C=svm.linear.C)
svm.linear.pred <- predict(svm.linear.fit, newdata=data.test.pca)
table(data.test.pca$Y, svm.linear.pred)
svm.linear.err <- mean(data.test.pca$Y != svm.linear.pred)


#1.2 SVM with gaussian kernel
# Choose hyper-param C by cross-validation
svm.gaussian <- foreach (i=1:N, .combine=c) %do% {
  error <- foreach (k=1:M, .combine=c) %dopar% {
    error <- cross(ksvm(Y~., data=data.train.pca, type="C-svc", kernel="rbfdot", kpar="automatic", C=CC[i], cross=5))
  }
  
  res <- mean(error)
}

plot(CC, svm.gaussian, type="b", log="x", xlab="C", ylab="CV error")

#svm.gaussian.C <- CC[which.min(svm.gaussian)]
svm.gaussian.C <- 10

# Test error with SVM gaussian kernel
svm.gaussian.fit <- ksvm(Y~., data=data.train.pca, type="C-svc",kernel="rbfdot", kpar="automatic", C=svm.gaussian.C)
svm.gaussian.pred <- predict(svm.gaussian.fit, newdata=data.test.pca)
table(data.test.pca$Y, svm.gaussian.pred)
svm.gaussian.error <- mean(data.test.pca$Y != svm.gaussian.pred)

# 1.3 One-class SVM for novelty detection
svm.onclass.fit <-ksvm(~.-Y, data=data.train.pca, type="one-svc", kernel="rbfdot", nu=0.1,kpar=list(sigma=0.3), cross=10)
svm.oneclass.pred <- predict(svm.onclass.fit, newdata=outlier.test)
table(outlier.test$Y==27, svm.oneclass.pred)
svm.oneclass.error <- mean((outlier.test$Y==27) != svm.oneclass.pred)
