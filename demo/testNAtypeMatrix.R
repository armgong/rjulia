library(rjulia)
#init embedding julia,paraments are julia_home and disable_gc
julia_init("d:/codes/julia32/usr/bin")
julia_LoadDataArrayFrame()

x<-matrix(1:9,c(3,3))
x[1]=NA
x[5]<-NA
x[8]<-NA
x
r2j(x,"xy")
y<-j2r("xy")
y
y<-j2r("length(xy)")
y
y<-j2r("length(xy.na)")
y
y<-j2r("length(xy.data)")
y

