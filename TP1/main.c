// Compilation Linux: gcc -Wall *c -o toto -fopenmp -msse2 -msse

#include "tp_utils.h"
#include <emmintrin.h>
#include <stdio.h>
#include <omp.h>

#define	ARRAY_LENGTH	512

void Add_Vfloat(float* A, float* B, float* C, unsigned short Length)
{
	unsigned short i;

	for (i = 0; i < Length; i++)
		C[i] = A[i] + B[i];
}

void  


void simd_Add_Vfloat(float* A, float* B, float* C, unsigned short Length)
{
}

void simd_openmp_Add_Vfloat(float* A, float* B, float* C, unsigned short Length)
{
}

float DotProduct(float* A, float* B, unsigned short Length)
{
	unsigned short i;
	float Result = 0.0;

	for (i = 0; i < Length; i++)
		Result += A[i] * B[i];

	return Result;
}

float simd_DotProduct(float* A, float* B, unsigned short Length)
{
	return 0.0;
}

float simd_openmp_DotProduct(float* A, float* B, unsigned short Length)
{
	return 0.0;
}

float Avg_Vfloat(float* A, unsigned short Length)
{
	unsigned short i;
	float Result = 0.0;

	for (i = 0; i < Length; i++)
		Result += A[i];

	return	Result / Length;
}

float simd_Avg_Vfloat(float* A, unsigned short Length)
{
	return 0.0;
}

float simd_openmp_Avg_Vfloat(float* A, unsigned short Length)
{
	return 0.0;
}

void Min3_Vfloat(float* A, float* R, unsigned short Length)
{
	unsigned short i;
	float tmp1, tmp2, min;

	for (i = 0; i < 4; i++)
	{
		R[i] = 0.0;
		R[Length - 1 - i] = 0.0;
	}

	for (i = 4; i<(Length - 4); i++)
	{
		tmp1 = A[i - 1];
		tmp2 = A[i];
		min = (tmp1 < tmp2) ? tmp1 : tmp2;
		tmp2 = A[i + 1];
		R[i] = (min < tmp2) ? min : tmp2;
	}
}

void simd_Min3_Vfloat(float* A, float* R, unsigned short Length)
{
}

void Max3_Vfloat(float* A, float* R, unsigned short Length)
{
	unsigned short i;
	float tmp1, tmp2, min;

	for (i = 0; i < 4; i++)
	{
		R[i] = 0.0;
		R[Length - 1 - i] = 0.0;
	}

	for (i = 4; i<(Length - 4); i++)
	{
		tmp1 = A[i - 1];
		tmp2 = A[i];
		min = (tmp1 > tmp2) ? tmp1 : tmp2;
		tmp2 = A[i + 1];
		R[i] = (min > tmp2) ? min : tmp2;
	}
}

void simd_Max3_Vfloat(float* A, float* R, unsigned short Length)
{
}

void Moy3_Vfloat(float* A, float* R, unsigned short Length)
{
	unsigned short i;

	for (i = 0; i < 4; i++)
	{
		R[i] = 0.0;
		R[Length - 1 - i] = 0.0;
	}

	for (i = 4; i<(Length - 4); i++)
		R[i] = (A[i - 1] + A[i] + A[i + 1]) / 3.0;
}

void simd_Moy3_Vfloat(float* A, float* R, unsigned short Length)
{
}

#define TRI(A,B)	tmp = (A > B) ? A : B;	\
					A   = (A <= B) ? A : B;	\
					B   = tmp
void Median3_Vfloat(float* A, float* R, unsigned short Length)
{
	unsigned short i;
	float tmpA, tmpB, tmpC, tmp;

	for (i = 0; i < 4; i++)
	{
		R[i] = 0.0;
		R[Length - 1 - i] = 0.0;
	}

	for (i = 4; i < (Length - 4); i++)
	{
		tmpA = A[i - 1]; tmpB = A[i]; tmpC = A[i + 1];

		TRI(tmpA, tmpB);
		TRI(tmpB, tmpC);
		TRI(tmpA, tmpB);

		R[i] = tmpB;
	}
}

void simd_Median3_Vfloat(float* A, float* R, unsigned short Length)
{
}

#ifdef WINDOWS_VERSION
float __declspec(align(ALIGNMENT))	TestFiltre3[] = { 1.2,	3.6,	7.8,	9.7,
6.8,	9.2,	65.8,	0.5,
1.2,	6.3,	23.0,	89.7,
56.7,	2.89,	67.2,	98.67,
12.7,	78.45,	89.0,	0.9 };
float __declspec(align(ALIGNMENT))	ResultFiltre3[20];
#else
float __attribute__((aligned(ALIGNMENT)))	TestFiltre3[] = { 1.2,	3.6,	7.8,	9.7,
6.8,	9.2,	65.8,	0.5,
1.2,	6.3,	23.0,	89.7,
56.7,	2.89,	67.2,	98.67,
12.7,	78.45,	89.0,	0.9 };
float __attribute__((aligned(ALIGNMENT)))	ResultFiltre3[20];
#endif

