library(rjulia)

julia_init()

jloaddf()

y <- j2r('df = DataFrame(A = 1:4, B = ["M", "F", "F", "M"])')
typeof(y)
is.data.frame(y)
y
## because julia data frame don't allow column name like R,so need change iris column name
names(iris) <- c("sl","sw","pl","wl","speics")
r2j(iris,"xx")
y <- j2r("xx")
y
## todo factor and pooldataarray

y <- j2r('pdv = @pdata(["Group A", "Group A", "Group A","Group B", "Group B", "Group B"])')
y
y <- j2r('levels(pdv)')
y
y <- j2r('df = DataFrame(A = [1, 1, 1, 2, 2, 2],B = ["X", "X", "X", "Y", "Y", "Y"])')
y
y <- j2r('pool!(df, [:A, :B])')
y <- j2r('df')
y

## xx=DataFrame(Ssl =xxdfelt1,sw =xxdfelt2,pl =xxdfelt3,wl =xxdfelt4,NA =xxdfelt5)
