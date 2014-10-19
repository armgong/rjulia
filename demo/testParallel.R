library(rjulia)
#init embedding julia,paraments are julia_home and disable_gc
if(.Platform$OS.type == "unix") julia_init("/usr/bin",F,T) else 
{
  if (.Platform$r_arch=="x64")   
   {julia_init("c:/julia/bin",F,T)}
  else 
   {julia_init("c:/julia32/bin",F,T)}  
 }
#warning don't add too much procs in test
#otherwise it will crash on low end machine
julia_eval("addprocs(1)")
for (i in 1:2)
{
 julia_void_eval(paste("r=remotecall(",i,", rand, 2, 2)",sep=""))
 y<-j2r(" fetch(r)")
 cat("\n")
 cat(paste("process ",i," get value:\n",sep=""))
 print(y)
}
julia_void_eval("rmprocs(workers())")
