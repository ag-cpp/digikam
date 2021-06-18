# Cross validation

# Méthode 1: KNN
library("class")
library("caret")
library("e1071")

# Réglage de K par validation croisée
trControl <- trainControl(method  = "cv",
                          number  = 5)
tuneGrid <- data.frame(k=c(1, 2, 3, 5, 10, 20, 30, 40, 50))
knn.fit <- train(Y~.,
                 method     = "knn",
                 tuneGrid   = tuneGrid,
                 trControl  = trControl,
                 metric     = "Accuracy",
                 data       = data.train.pca)

plot(knn.fit$results$k, knn.fit$result$Accuracy, type="b", log="x", xlab="K", ylab="CV Accuracy")

knn.pred <- knn(data.train.pca[, -ncol(data.train.pca)], data.test.pca[, -ncol(data.test.pca)], data.train.pca$Y, k=knn.fit$bestTune$k)
table(data.test.pca$Y, knn.pred)
knn.err <- mean(data.test.pca$Y != knn.pred)