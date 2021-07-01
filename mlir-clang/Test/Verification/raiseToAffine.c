// RUN: split-file %s %t

// RUN: mlir-clang %t/matmul_signed_cmp.cpp --function=matmul --raise-scf-to-affine | FileCheck %s -check-prefix=GEMMSIGNED
// RUN: mlir-clang %t/matmul_unsigned_cmp.cpp --function=matmul_unsigned_cmp --raise-scf-to-affine | FileCheck %s -check-prefix=GEMMUNSIGNED

//--- matmul_signed_cmp.cpp
#define N 200
#define M 300
#define K 400
#define DATA_TYPE float

void matmul(DATA_TYPE A[N][K], DATA_TYPE B[K][M], DATA_TYPE C[N][M]) {
  int i, j, k;
  // GEMMSIGNED: affine.for
  for (i = 0; i < N; i++)
    // GEMMSIGNED: affine.for
    for (j = 0; j < M; j++)
      // GEMMSIGNED: affine.for
      for (k = 0; k < K; k++)
        // GEMMSIGNED: {{.*}} = affine.load %{{.*}}[%{{.*}}, %{{.*}}] : memref<?x400xf32>
        // GEMMSIGNED: {{.*}} = affine.load %{{.*}}[%{{.*}}, %{{.*}}] : memref<?x300xf32>
        // GEMMSIGNED: {{.*}} = mulf
        // GEMMSIGNED: {{.*}} = affine.load %{{.*}}[%{{.*}}, %{{.*}}] : memref<?x300xf32>
        // GEMMSIGNED: {{.*}} = addf
        // GEMMSIGNED: affine.store {{.*}}, %{{.*}}[%{{.*}}, %{{.*}}] : memref<?x300xf32>
        C[i][j] += A[i][k] * B[k][j];
}

//--- matmul_unsigned_cmp.cpp
void matmul_unsigned_cmp(float A[100][200], float B[200][300], float C[100][300]) {
  int i, j, k;

  // GEMMUNSIGNED: affine.for
  for (i = 0; i < 100; i++) {
    // GEMMUNSIGNED: affine.for
    for (j = 0; j < 300; j++) {
      // GEMMUNSIGNED: affine.store %{{.*}}, %{{.*}}[%{{.*}}, %{{.*}}] : memref<?x300xf32>
      C[i][j] = 0;
      // GEMMUNSIGNED: affine.for
      for (k = 0; k < 200; k++) {
        // GEMMUNSIGNED: {{.*}} = affine.load %{{.*}}[%{{.*}}, %{{.*}}] : memref<?x200xf32>
        // GEMMUNSIGNED: {{.*}} = affine.load %{{.*}}[%{{.*}}, %{{.*}}] : memref<?x300xf32>
        // GEMMUNSIGNED: {{.*}} = mulf
        // GEMMUNSIGNED: {{.*}} = affine.load %{{.*}}[%{{.*}}, %{{.*}}] : memref<?x300xf32>
        // GEMMUNSIGNED: {{.*}} = addf
        // GEMMUNSIGNED: affine.store {{.*}}, %{{.*}}[%{{.*}}, %{{.*}}] : memref<?x300xf32>
        C[i][j] += A[i][k] * B[k][j];
      }
    }
  }
}
