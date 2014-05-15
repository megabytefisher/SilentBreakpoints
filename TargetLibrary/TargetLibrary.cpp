#include "TargetLibrary.h"

__declspec(dllexport) int targetFunction(int a, int b)
{
	puts("Hello from targetFunction!");
	return a + b;
}
