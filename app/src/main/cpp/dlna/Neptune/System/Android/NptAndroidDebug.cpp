/*****************************************************************
|
|      File: NptStdcDebug.c
|
|      Atomix - Debug Support: Android Implementation
|
|      (c) 2002-2009 Gilles Boccon-Gibod
|      Author: Gilles Boccon-Gibod (bok@bok.net)
|
 ****************************************************************/

/*----------------------------------------------------------------------
|       includes
+---------------------------------------------------------------------*/
#include <stdio.h>

#include "../../Core/NptConfig.h"
#include "../../Core/NptDefs.h"
#include "../../Core/NptTypes.h"
#include "../../Core/NptDebug.h"

/*----------------------------------------------------------------------
|       NPT_DebugOuput
+---------------------------------------------------------------------*/
void
NPT_DebugOutput(const char* message)
{
    printf("%s", message);
}
