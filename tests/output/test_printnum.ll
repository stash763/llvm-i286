; ModuleID = 'test_printnum.c'
source_filename = "test_printnum.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-pc-linux-gnu"

@.buf = private unnamed_addr constant [16 x i8] c"                \00", align 1

declare void @printnum(ptr noundef, i32 noundef, i32 noundef)

define dso_local i32 @main() #0 {
  call void @printnum(ptr noundef @.buf, i32 noundef 16, i32 noundef -999)
  ret i32 0
}

attributes #0 = { noinline nounwind optnone uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }