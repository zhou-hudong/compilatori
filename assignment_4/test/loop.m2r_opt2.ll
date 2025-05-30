; ModuleID = 'loop.optimized.ll'
source_filename = "loop.c"
target datalayout = "e-m:e-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-f80:128-n8:16:32:64-S128"
target triple = "x86_64-unknown-linux-gnu"

; Function Attrs: noinline nounwind uwtable
define dso_local void @Loop1() #0 {
  %1 = icmp eq i32 0, 0
  br i1 %1, label %2, label %10

2:                                                ; preds = %0
  br label %3

3:                                                ; preds = %7, %2
  %.02 = phi i32 [ 0, %2 ], [ %8, %7 ]
  %.0 = phi i32 [ 0, %2 ], [ %6, %7 ]
  %4 = icmp slt i32 %.02, 5
  br i1 %4, label %5, label %9

5:                                                ; preds = %3
  %6 = add nsw i32 %.0, 1
  br label %7

7:                                                ; preds = %5
  %8 = add nsw i32 %.02, 1
  br label %3, !llvm.loop !6

9:                                                ; preds = %3
  br label %10

10:                                               ; preds = %9, %0
  br label %11

11:                                               ; preds = %15, %10
  %.03 = phi i32 [ 0, %10 ], [ %16, %15 ]
  %.01 = phi i32 [ 0, %10 ], [ %14, %15 ]
  %12 = icmp slt i32 %.03, 5
  br i1 %12, label %13, label %17

13:                                               ; preds = %11
  %14 = add nsw i32 %.01, 1
  br label %15

15:                                               ; preds = %13
  %16 = add nsw i32 %.03, 1
  br label %11, !llvm.loop !8

17:                                               ; preds = %11
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @Loop2() #0 {
  br label %1

1:                                                ; preds = %8, %0
  %.02 = phi i32 [ 0, %0 ], [ %9, %8 ]
  %2 = phi i32 [ 0, %0 ], [ %7, %8 ]
  %.0 = phi i32 [ 0, %0 ], [ %5, %8 ]
  %3 = icmp slt i32 %.02, 5
  br i1 %3, label %4, label %10

4:                                                ; preds = %1
  %5 = add nsw i32 %.0, 1
  br label %6

6:                                                ; preds = %4
  %7 = add nsw i32 %2, 1
  br label %8

8:                                                ; preds = %6
  %9 = add nsw i32 %.02, 1
  br label %1, !llvm.loop !9

10:                                               ; preds = %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @Loop3() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %.02 = phi i32 [ 0, %0 ], [ %6, %5 ]
  %.0 = phi i32 [ 0, %0 ], [ %4, %5 ]
  %2 = icmp slt i32 %.02, 5
  br i1 %2, label %3, label %7

3:                                                ; preds = %1
  %4 = add nsw i32 %.0, 1
  br label %5

5:                                                ; preds = %3
  %6 = add nsw i32 %.02, 1
  br label %1, !llvm.loop !10

7:                                                ; preds = %1
  %8 = mul nsw i32 %.0, 2
  br label %9

9:                                                ; preds = %13, %7
  %.03 = phi i32 [ 0, %7 ], [ %14, %13 ]
  %.01 = phi i32 [ 0, %7 ], [ %12, %13 ]
  %10 = icmp slt i32 %.03, 5
  br i1 %10, label %11, label %15

11:                                               ; preds = %9
  %12 = add nsw i32 %.01, 1
  br label %13

13:                                               ; preds = %11
  %14 = add nsw i32 %.03, 1
  br label %9, !llvm.loop !11

15:                                               ; preds = %9
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @Loop4() #0 {
  br label %1

1:                                                ; preds = %5, %0
  %.02 = phi i32 [ 0, %0 ], [ %6, %5 ]
  %.0 = phi i32 [ 0, %0 ], [ %4, %5 ]
  %2 = icmp slt i32 %.02, 5
  br i1 %2, label %3, label %7

3:                                                ; preds = %1
  %4 = add nsw i32 %.0, 1
  br label %5

5:                                                ; preds = %3
  %6 = add nsw i32 %.02, 1
  br label %1, !llvm.loop !12

7:                                                ; preds = %1
  %8 = icmp sgt i32 %.0, 0
  br i1 %8, label %9, label %11

9:                                                ; preds = %7
  %10 = mul nsw i32 %.0, 2
  br label %11

11:                                               ; preds = %9, %7
  br label %12

12:                                               ; preds = %16, %11
  %.03 = phi i32 [ 0, %11 ], [ %17, %16 ]
  %.01 = phi i32 [ 0, %11 ], [ %15, %16 ]
  %13 = icmp slt i32 %.03, 5
  br i1 %13, label %14, label %18

14:                                               ; preds = %12
  %15 = add nsw i32 %.01, 1
  br label %16

16:                                               ; preds = %14
  %17 = add nsw i32 %.03, 1
  br label %12, !llvm.loop !13

18:                                               ; preds = %12
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @Loop5() #0 {
  %1 = icmp eq i32 0, 0
  br i1 %1, label %2, label %13

2:                                                ; preds = %0
  br label %3

3:                                                ; preds = %10, %2
  %.03 = phi i32 [ 0, %2 ], [ %11, %10 ]
  %4 = phi i32 [ 0, %2 ], [ %9, %10 ]
  %.0 = phi i32 [ 0, %2 ], [ %7, %10 ]
  %5 = icmp slt i32 %.03, 5
  br i1 %5, label %6, label %12

