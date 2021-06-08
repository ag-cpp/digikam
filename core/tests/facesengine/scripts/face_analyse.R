data <- read.table("./face_data.txt",sep = ",")

# Left out  a label as an unknown label on learning data
leftout <- data[data$V1 == 27,]
data <-  data[data$V1 != 27,]

data$V1 <- as.factor(data$V1)

# Split train/test/validation
set.seed(1729)
n<-nrow(data)
data[, -1] <- scale(data[, -1])
train<-sample(1:n, round(n/2))
data.train<-data[train,]
data.validation<-data[-train,]

m<-nrow(data.train)
test<-sample(1:m, round(m/3))
data.test<-data.train[test,]
data.train<-data.train[-test,]


library(tidyr)
library(corrplot)
library(ggplot2)
library(ggthemes)

#corrplot(cor(data[,-1]), method="number", type = "lower", number.cex = .6)
# PCA on train set
train.pca <- prcomp(data.train[, -1])
train.lambda <- train.pca$sdev^2
pairs(train.pca$x[,1:5], col=data.train$V1, pch=as.numeric(data.train$V1))
plot(cumsum(train.lambda)/sum(train.lambda), type="l", xlab="q", ylab="proportion of explained variance")

data.train.pca <- as.data.frame(scale(train.pca$x[,1:27]))
data.train.pca$Y <- data.train$V1
data.train.pca[, -ncol(data.train.pca)] <- scale(data.train.pca[, -ncol(data.train.pca)])

# PCA on test set
test.pca <- predict(train.pca, newdata=data.test[, -1])
data.test.pca <- as.data.frame(scale(test.pca[,1:27]))
data.test.pca$Y <- data.test$V1
data.test.pca[, -ncol(data.test.pca)] <- scale(data.test.pca[, -ncol(data.test.pca)])

# PCA on validation set
validation.pca <- predict(train.pca, data.validation[, -1])

data.validation.pca <- as.data.frame(scale(validation.pca[,1:27]))
data.validation.pca$Y <- data.validation$V1
data.validation.pca[, -ncol(data.validation.pca)] <- scale(data.validation.pca[, -ncol(data.validation.pca)])

# LeftoutPCA
leftout.pca <- predict(train.pca, leftout[, -1])
leftout.pca  <- as.data.frame(scale(leftout.pca [,1:27]))
leftout.pca$Y <- 27
leftout.pca[, -ncol(leftout.pca)] <- scale(leftout.pca[, -ncol(leftout.pca)])

