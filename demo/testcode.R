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

f=function(n){
 for (i in 1:n)
 {   
  #pass R double vector to Julia
  x<-1.01:5.01
  r2j(x,"tt")
  #get passed vector from Julia
  y<-j2r("tt")
  cat("float vector:",y,"\n")
 

 #pass R int vector to Julia
  x<-1:5
  r2j(x,"tt")
  #get passed vector from Julia
  y<-j2r("tt")
  cat("int vector:",y,"\n")
  
 #pass R int vector to Julia
  x<-c(TRUE,FALSE,TRUE)
  r2j(x,"ttt")
  yy<-j2r("ttt")
  cat("bool vector:",yy,"\n")

  
  x<-1
  r2j(x,"ss")
  yy<-j2r("ss")
  cat("int :",yy,"\n")

  
  
  x<-1.1
  r2j(x,"ss")
  yy<-j2r("ss")
  cat("float :",yy,"\n")

  
  x<-TRUE
  r2j(x,"ss")
  yy<-j2r("ss")
  cat("bool :",yy,"\n")

 
  # pass string vector to julia,need to verify 
  x<-c("tttt","xxxx")
  r2j(x,"sss")
  yy<-j2r("sss")
  cat("string vector :",yy,"\n")
  yy<-j2r("sss[1]")
  cat("string :",yy,"\n")
  
  # pass string vector to julia,need to verify
  julia_void_eval('xx=["ttt","xxxx"]')
  y<-j2r("xx")
  cat("string vector:",y,"\n")
  y<-j2r("xx[1]")
  cat("string:",y,"\n")
  cat("run time is:",i,"\n")
 }  
}
f(1)
f(10)
xdd<-f(10000)
