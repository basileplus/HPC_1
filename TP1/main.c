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


void simd_Add_Vfloat(float* A, float* B, float* C, unsigned short Length)
{

	for (int i = 0; i < Length; i += 4) {
		__m128 vectA = _mm_load_ps(&A[i]);
		__m128 vectB = _mm_load_ps(&B[i]);
		__m128 vectC = _mm_add_ps(vectA, vectB);
		_mm_store_ps(&C[i], vectC);
	}
}

void simd_openmp_Add_Vfloat(float* A, float* B, float* C, unsigned short Length)
{
	int i;
    #pragma omp parallel for
    for (i = 0; i < Length; i += 4) {
        __m128 vectA = _mm_load_ps(&A[i]);
        __m128 vectB = _mm_load_ps(&B[i]);
        __m128 vectC = _mm_add_ps(vectA, vectB);
        _mm_store_ps(&C[i], vectC);
    }
}

float DotProduct(float* A, float* B, unsigned short Length)
{
	unsigned short i;
	float Result = 0.0;

	for (i = 0; i < Length; i++)
		Result += A[i] * B[i];

	return Result;
}

// On manipule des floats. Un float * float = float et float + float = float donc pas de prblème de débordement (dans la plupart des cas)
// Ca marche pareil pour les doubles
float simd_DotProduct(float* A, float* B, unsigned short Length)
{
    __m128 sum = _mm_setzero_ps();
    for (int i = 0; i < Length; i += 4) {
        __m128 vectA = _mm_load_ps(&A[i]);
        __m128 vectB = _mm_load_ps(&B[i]);
        __m128 mul = _mm_mul_ps(vectA, vectB);
        sum = _mm_add_ps(sum, mul); // ps pour simple precision : float sur 32bits
    }
    float result[4];
    _mm_store_ps(result, sum);
    return result[0] + result[1] + result[2] + result[3];
}

float simd_openmp_DotProduct(float* A, float* B, unsigned short Length)
{
    __m128 sum = _mm_setzero_ps();
    #pragma omp parallel
    {
        __m128 local_sum = _mm_setzero_ps();
		int i;
        #pragma omp for
        for (i = 0; i < Length; i += 4) {
            __m128 vectA = _mm_load_ps(&A[i]);
            __m128 vectB = _mm_load_ps(&B[i]);
            __m128 mul = _mm_mul_ps(vectA, vectB);
            local_sum = _mm_add_ps(local_sum, mul);
        }
        #pragma omp critical
        {
            sum = _mm_add_ps(sum, local_sum);
        }
    }
    float result[4];
    _mm_store_ps(result, sum);
    return result[0] + result[1] + result[2] + result[3];
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
    __m128 sum = _mm_setzero_ps();
    for (int i = 0; i < Length; i += 4) {
        __m128 vectA = _mm_load_ps(&A[i]);
        sum = _mm_add_ps(sum, vectA);
    }
    float result[4];
    _mm_store_ps(result, sum);
    float totalSum = result[0] + result[1] + result[2] + result[3];
    return totalSum / Length;
}

float simd_openmp_Avg_Vfloat(float* A, unsigned short Length)
{
    __m128 sum = _mm_setzero_ps();
    #pragma omp parallel
    {
        __m128 local_sum = _mm_setzero_ps();
		int i;
        #pragma omp for
        for (i = 0; i < Length; i += 4) {
            __m128 vectA = _mm_load_ps(&A[i]);
            local_sum = _mm_add_ps(local_sum, vectA);
        }
        #pragma omp critical
        {
            sum = _mm_add_ps(sum, local_sum);
        }
    }
    float result[4];
    _mm_store_ps(result, sum);
    float totalSum = result[0] + result[1] + result[2] + result[3];
    return totalSum / Length;
}

