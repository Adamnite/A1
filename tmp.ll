; ModuleID = 'Module'
source_filename = "Module"
target datalayout = "e-m:o-i64:64-i128:128-n32:64-S128"
target triple = "arm64-apple-darwin21.4.0"

@0 = private unnamed_addr constant [14 x i8] c"Hello, world!\00", align 1

declare i32 @printf(i8*, ...)

define void @main() {
entry:
  ret void
}

define i8* @get() {
entry:
  ret i8* getelementptr inbounds ([14 x i8], [14 x i8]* @0, i32 0, i32 0)
}
