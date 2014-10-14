library(rjulia)
#init embedding julia,paraments are julia_home and disable_gc
if(.Platform$OS.type == "unix") julia_init("/usr/bin",F,T) else 
{
  if (.Platform$r_arch=="x64")   
   {julia_init("c:/julia/bin",F,T)}
  else 
   {julia_init("c:/julia32/bin",F,T)}  
 }


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

