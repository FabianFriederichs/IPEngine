// CoreTetst.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "ICore.h"
#include "Injector.h"

int main()
{
	Injector inj("dep.xml", "..\Debug");
	inj.LoadModules();


    return 0;
}

