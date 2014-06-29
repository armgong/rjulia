library(rjulia)
#init embedding julia,paraments are julia_home and disable_gc
julia_init("d:/codes/julia32/usr/bin",F)
julia_void_eval("using DataArrays,DataFrames")



x<-1:10
x[2]<-NA
r2j(x,"ttt")
y<-j2r("ttt[10]")
y
y<-j2r("ttt[2]")
y
y<-j2r("ttt")
y



x<-c(T,F,T,T,F,F,F)
x[2]<-NA
r2j(x,"ttt")
y<-j2r("ttt[3]")
y
y<-j2r("ttt[2]")
y
y<-j2r("ttt")
y



x<-1.1:10.1
x[2]<-NA
r2j(x,"ttt")
y<-j2r("ttt[10]")
y
y<-j2r("ttt[2]")
y
y<-j2r("ttt")
y

x<-c("x","y","z","u","v","w","a")
x[2]<-NA
r2j(x,"ttt")
y<-j2r("ttt[3]")
y
y<-j2r("ttt[2]")
y
y<-j2r("ttt")
y




