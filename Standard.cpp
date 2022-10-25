#include "stdafx.h"

//
// see: http://forum.thegamecreators.com/?m=forum_view&t=190224&b=18
// for more refernces
//

#pragma comment (lib, "DBProInterface9.0.lib")
//#pragma comment (lib, "comctl32.lib")

GlobStruct*	g_pGlob;
extern _GDK_PM MAP;

// Constructor

void Constructor ( void ) { // Create memory here
	//InitCommonControls();
}

// Destructor

EXPORT void PreDestructor(void) { // Early shutdown stuff goes here
	//delete MAP;
}

void Destructor ( void ) { // Free memory here
}

// Get GlobStruct

void ReceiveCoreDataPtr ( LPVOID pCore ) { // Get Core Data Pointer here
	g_pGlob = (GlobStruct*)pCore;
}

// List of DLL's that this plug-in depends on
// I've added some others just in case but originally only had Basic3D, Image and Basic2D

const char* Dependencies[] =
{
	//"DBProCore.dll",
	//"DBProBitmapDebug.dll",
    "DBProBasic3DDebug.dll",
	"DBProImageDebug.dll",
	//"DBProBasic2DDebug.dll"
};


EXPORT int GetNumDependencies( void )
{
    return sizeof(Dependencies) / sizeof(const char*);
}


EXPORT const char* GetDependencyID(int ID)
{
    return Dependencies[ID];
}

// missing function declaration

typedef sFrame* (__cdecl * dbCreateNewFrame0) ( sObject* pObject, LPSTR pName, bool bNewMesh );

sFrame* WL_dbCreateNewFrame( sObject* pObject, LPSTR pName, bool bNewMesh )
{
	static dbCreateNewFrame0 fPtr = 0;

	if ( fPtr == 0 )
		fPtr = (dbCreateNewFrame0) GetProcAddress(
			GetModuleHandle("DBProBasic3DDebug.dll"),
			"?CreateNewFrame@@YAPAUsFrame@@PAUsObject@@PAD_N@Z" );

	return fPtr(pObject, pName, bNewMesh);
}