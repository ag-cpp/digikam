# Method 3: Decision Tree

library(rpart)

# 3.1 Full tree
tree.full.fit<-rpart(Y~., data=data.train.pca, method="class", control=rpart.control(xval=10, minbucket=10, cp=0.00))

# Réprésentation graphique de l'arbre complet
plot(tree.full.fit, margin=0.005)
text(tree.full.fit, minlength=0.5, cex=0.2, splits=TRUE)

library(rpart.plot)
rpart.plot(tree.full.fit, box.palette="RdBu", shadow.col="gray", fallen.leaves=FALSE)

# test error
tree.full.pred <-predict(tree.full.fit, newdata=data.test.pca, type='class')
table(data.test.pca$Y, tree.full.pred)
tree.full.error <- mean(data.test.pca$Y != tree.full.pred)

# 3.2 Pruned tree
printcp(tree.full.fit)
plotcp(tree.full.fit, minline=TRUE)
i.min<-which.min(tree.full.fit$cptable[,4])
cp.opt1<-tree.full.fit$cptable[i.min,1]

tree.pruned.fit <-prune(tree.full.fit, cp=cp.opt1)
rpart.plot(tree.pruned.fit, box.palette="RdBu", shadow.col="gray")

# test error
tree.pruned.pred <-predict(tree.pruned.fit, newdata=data.test.pca, type='class')
table(data.test.pca$Y, tree.pruned.pred)
tree.pruned.error <- mean(data.test.pca$Y != tree.pruned.pred)

# 3.3 Bagging
library(randomForest)

p <-ncol(data.train.pca) - 1
bagging.fit <-randomForest(Y~., data=data.train.pca, mtry=p)

bagging.pred <-predict(bagging.fit, newdata=data.test.pca, type="response")
table(data.test.pca$Y, bagging.pred)
bagging.error <- mean(data.test.pca$Y != bagging.pred)

# 3.4 Random forest
randomforest.fit <-randomForest(Y~., data=data.train.pca)
randomforest.pred <- predict(randomforest.fit, newdata=data.test.pca, type="response")
table(data.test.pca$Y, randomforest.pred)
randomforest.error <- mean(data.test.pca$Y != randomforest.pred)

