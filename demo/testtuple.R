library(rjulia)
#init embedding julia,paraments are julia_home and disable_gc
if(.Platform$OS.type == "unix") {
 julia_init("/usr/bin",F,T)  
}
else
{  
 if (.Platform$r_arch=="x64")   
  julia_init("c:/julia/bin",F,T)
 else
   julia_init("c:/julia32/bin",F,T)  
}
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
