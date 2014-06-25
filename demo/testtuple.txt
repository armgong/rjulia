library(rjulia)
#init embedding julia,paraments are julia_home and disable_gc
julia_init("d:/codes/julia32/usr/bin",F)
x<-1:3
x1<-c("hello","world")
y<-matrix(1:12,c(3,4))
z<-list(x,x1,y)
r2j(z,"tupletest")
y<-j2r("tupletest")
y

zz<-list(x,x1,y,z)
r2j(zz,"tupletest")
y<-j2r("tupletest")
y