int main(void)
{
	float*	A = float_InitArrayWithStep(0.0, 0.5, ARRAY_LENGTH);
	float*	B = float_InitArrayWithStep(1.0, 1.0, ARRAY_LENGTH);
	float*	C = float_CreateArray(ARRAY_LENGTH);

	omp_set_num_threads(4);

	float_PrintArray("A", A, 8);	printf("\n");
	float_PrintArray("B", B, 8);	printf("\n");

	Add_Vfloat(A, B, C, ARRAY_LENGTH);
	printf("Version SCALAIRE\t: Addition des tableaux A et B :\n");
	float_PrintArray("C", C, 8);

	simd_Add_Vfloat(A, B, C, ARRAY_LENGTH);
	printf("Version SIMD\t\t: Addition des tableaux A et B :\n");
	float_PrintArray("C", C, 8);

	simd_openmp_Add_Vfloat(A, B, C, ARRAY_LENGTH);
	printf("Version SIMD/OPENMP\t: Addition des tableaux A et B :\n");
	float_PrintArray("C", C, 8);


	printf("\n");
	printf("Version SCALAIRE\t: Le produit scalaire de A et B vaut %6.2f\n", DotProduct(A, B, ARRAY_LENGTH));
	printf("Version SCALAIRE\t: La moyenne de A vaut %6.2f\n", Avg_Vfloat(A, ARRAY_LENGTH));
	printf("\n");

	printf("\n");
	printf("Version SIMD\t\t: Le produit scalaire de A et B vaut %6.2f\n", simd_DotProduct(A, B, ARRAY_LENGTH));
	printf("Version SIMD\t\t: La moyenne de A vaut %6.2f\n", simd_Avg_Vfloat(A, ARRAY_LENGTH));
	printf("\n");

	printf("\n");
	printf("Version SIMD/OPENMP\t: Le produit scalaire de A et B vaut %6.2f\n", simd_openmp_DotProduct(A, B, ARRAY_LENGTH));
	printf("Version SIMD/OPENMP\t: La moyenne de A vaut %6.2f\n", simd_openmp_Avg_Vfloat(A, ARRAY_LENGTH));
	printf("\n");

	printf("\n");
	Min3_Vfloat(TestFiltre3, ResultFiltre3, 20);
	printf("Version SCALAIRE\t: Min3\n");
	float_PrintArray("TestMin", TestFiltre3, sizeof(TestFiltre3) / sizeof(TestFiltre3[0]));
	printf("\n");
	float_PrintArray("ResuMin", ResultFiltre3, sizeof(ResultFiltre3) / sizeof(ResultFiltre3[0]));

	printf("\n");
	simd_Min3_Vfloat(TestFiltre3, ResultFiltre3, 20);
	printf("Version SIMD\t\t: Min3\n");
	float_PrintArray("TestMin", TestFiltre3, sizeof(TestFiltre3) / sizeof(TestFiltre3[0]));
	printf("\n");
	float_PrintArray("ResuMin", ResultFiltre3, sizeof(ResultFiltre3) / sizeof(ResultFiltre3[0]));

	printf("\n");
	Max3_Vfloat(TestFiltre3, ResultFiltre3, 20);
	printf("Version SCALAIRE\t: Max3\n");
	float_PrintArray("TestMax", TestFiltre3, sizeof(TestFiltre3) / sizeof(TestFiltre3[0]));
	printf("\n");
	float_PrintArray("ResuMax", ResultFiltre3, sizeof(ResultFiltre3) / sizeof(ResultFiltre3[0]));

	printf("\n");
	simd_Max3_Vfloat(TestFiltre3, ResultFiltre3, 20);
	printf("Version SIMD\t\t: Max3\n");
	float_PrintArray("TestMax", TestFiltre3, sizeof(TestFiltre3) / sizeof(TestFiltre3[0]));
	printf("\n");
	float_PrintArray("ResuMax", ResultFiltre3, sizeof(ResultFiltre3) / sizeof(ResultFiltre3[0]));

	printf("\n");
	Moy3_Vfloat(TestFiltre3, ResultFiltre3, 20);
	printf("Version SCALAIRE\t: Moy3\n");
	float_PrintArray("TestMoy", TestFiltre3, sizeof(TestFiltre3) / sizeof(TestFiltre3[0]));
	printf("\n");
	float_PrintArray("ResuMoy", ResultFiltre3, sizeof(ResultFiltre3) / sizeof(ResultFiltre3[0]));

	printf("\n");
	simd_Moy3_Vfloat(TestFiltre3, ResultFiltre3, 20);
	printf("Version SIMD\t\t: Moy3\n");
	float_PrintArray("TestMoy", TestFiltre3, sizeof(TestFiltre3) / sizeof(TestFiltre3[0]));
	printf("\n");
	float_PrintArray("ResuMoy", ResultFiltre3, sizeof(ResultFiltre3) / sizeof(ResultFiltre3[0]));

	printf("\n");
	Median3_Vfloat(TestFiltre3, ResultFiltre3, 20);
	printf("Version SCALAIRE\t: Median3\n");
	float_PrintArray("TestMedian", TestFiltre3, sizeof(TestFiltre3) / sizeof(TestFiltre3[0]));
	printf("\n");
	float_PrintArray("ResuMedian", ResultFiltre3, sizeof(ResultFiltre3) / sizeof(ResultFiltre3[0]));

	printf("\n");
	simd_Median3_Vfloat(TestFiltre3, ResultFiltre3, 20);
	printf("Version SIMD\t\t: Median3\n");
	float_PrintArray("TestMedian", TestFiltre3, sizeof(TestFiltre3) / sizeof(TestFiltre3[0]));
	printf("\n");
	float_PrintArray("ResuMedian", ResultFiltre3, sizeof(ResultFiltre3) / sizeof(ResultFiltre3[0]));

	float_FreeArray(A);
	float_FreeArray(B);
	float_FreeArray(C);

#ifdef WINDOWS_VERSION
	getchar();
#endif

	return 0;
}
