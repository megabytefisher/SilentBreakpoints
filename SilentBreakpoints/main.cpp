#include <Windows.h>
#include <stdio.h>
#include "TargetLibrary.h"

DWORD breakpointLocation;
DWORD replacementLocation;

void replacementFunction(int a, int b)
{
	printf("Hooked target! a = %i, b=%i\n", a, b);
	return;
}

LONG CALLBACK exceptionHandler(EXCEPTION_POINTERS *pExceptionInfo) {
	if (pExceptionInfo->ExceptionRecord->ExceptionCode == EXCEPTION_ACCESS_VIOLATION)
	{
		if (pExceptionInfo->ContextRecord->Eip == breakpointLocation)
		{
			// redirect the call
			pExceptionInfo->ContextRecord->Eip = replacementLocation;
			return EXCEPTION_CONTINUE_EXECUTION;
		}
		else if (pExceptionInfo->ContextRecord->Eip == (DWORD)pExceptionInfo->ExceptionRecord->ExceptionInformation[1])
		{
			// if eip matches the address we can't access, it probably needs to be executable
			DWORD oldProtect;
			VirtualProtect((LPVOID)pExceptionInfo->ContextRecord->Eip, 16, PAGE_EXECUTE_READWRITE, &oldProtect);
			return EXCEPTION_CONTINUE_EXECUTION;
		}
	}
	return EXCEPTION_CONTINUE_SEARCH;
}

void setupFunctionRedirect(DWORD target, DWORD replacement)
{
	breakpointLocation = target;
	replacementLocation = replacement;

	DWORD oldProtect;
	VirtualProtect((LPVOID)target, 8, PAGE_READWRITE, &oldProtect);

	AddVectoredExceptionHandler(1, exceptionHandler);
	// this is already enabled in later versions of visual studio, but we'll enable it just to be safe.
	SetProcessDEPPolicy(1);
}

int main()
{
	setupFunctionRedirect((DWORD)&targetFunction, (DWORD)&replacementFunction);
	targetFunction(1, 2);

	getchar();
	return 0;
}