void Min3_Vfloat(float* A, float* R, unsigned short Length)
{
	unsigned short i;
	float tmp1, tmp2, min;

	// pour éviter les bords ? on intialise les 4 premières et dernières valeurs à 0
	for (i = 0; i < 4; i++)
	{
		R[i] = 0.0;
		R[Length - 1 - i] = 0.0;
	}

	// on compare les 3 valeurs et on garde la plus petite
	for (i = 4; i<(Length - 4); i++)
	{
		tmp1 = A[i - 1];
		tmp2 = A[i];
		min = (tmp1 < tmp2) ? tmp1 : tmp2;
		tmp2 = A[i + 1];
		R[i] = (min < tmp2) ? min : tmp2;
	}
}

//void simd_Min3_Vfloat(float* A, float* R, unsigned short Length)
//{
//	__m128 min;
//	__m128 tmp1, tmp2, tmp3;
//	for (int i = 0; i < Length; i += 4) {
//		tmp1 = _mm_load_ps(&A[i]);
//		tmp2 = _mm_loadu_ps(&A[i + 1]);
//		tmp3 = _mm_loadu_ps(&A[i + 2]);
//		min = _mm_min_ps(tmp1, tmp2);
//		min = _mm_min_ps(min, tmp3);
//		_mm_store_ps(&R[i], min);
//	}
//}

// autre version n'utilsant qu'un load par iteration et pas de unaligned load
void simd_Min3_Vfloat(float* A, float* R, unsigned short Length)
{
	for (int i = 0; i < 4; i++)
	{
		R[i] = 0.0;
		R[Length - 1 - i] = 0.0;
	}

	__m128 min;
	__m128 Vphi, V1, V2= _mm_setzero_ps();
	Vphi = _mm_load_ps(&A[0]);
	V1 = _mm_load_ps(&A[4]);
	for (int i = 8; i < Length; i += 4) {
		V2 = _mm_load_ps(&A[i]);
		min = _mm_min_ps(vec_left1_ps(Vphi, V1), V1);
		min = _mm_min_ps(V1, vec_right1_ps(V1, V2));
		_mm_store_ps(&R[i], min);
		Vphi = V1;
		V1 = V2;
	}
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
	for (int i = 0; i < 4; i++)
	{
		R[i] = 0.0;
		R[Length - 1 - i] = 0.0;
	}
	__m128 max;
	__m128 tmp1, tmp2, tmp3;
	for (int i = 0; i < Length; i += 4) {
		tmp1 = _mm_load_ps(&A[i]);
		tmp2 = _mm_loadu_ps(&A[i + 1]);
		tmp3 = _mm_loadu_ps(&A[i + 2]);
		max = _mm_max_ps(tmp1, tmp2);
		max = _mm_max_ps(max, tmp3);
		_mm_store_ps(&R[i], max);
	}
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
	for (int i = 0; i < 4; i++)
	{
		R[i] = 0.0;
		R[Length - 1 - i] = 0.0;
	}

	__m128 left, center, right, sum;
	for (int i = 4; i < Length - 4; i += 4) {
		left = _mm_loadu_ps(&A[i - 1]);
		center = _mm_load_ps(&A[i]);
		right = _mm_loadu_ps(&A[i + 1]);
		sum = _mm_add_ps(_mm_add_ps(left, center), right);
		sum = _mm_div_ps(sum, _mm_set1_ps(3.0f));
		_mm_store_ps(&R[i], sum);
	}
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

#define TRI2(vecA, vecB) tmp = _mm_max_ps(vecA, vecB); \
						vecA =_mm_min_ps( vecA, vecB); \
						vecB = tmp;


void simd_Median3_Vfloat(float* A, float* R, unsigned short Length)
{
	__m128 V1, V2, V3;
	__m128 a, b, c;
	__m128 tmp;
	V1 = _mm_load_ps(&A[0]);
	V2 = _mm_load_ps(&A[4]);
	for (int i = 8; i < Length; i += 4) {
		V3 = _mm_load_ps(&A[i]);
		a = vec_left1_ps(V1, V2);
		b = V2;
		c = vec_right1_ps(V2, V3);
		TRI2(a,b);
		TRI2(b,c);
		TRI2(a,b);
		_mm_store_ps(&R[i-4], b);
	}
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
