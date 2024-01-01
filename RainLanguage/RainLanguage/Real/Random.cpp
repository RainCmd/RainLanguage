#include "Random.h"
#include "MathReal.h"

#undef INT32_MAX
constexpr int32 INT32_MAX = 0x7FFFFFFF;
int32 Random::InternalSample()
{
	int32 num = inext;
	int32 num2 = inextp;
	if (++num >= 56)
	{
		num = 1;
	}

	if (++num2 >= 56)
	{
		num2 = 1;
	}

	int32 num3 = seedArray[num] - seedArray[num2];
	if (num3 < 0)
	{
		num3 += INT32_MAX;
	}

	seedArray[num] = num3;
	inext = num;
	inextp = num2;
	return num3;
}
void Random::SetSeed(integer seed)
{
	int32 num = 161803398 - (int32)seed;
	seedArray[55] = num;
	int32 num2 = 1;
	for (int32 i = 1; i < 55; i++)
	{
		int32 num3 = 21 * i % 55;
		seedArray[num3] = num2;
		num2 = num - num2;
		if (num2 < 0)
		{
			num2 += INT32_MAX;
		}

		num = seedArray[num3];
	}

	for (int32 j = 1; j < 5; j++)
	{
		for (int32 k = 1; k < 56; k++)
		{
			seedArray[k] -= seedArray[1 + (k + 30) % 55];
			if (seedArray[k] < 0)
			{
				seedArray[k] += INT32_MAX;
			}
		}
	}

	inext = 0;
	inextp = 21;
}

real Random::NextReal()
{
#ifdef FIXED_REAL
	return real::CreateReal(Next() & MASK_DECIMAL);
#else
	return (real)InternalSample() * 4.6566128752457969E-10;
#endif // FIXED
}
