library(rjulia)

julia_init()
## uint 8
y <- j2r("x=convert(Uint8,1)")
y
## uint 16
y <- j2r("x=convert(Uint16,1)")
y
## uint 32
y <- j2r("x=convert(Uint32,1)")
y
## uint 64
y <- j2r("x=convert(Uint64,1)")
y
## int 8
y <- j2r("x=convert(Int8,1)")
y
## int 16
y <- j2r("x=convert(Int16,1)")
y
## int 32
y <- j2r("x=convert(Int32,1)")
y
## int 64
y <- j2r("x=convert(Int64,1)")
y
## float32
y <- j2r("x=convert(Float32,1.01)")
y
## float64
y <- j2r("x=convert(Float64,1.01)")
y

## vector
## uint 8
x <- 1:10
r2j(x,"tt")
y <- j2r("x1=convert(Uint8,tt[1])")
y <- j2r("x2=convert(Uint8,tt[2])")
y <- j2r("x=[x1,x2]")
y
## uint16
y <- j2r("x1=convert(Uint16,tt[1])")
y <- j2r("x2=convert(Uint16,tt[2])")
y <- j2r("x=[x1,x2]")
y
## uint 32
y <- j2r("x1=convert(Uint32,tt[1])")
y <- j2r("x2=convert(Uint32,tt[2])")
y <- j2r("x=[x1,x2]")
y
## uint 64
y <- j2r("x1=convert(Uint64,tt[1])")
y <- j2r("x2=convert(Uint64,tt[2])")
y <- j2r("x=[x1,x2]")
y
## int 8
y <- j2r("x1=convert(Int8,tt[1])")
y <- j2r("x2=convert(Int8,tt[2])")
y <- j2r("x=[x1,x2]")
y
## int 16
y <- j2r("x1=convert(Int16,tt[1])")
y <- j2r("x2=convert(Int16,tt[2])")
y <- j2r("x=[x1,x2]")
y
## int 32
y <- j2r("x1=convert(Int32,tt[1])")
y <- j2r("x2=convert(Int32,tt[2])")
y <- j2r("x=[x1,x2]")
y
## int 64
y <- j2r("x1=convert(Int64,tt[1])")
y <- j2r("x2=convert(Int64,tt[2])")
y <- j2r("x=[x1,x2]")
y

x <- 1.1:10.1
## float32
r2j(x,"tt")
y <- j2r("x1=convert(Float32,tt[1])")
y <- j2r("x2=convert(Float32,tt[2])")
y <- j2r("x=[x1,x2]")
y
## float64
y <- j2r("x1=convert(Float64,tt[1])")
y <- j2r("x2=convert(Float64,tt[2])")
y <- j2r("x=[x1,x2]")
y
