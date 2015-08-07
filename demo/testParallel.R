library(rjulia)

julia_init()
## warning due to https://github.com/JuliaLang/julia/issues/10085, the rjulia master branch may crash when calling julia parallel functions on Julia 0.3.x.
## warning don't add too much procs in test
## otherwise it will crash on low end machine
julia_eval("addprocs(3)")

#julia 0.4 move distributed array from base library, so DistributedArrays need to be installed and loaded
julia_void_eval("@everywhere using DistributedArrays")

#demo rand on remote
julia_void_eval(paste("r=remotecall(",2,", rand, 2, 2)",sep = ""))
y <- j2r("fetch(r)")
cat("\n")
cat("process", 2, "got value:\n"); print(y)

#simple distribute 
julia_void_eval('x=drand((100,100));')
julia_void_eval('println(x.chunks); println(x.indexes)')
julia_void_eval('println(typeof(x))')
julia_void_eval('y=convert(Array,x);')
y1<-j2r("y")
y1[1:4]

#demo DArray 
julia_void_eval('x=drand((100,100));')
julia_void_eval('println(x.chunks); println(x.indexes)')
julia_void_eval('println(typeof(x))')
julia_void_eval('y=convert(Array,x);')
y1<-j2r("y")
y1[1:4]

#demo PMAP
julia_void_eval('x=[rand(100,100) for i=1:4];')
julia_void_eval('z=pmap(sum, x)')
julia_void_eval('println(typeof(z))')
julia_void_eval('y=convert(Array{Float64},z);')
y2<-j2r('y')
y2

#close workers
julia_void_eval("rmprocs(workers())")
