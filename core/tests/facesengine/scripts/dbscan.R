install.packages("fpc")
install.packages("dbscan")

library(fpc)
data.train.pca.X <- subset(data.train.pca, select = -Y)
db <- fpc::dbscan(data.train.pca.X, eps = 1, MinPts = 2, scale = TRUE)
