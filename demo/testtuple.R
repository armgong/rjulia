library(rjulia)

julia_init()

x <- 1:3
x1 <- c("hello","world")
y <- matrix(1:12,c(3,4))
z <- list(x,x1,y)

r2j(z,"tupletest")
y <- j2r("tupletest")
y
stopifnot(identical(y, z)

zz <- list(x,x1,y,z)
r2j(zz,"tupletst2")
yy <- j2r("tupletest")
stopifnot(identical(yy, zz)

