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

svm.gaussian.C <- CC[which.min(svm.gaussian)]