6:                                                ; preds = %3
  %7 = add nsw i32 %.0, 1
  br label %8

8:                                                ; preds = %6
  %9 = add nsw i32 %4, 1
  br label %10

10:                                               ; preds = %8
  %11 = add nsw i32 %.03, 1
  br label %3, !llvm.loop !14

12:                                               ; preds = %3
  br label %13

13:                                               ; preds = %12, %0
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @Loop6() #0 {
  br label %1

1:                                                ; preds = %19, %0
  %.03 = phi i32 [ 0, %0 ], [ %20, %19 ]
  %2 = phi i32 [ 0, %0 ], [ %.12, %19 ]
  %.0 = phi i32 [ 0, %0 ], [ %.1, %19 ]
  %3 = icmp slt i32 %.03, 5
  br i1 %3, label %4, label %21

4:                                                ; preds = %1
  br label %5

5:                                                ; preds = %9, %4
  %.04 = phi i32 [ 0, %4 ], [ %10, %9 ]
  %.1 = phi i32 [ %.0, %4 ], [ %8, %9 ]
  %6 = icmp slt i32 %.04, 5
  br i1 %6, label %7, label %11

7:                                                ; preds = %5
  %8 = add nsw i32 %.1, 1
  br label %9

9:                                                ; preds = %7
  %10 = add nsw i32 %.04, 1
  br label %5, !llvm.loop !15

11:                                               ; preds = %5
  br label %12

12:                                               ; preds = %11, %17
  %.06 = phi i32 [ 0, %11 ], [ %18, %17 ]
  %.12 = phi i32 [ %2, %11 ], [ %16, %17 ]
  %13 = icmp slt i32 %.06, 5
  br i1 %13, label %15, label %14

14:                                               ; preds = %12
  br label %19

15:                                               ; preds = %12
  %16 = add nsw i32 %.12, 1
  br label %17

17:                                               ; preds = %15
  %18 = add nsw i32 %.06, 1
  br label %12, !llvm.loop !16

19:                                               ; preds = %14
  %20 = add nsw i32 %.03, 1
  br label %1, !llvm.loop !17

21:                                               ; preds = %1
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @Loop7() #0 {
  %1 = icmp eq i32 0, 0
  br i1 %1, label %2, label %13

2:                                                ; preds = %0
  br label %3

3:                                                ; preds = %10, %2
  %.03 = phi i32 [ 0, %2 ], [ %11, %10 ]
  %4 = phi i32 [ 0, %2 ], [ %9, %10 ]
  %.0 = phi i32 [ 0, %2 ], [ %7, %10 ]
  %5 = icmp slt i32 %.03, 5
  br i1 %5, label %6, label %12

6:                                                ; preds = %3
  %7 = add nsw i32 %.0, 1
  br label %8

8:                                                ; preds = %6
  %9 = add nsw i32 %4, 1
  br label %10

10:                                               ; preds = %8
  %11 = add nsw i32 %.03, 1
  br label %3, !llvm.loop !18

12:                                               ; preds = %3
  br label %13

13:                                               ; preds = %0, %12
  ret void
}

; Function Attrs: noinline nounwind uwtable
define dso_local void @Loop8() #0 {
  %1 = alloca [5 x i32], align 16
  %2 = alloca [5 x i32], align 16
  br label %3

3:                                                ; preds = %9, %0
  %.0 = phi i32 [ 0, %0 ], [ %10, %9 ]
  %4 = icmp slt i32 %.0, 5
  br i1 %4, label %5, label %11

5:                                                ; preds = %3
  %6 = add nsw i32 %.0, 1
  %7 = sext i32 %.0 to i64
  %8 = getelementptr inbounds [5 x i32], ptr %1, i64 0, i64 %7
  store i32 %6, ptr %8, align 4
  br label %9

9:                                                ; preds = %5
  %10 = add nsw i32 %.0, 1
  br label %3, !llvm.loop !19

11:                                               ; preds = %3
  br label %12

12:                                               ; preds = %22, %11
  %.01 = phi i32 [ 0, %11 ], [ %23, %22 ]
  %13 = icmp slt i32 %.01, 5
  br i1 %13, label %14, label %24

14:                                               ; preds = %12
  %15 = add nsw i32 %.01, 3
  %16 = sext i32 %15 to i64
  %17 = getelementptr inbounds [5 x i32], ptr %1, i64 0, i64 %16
  %18 = load i32, ptr %17, align 4
  %19 = add nsw i32 %18, 1
  %20 = sext i32 %.01 to i64
  %21 = getelementptr inbounds [5 x i32], ptr %2, i64 0, i64 %20
  store i32 %19, ptr %21, align 4
  br label %22

22:                                               ; preds = %14
  %23 = add nsw i32 %.01, 1
  br label %12, !llvm.loop !20

24:                                               ; preds = %12
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
!9 = distinct !{!9, !7}
!10 = distinct !{!10, !7}
!11 = distinct !{!11, !7}
!12 = distinct !{!12, !7}
!13 = distinct !{!13, !7}
!14 = distinct !{!14, !7}
!15 = distinct !{!15, !7}
!16 = distinct !{!16, !7}
!17 = distinct !{!17, !7}
!18 = distinct !{!18, !7}
!19 = distinct !{!19, !7}
!20 = distinct !{!20, !7}
