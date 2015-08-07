library(rjulia)

julia_init()
## ## init embedding julia,paraments are julia_home and disable_gc
## if(.Platform$OS.type == "unix") julia_init("/usr/bin",F,T) else
## {
##   if (.Platform$r_arch=="x64")
##   {julia_init("c:/julia/64/bin",F,T)}
##   else
##   {julia_init("c:/julia/32/bin",F,T)}
## }

## load Julia DataFrames and DataArrays -- with care
T <- tryCatch(julia_void_eval("using DataArrays,DataFrames"),
                          error = function(e) e)
## MM [2015-02-04, lynne]:
## LoadError("/u/maechler/.julia/v0.3/DataArrays/src/DataArrays.jl",59,LoadError("/u/maechler/.julia/v0.3/DataArrays/src/abstractdataarray.jl",5,UndefVarError(:StoredArray)))
okDF <- !inherits(T, "error")


f <- function(n)
{
    stopifnot(n >= 1)
    for (i in 1:n) {
        ## pass R double vector to Julia
        x <- 1.01:5.01
        r2j(x,"tt")
        ## get passed vector from Julia
        y <- j2r("tt")
        cat("float vector:",y,"\n")


        ## pass R int vector to Julia
        x <- 1:5
        r2j(x,"tt")
        ## get passed vector from Julia
        y <- j2r("tt")
        cat("int vector:",y,"\n")

        ## pass R int vector to Julia
        x <- c(TRUE,FALSE,TRUE)
        r2j(x,"ttt")
        yy <- j2r("ttt")
        cat("bool vector:",yy,"\n")

        x <- 1
        r2j(x,"ss")
        yy <- j2r("ss")
        cat("int :",yy,"\n")

        x <- 1.1
        r2j(x,"ss")
        yy <- j2r("ss")
        cat("float :",yy,"\n")

        x <- TRUE
        r2j(x,"ss")
        yy <- j2r("ss")
        cat("bool :",yy,"\n")

        ## pass string vector to julia,need to verify
        x <- c("tttt","xxxx")
        r2j(x,"sss")
        yy <- j2r("sss")
        cat("string vector :",yy,"\n")
        yy <- j2r("sss[1]")
        cat("string :",yy,"\n")

        ## pass string vector to julia,need to verify
        julia_void_eval('xx=["ttt","xxxx"]')
        y <- j2r("xx")
        cat("string vector:",y,"\n")
        y <- j2r("xx[1]")
        cat("string:",y,"\n")
        cat("run time is:",i,"\n")
    }
}
f(1)
f(10)
xdd <- f(10000)

f2 <- function(n)
{
  stopifnot(n >= 1)
  for (i in 1:n) {
    x <- matrix(1.01:6.01, 3,2)
    st <- system.time({
        ## pass R matrix to Julia
        r2j(x,"tt")
        ## and get it passed back from Julia
        y <- j2r("tt")
    })
    cat("Matrix passed to julia and back: ")
    stopifnot(identical(y, x))
    cat(sprintf("[Ok].  Elapsed system.time(): %g\n", st[["elapsed"]]))
  }
  ##
  ## create 2d array in julia,get from R
  julia_void_eval("x = rand(2,2)")
  yy <- j2r("x")
  cat("rand(2,2) matrix:\n")
  print(yy)
} ## end{f2}

f2(1)
f2(10)
## !!!! If I interrupt the following,  R is taken down !!! BUG !!!
xdd <- f2(10000)

f3 <- function(n) {
  stopifnot(n >= 1)
  for (i in 1:n)
  {
    ## pass R double vector to Julia
    x <- array(1.01:18.01,c(3,3,2))
    r2j(x,"tt")
    y <- j2r("tt")
    cat("MD array:","\n")
    print(y)
    cat("run times is:",i,"\n")
  }
}
f3(1)
f3(10)
xdd <- f3(10000)
cat("clear R Object begin\n")
rm(list = ls())
cat("clear R Object Finish\n")

