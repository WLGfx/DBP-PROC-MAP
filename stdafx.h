// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// Windows Header Files:

#include <windows.h>
#include <vector>
#include <math.h>

// TODO: reference additional headers your program requires here

#include <DBPro.hpp>
#include "PROC_MAP.h"
#include "_GDK_PM.h"

// define the EXPORT functions

#define EXPORT __declspec(dllexport)
#define EXPORTC extern "C" __declspec(dllexport)

// make sure to use ASM optimised code (not used at the moment)

#define WL_USE_ASM

// other function declarations

sFrame* WL_dbCreateNewFrame( sObject* pObject, LPSTR pName, bool bNewMesh );

// external references

extern GlobStruct *g_pGlob;

// link DBP defs library

#pragma comment ( lib, "DBProInterface9.0.lib" )