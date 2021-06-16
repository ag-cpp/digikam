# Method 2: Generative models

# 2.1 LDA

library(MASS)
lda.fit <- lda(Y~., data=data.train.pca)
lda.pred <- predict(lda.fit, newdata=data.test.pca)
lda.class <- lda.pred$class
table(data.test.pca$Y, lda.class)
lda.error <- mean(data.test.pca$Y != lda.class)

colnames(lda.pred$posterior)[max.col(lda.pred$posterior,ties.method="first")]
apply(lda.pred$posterior[, 2:28], 1, max)

#Outlier detection with RDA
lda.outlier.pred <- predict(lda.fit, newdata=outlier.test)

colnames(lda.outlier.pred$posterior)[max.col(lda.outlier.pred$posterior,ties.method="first")]
table(outlier.test$Y==27, apply(lda.outlier.pred$posterior, 1, max) < 0.5)
lda.outlier.pred <- mean((outlier.test$Y==27) != (apply(lda.outlier.pred$posterior, 1, max) < 0.5))

# 2.2 QDA
qda.fit <- qda(Y~., data=data.train.pca)
qda.pred <- predict(qda.fit, newdata=data.test.pca)$class
table(data.test.pca$Y, qda.pred)
qda.error <- mean(data.test.pca$Y != qda.pred)

# 2.3 RDA
# cross validation 
library(caret)
library(klaR)
trControl <- trainControl(method  = "cv", number  = 5, search = "random")

rda.cv <- train(Y~.,
                method     = "rda",
                tuneLength = 20,
                trControl  = trControl,
                metric     = "Accuracy",
                data       = data.train.pca)

rda.fit <- rda(Y~., data=data.train.pca,
                    lambda=rda.cv$bestTune$lambda, gamma=rda.cv$bestTune$gamma)

rda.pred <- predict(rda.fit, newdata=data.test.pca)
rda.class <- rda.pred$class
table(data.test.pca$Y, rda.class)
rda.error <- mean(data.test.pca$Y != rda.class)

#Outlier detection with RDA
rda.outlier.pred <- predict(rda.fit, newdata=outlier.test)

colnames(rda.outlier.pred$posterior)[max.col(rda.outlier.pred$posterior,ties.method="first")]
table(outlier.test$Y==27, apply(rda.outlier.pred$posterior, 1, max) < 0.5)
rda.outlier.error <- mean((outlier.test$Y==27) != (apply(rda.outlier.pred$posterior, 1, max) < 0.5))


