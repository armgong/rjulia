library(rjulia)
#init embedding julia,paraments are julia_home and disable_gc
julia_init("/usr/bin")
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
f=function(n){
 for (i in 1:n)
 {   
  #pass R double vector to Julia
  x<-matrix(1.01:6.01,3,2)
  r2j(x,"tt")
  #get passed vector from Julia
  y<-j2r("tt")
  cat("Matrix:","\n")
  print(y)
  cat("run time is:",i,"\n")
 }
 #create 2d array in julia,get from R
 julia_void_eval("x = rand(2,2)")
 yy<-j2r("x")
 cat("Matrix:","\n")
 print(yy)
}
f(1)
f(10)
xdd<-f(10000)
f=function(n){
 for (i in 1:n)
 {   
  #pass R double vector to Julia
  x<-array(1.01:18.01,c(3,3,2))
  r2j(x,"tt")
  y<-j2r("tt")
  cat("MD array:","\n")
  print(y)
  cat("run times is:",i,"\n")
 }
}
f(1)
f(10)
xdd<-f(10000)


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
#uint 8
y<-j2r("x=convert(Uint8,1)")
y
#uint 16
y<-j2r("x=convert(Uint16,1)")
y
#uint 32
y<-j2r("x=convert(Uint32,1)")
y
#uint 64
y<-j2r("x=convert(Uint64,1)")
y
#int 8
y<-j2r("x=convert(Int8,1)")
y
#int 16
y<-j2r("x=convert(Int16,1)")
y
#int 32
y<-j2r("x=convert(Int32,1)")
y
#int 64
y<-j2r("x=convert(Int64,1)")
y
#float32
y<-j2r("x=convert(Float32,1.01)")
y
#float64
y<-j2r("x=convert(Float64,1.01)")
y

#vector
#uint 8
x<-1:10
r2j(x,"tt")
y<-j2r("x1=convert(Uint8,tt[1])")
y<-j2r("x2=convert(Uint8,tt[2])")
y<-j2r("x=[x1,x2]")
y
#uint16
y<-j2r("x1=convert(Uint16,tt[1])")
y<-j2r("x2=convert(Uint16,tt[2])")
y<-j2r("x=[x1,x2]")
y
#uint 32
y<-j2r("x1=convert(Uint32,tt[1])")
y<-j2r("x2=convert(Uint32,tt[2])")
y<-j2r("x=[x1,x2]")
y
#uint 64
y<-j2r("x1=convert(Uint64,tt[1])")
y<-j2r("x2=convert(Uint64,tt[2])")
y<-j2r("x=[x1,x2]")
y
#int 8
y<-j2r("x1=convert(Int8,tt[1])")
y<-j2r("x2=convert(Int8,tt[2])")
y<-j2r("x=[x1,x2]")
y
#int 16
y<-j2r("x1=convert(Int16,tt[1])")
y<-j2r("x2=convert(Int16,tt[2])")
y<-j2r("x=[x1,x2]")
y
#int 32
y<-j2r("x1=convert(Int32,tt[1])")
y<-j2r("x2=convert(Int32,tt[2])")
y<-j2r("x=[x1,x2]")
y
#int 64
y<-j2r("x1=convert(Int64,tt[1])")
y<-j2r("x2=convert(Int64,tt[2])")
y<-j2r("x=[x1,x2]")
y

x<-1.1:10.1
#float32
r2j(x,"tt")
y<-j2r("x1=convert(Float32,tt[1])")
y<-j2r("x2=convert(Float32,tt[2])")
y<-j2r("x=[x1,x2]")
y
#float64
y<-j2r("x1=convert(Float64,tt[1])")
y<-j2r("x2=convert(Float64,tt[2])")
y<-j2r("x=[x1,x2]")
y

y<-j2r('df = DataFrame(A = 1:4, B = ["M", "F", "F", "M"])')
typeof(y)
is.data.frame(y)
y
#because julia data frame don't allow column name like R,so need change iris column name
names(iris)<-c("sl","sw","pl","wl","speics")
r2j(iris,"xx")
y<-j2r("xx")
y
#todo factor and pooldataarray

y<-j2r('pdv = @pdata(["Group A", "Group A", "Group A","Group B", "Group B", "Group B"])')
y
y<-j2r('levels(pdv)')
y
y<-j2r('df = DataFrame(A = [1, 1, 1, 2, 2, 2],B = ["X", "X", "X", "Y", "Y", "Y"])')
y
y<-j2r('pool!(df, [:A, :B])')
y<-j2r('df')
y

julia_eval("addprocs(3)")
for (i in 1:4)
{
 julia_void_eval(paste("r=remotecall(",i,", rand, 2, 2)",sep=""))
 y<-j2r(" fetch(r)")
 cat("\n")
 cat(paste("process ",i," get value:\n",sep=""))

 print(y)
}


