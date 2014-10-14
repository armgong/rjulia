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
julia_eval("addprocs(3)")
for (i in 1:4)
{
 julia_void_eval(paste("r=remotecall(",i,", rand, 2, 2)",sep=""))
 y<-j2r(" fetch(r)")
 cat("\n")
 cat(paste("process ",i," get value:\n",sep=""))

 print(y)
}