for (i in 1:1)
{
  x <- 1:10
  x[2] <- NA
  r2j(x,"ttt")
  y <- j2r("ttt[10]")

  y <- j2r("ttt[2]")

  y <- j2r("ttt")

  x <- c(TRUE,FALSE,TRUE,TRUE,FALSE,FALSE,FALSE)
  x[2] <- NA
  r2j(x,"ttt")
  y <- j2r("ttt[3]")

  y <- j2r("ttt[2]")

  y <- j2r("ttt")



  x <- 1.1:10.1
  x[2] <- NA
  r2j(x,"ttt")
  y <- j2r("ttt[10]")
  y <- j2r("ttt[2]")
  y <- j2r("ttt")


  x <- c("x", NA, "z","u","v","w","a")
  r2j(x,"ttt")
  y <- j2r("ttt[3]")

  y <- j2r("ttt[2]")

  y <- j2r("ttt")

  x <- matrix(1:9, 3,3)
  x[c(1,5,8)] <- NA
  x
  r2j(x,"xy")
  y <- j2r("xy")

  y <- j2r("length(xy)")

  y <- j2r("length(xy.na)")

  y <- j2r("length(xy.data)")


  x <- 1:3
  x1 <- c("hello","world")
  y <- matrix(1:12, 3,4)
  z <- list(x,x1,y)
  r2j(z,"tupletest")
  y <- j2r("tupletest")


  zz <- list(x,x1,y,z)
  r2j(zz,"tupletest")
  y <- j2r("tupletest")

  ## uint 8
  y <- j2r("x=convert(Uint8,1)")

  ## uint 16
  y <- j2r("x=convert(Uint16,1)")

  ## uint 32
  y <- j2r("x=convert(Uint32,1)")

  ## uint 64
  y <- j2r("x=convert(Uint64,1)")

  ## int 8
  y <- j2r("x=convert(Int8,1)")

  ## int 16
  y <- j2r("x=convert(Int16,1)")

  ## int 32
  y <- j2r("x=convert(Int32,1)")

  ## int 64
  y <- j2r("x=convert(Int64,1)")

  ## float32
  y <- j2r("x=convert(Float32,1.01)")

  ## float64
  y <- j2r("x=convert(Float64,1.01)")


  ## vector
  ## uint 8
  x <- 1:10
  r2j(x,"tt")
  y <- j2r("x1=convert(Uint8,tt[1])")
  y <- j2r("x2=convert(Uint8,tt[2])")
  y <- j2r("x=[x1,x2]")
  ## uint16
  y <- j2r("x1=convert(Uint16,tt[1])")
  y <- j2r("x2=convert(Uint16,tt[2])")
  y <- j2r("x=[x1,x2]")

  ## uint 32
  y <- j2r("x1=convert(Uint32,tt[1])")
  y <- j2r("x2=convert(Uint32,tt[2])")
  y <- j2r("x=[x1,x2]")

  ## uint 64
  y <- j2r("x1=convert(Uint64,tt[1])")
  y <- j2r("x2=convert(Uint64,tt[2])")
  y <- j2r("x=[x1,x2]")
  ## int 8
  y <- j2r("x1=convert(Int8,tt[1])")
  y <- j2r("x2=convert(Int8,tt[2])")
  y <- j2r("x=[x1,x2]")

  ## int 16
  y <- j2r("x1=convert(Int16,tt[1])")
  y <- j2r("x2=convert(Int16,tt[2])")
  y <- j2r("x=[x1,x2]")

  ## int 32
  y <- j2r("x1=convert(Int32,tt[1])")
  y <- j2r("x2=convert(Int32,tt[2])")
  y <- j2r("x=[x1,x2]")

  ## int 64
  y <- j2r("x1=convert(Int64,tt[1])")
  y <- j2r("x2=convert(Int64,tt[2])")
  y <- j2r("x=[x1,x2]")


  x <- 1.1:10.1
  ## float32
  r2j(x,"tt")
  y <- j2r("x1=convert(Float32,tt[1])")
  y <- j2r("x2=convert(Float32,tt[2])")
  y <- j2r("x=[x1,x2]")

  ## float64
  y <- j2r("x1=convert(Float64,tt[1])")
  y <- j2r("x2=convert(Float64,tt[2])")
  y <- j2r("x=[x1,x2]")

  y <- j2r('df = DataFrame(A = 1:4, B = ["M", "F", "F", "M"])')
  typeof(y)
  is.data.frame(y)
  ## todo factor and pooldataarray

  y <- j2r('pdv = @pdata(["Group A", "Group A", "Group A","Group B", "Group B", "Group B"])')
  y <- j2r('levels(pdv)')
  y <- j2r('df = DataFrame(A = [1, 1, 1, 2, 2, 2],B = ["X", "X", "X", "Y", "Y", "Y"])')
  y <- j2r('pool!(df, [:A, :B])')
  y <- j2r('df')
  julia_void_eval("df=0;")
  names(iris) <- c("sl","sw","pl","wl","speics")
  r2j(iris,"xx")
  y <- j2r("xx")
  julia_void_eval("xx=0;")

  cat("run time is:",i,"\n")

  cat("clear R Object begin\n")
  rm(list = ls())
  cat("clear R Object Finish\n")
}

## warning don't add too much procs in test
## otherwise it will crash on low end machine
## warning due to https://github.com/JuliaLang/julia/issues/10085, the rjulia master branch may crash when calling julia parallel functions on Julia 0.3.x. 

julia_void_eval("addprocs(1)")

julia_void_eval(paste("r=remotecall(",2,", rand, 2, 2)",sep = ""))
y <- j2r(" fetch(r)")
cat("\n")
cat(paste("process ",2," get value:\n",sep = ""))
print(y)

julia_void_eval("rmprocs(workers())")

