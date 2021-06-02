plot.umap <- function(x, labels,
                      main="A UMAP visualization",
                      colors=c("#ff7f00", "#e377c2", "#17becf"),
                      pad=0.1, cex=0.6, pch=19, add=FALSE, legend.suffix="",
                      cex.main=1, cex.legend=0.85) {
  layout = x
  if (is(x, "umap")) {
    layout = x$layout
  } 

  xylim = range(layout)
  xylim = xylim + ((xylim[2]-xylim[1])*pad)*c(-0.5, 0.5)
  if (!add) {
    par(mar=c(0.2,0.7,1.2,0.7), ps=10)
    plot(xylim, xylim, type="n", axes=F, frame=F)
    rect(xylim[1], xylim[1], xylim[2], xylim[2], border="#aaaaaa", lwd=0.25)  
  }
  
  points(layout[,1], layout[,2], col=colors[as.integer(labels)], cex=cex, pch=pch)
  mtext(side=3, main, cex=cex.main)

  labels.u = unique(labels)
  legend.pos = "topleft"
  legend.text = as.character(labels.u)
  
  if (add) {
    legend.pos = "bottomleft"
    legend.text = paste(as.character(labels.u), legend.suffix)
  }
  
  legend(legend.pos, legend=legend.text, inset=0.03, col=colors[as.integer(labels.u)], bty="n", pch=pch, cex=cex.legend)
}


library(umap)

data <- read.table("./face_data.txt",sep = ",")

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



data.train.X <- data.train[, -1]
data.train.Y <- data.train$V1

data.train.X.umap = umap(data.train.X)

plot(data.train.X.umap$layout)

plot.umap(data.train.X.umap, data.train.Y)

