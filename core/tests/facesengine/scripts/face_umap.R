library(umap)

data <- read.table("./face_data.txt",sep = ",")

data.umap = umap(data)

plot(data.umap$layout)
