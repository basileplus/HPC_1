#include "tp_utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

float*	float_CreateArray(unsigned short Length)
{
	float* Array = NULL;
	if (Length > 0)
#ifdef WINDOWS_VERSION
		Array = (float*)_aligned_malloc(Length * sizeof(float), ALIGNMENT);
#else
		Array = (float*)memalign(ALIGNMENT, Length * sizeof(float));
#endif

	return Array;
}

float*	float_InitArrayWithStep(float First, float Step, unsigned short Length)
{
	unsigned short	i;
	float* Array = NULL;

	if (Length > 0)
#ifdef WINDOWS_VERSION
		Array = (float*)_aligned_malloc(Length * sizeof(float), ALIGNMENT);
#else
		Array = (float*)memalign(ALIGNMENT, Length * sizeof(float));
#endif

	if (Array != NULL)
	{
		Array[0] = First;
		for (i = 1; i<Length; i++)
			Array[i] = Array[i - 1] + Step;
	}

	return Array;
}

#define NB_CARACTERS_BY_LINE	4
void float_PrintArray(char* str, float* Array, unsigned short Length)
{
	unsigned short	i;
	unsigned char	j;

	for (i = 0; i<Length; i += NB_CARACTERS_BY_LINE)
	{
		for (j = 0; (j<NB_CARACTERS_BY_LINE) && ((i + j)<Length); j++)
			printf("%s[%3d]=%6.2f ", str, i + j, Array[i + j]);
		printf("\n");
	}
}

void float_FreeArray(float* Array)
{
#ifdef WINDOWS_VERSION
	_aligned_free(Array);
#else
	free(Array);
#endif
}
