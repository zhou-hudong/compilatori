; ModuleID = 'loop.optimized.ll'
source_filename = "loop.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @prova1() #0 {
  %1 = add nsw i32 1, 1
  br label %2

2:                                                ; preds = %13, %0
  %.01 = phi i32 [ 0, %0 ], [ %14, %13 ]
  %3 = icmp slt i32 %.01, 5
  br i1 %3, label %4, label %15

4:                                                ; preds = %2
  %5 = add nsw i32 %.01, 1
  br label %6

6:                                                ; preds = %10, %4
  %.0 = phi i32 [ 0, %4 ], [ %11, %10 ]
  %7 = icmp slt i32 %.0, 5
  br i1 %7, label %8, label %12

8:                                                ; preds = %6
  %9 = add nsw i32 %5, 1
  br label %10

10:                                               ; preds = %8
  %11 = add nsw i32 %.0, 1
  br label %6, !llvm.loop !6

12:                                               ; preds = %6
  br label %13

13:                                               ; preds = %12
  %14 = add nsw i32 %.01, 1
  br label %2, !llvm.loop !8

15:                                               ; preds = %2
  ret void
}

attributes #0 = { noinline nounwind uwtable "frame-pointer"="all" "min-legal-vector-width"="0" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="x86-64" "target-features"="+cmov,+cx8,+fxsr,+mmx,+sse,+sse2,+x87" "tune-cpu"="generic" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 8, !"PIC Level", i32 2}
!2 = !{i32 7, !"PIE Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 2}
!4 = !{i32 7, !"frame-pointer", i32 2}
!5 = !{!"clang version 19.1.7 (/home/runner/work/llvm-project/llvm-project/clang cd708029e0b2869e80abe31ddb175f7c35361f90)"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
!8 = distinct !{!8, !7}
