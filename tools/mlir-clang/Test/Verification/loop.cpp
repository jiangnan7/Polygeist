// RUN: mlir-clang %s --function=* -S | FileCheck %s

int MAX_DIMS;

struct A {
    int x;
    double y;
};

void div_(int* sizes) {
    A data[25];
    for (int i=0; i < MAX_DIMS; ++i) {
            data[i].x = sizes[i];
    }
}

// CHECK:   func @_Z4div_Pi(%arg0: memref<?xi32>) attributes {llvm.linkage = #llvm.linkage<external>} {
// CHECK-DAG:     %c1_i32 = arith.constant 1 : i32
// CHECK-DAG:     %c0_i32 = arith.constant 0 : i32
// CHECK-DAG:     %c1_i64 = arith.constant 1 : i64
// CHECK-NEXT:     %0 = llvm.alloca %c1_i64 x !llvm.array<25 x struct<(i32, f64)>> : (i64) -> !llvm.ptr<array<25 x struct<(i32, f64)>>>
// CHECK-NEXT:     %1 = memref.get_global @MAX_DIMS : memref<1xi32>
// CHECK-NEXT:     %2 = scf.while (%arg1 = %c0_i32) : (i32) -> i32 {
// CHECK-NEXT:       %3 = affine.load %1[0] : memref<1xi32>
// CHECK-NEXT:       %4 = arith.cmpi ult, %arg1, %3 : i32
// CHECK-NEXT:       scf.condition(%4) %arg1 : i32
// CHECK-NEXT:     } do {
// CHECK-NEXT:     ^bb0(%arg1: i32):  // no predecessors
// CHECK-NEXT:       %3 = arith.index_cast %arg1 : i32 to index
// CHECK-NEXT:       %4 = arith.index_cast %3 : index to i64
// CHECK-NEXT:       %5 = llvm.getelementptr %0[%c0_i32, %c0_i32] : (!llvm.ptr<array<25 x struct<(i32, f64)>>>, i32, i32) -> !llvm.ptr<struct<(i32, f64)>>
// CHECK-NEXT:       %6 = llvm.getelementptr %5[%4] : (!llvm.ptr<struct<(i32, f64)>>, i64) -> !llvm.ptr<struct<(i32, f64)>>
// CHECK-NEXT:       %7 = llvm.getelementptr %6[%c0_i32, %c0_i32] : (!llvm.ptr<struct<(i32, f64)>>, i32, i32) -> !llvm.ptr<i32>
// CHECK-NEXT:       %8 = memref.load %arg0[%3] : memref<?xi32>
// CHECK-NEXT:       llvm.store %8, %7 : !llvm.ptr<i32>
// CHECK-NEXT:       %9 = arith.addi %arg1, %c1_i32 : i32
// CHECK-NEXT:       scf.yield %9 : i32
// CHECK-NEXT:     }
// CHECK-NEXT:     return
// CHECK-NEXT:   }