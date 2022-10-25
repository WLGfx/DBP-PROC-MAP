========================================================================
    DYNAMIC LINK LIBRARY : PROC_MAP Project Overview
========================================================================

AppWizard has created this PROC_MAP DLL for you.

This file contains a summary of what you will find in each of the files that
make up your PROC_MAP application.


PROC_MAP.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

PROC_MAP.cpp
    This is the main DLL source file.

	When created, this DLL does not export any symbols. As a result, it
	will not produce a .lib file when it is built. If you wish this project
	to be a project dependency of some other project, you will either need to
	add code to export some symbols from the DLL so that an export library
	will be produced, or you can set the Ignore Input Library property to Yes
	on the General propert page of the Linker folder in the project's Property
	Pages dialog box.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named PROC_MAP.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////

HELP POSTED TO THE TGC FORUMS:

Plugin problem:

I've been working on my procedural 3D level generator for a long time now and just recently got it to work perfect with Dark GDK (V1.0). So, once it was working in Dark GDK, it was time to turn it into a plugin for the Dark Basic Pro community.

The Visual Studio 2008 Express C++ setup for a DLL with the default project settings I used, with the Multi-Byte settings changed and also attached the lib and the include dirs for the DBP plugin header files from IanM.

At first, when writing a sample dba file to test the plugin I'd immediatly hit 'Error 501: Something about an image'. So I put checks in my code to test the stages it was running at and it got past the first function, the setup. I also took out a vector with held an array of objects for the plugin for future use. I even ditched the lot and started the DLL plugin project another 2 times.

What is actually happening now is the when I go to compile a project from the DBP IDE (default editor), it will freeze and then complain it cannot write the exe file. So I loaded up some old DBP projects and they compile and run just fine.

In case the problem can be spotted from here I'll post some of the source files. If not I can email the project over for someone to take a look at. I'm pulling my hair out again and really want to release this plugin as soon as but I'm flummoxed.

stdafx.h [code]// stdafx.h : include file for standard system include files,
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

sFrame* dbCreateNewFrame( sObject* pObject, LPSTR pName, bool bNewMesh );

// external references

extern GlobStruct *g_pGlob;[/code]

Standard.cpp [code]#include "stdafx.h"

//
// see: http://forum.thegamecreators.com/?m=forum_view&t=190224&b=18
// for more refernces
//

#pragma comment (lib, "DBProInterface9.0.lib")
//#pragma comment (lib, "comctl32.lib")

GlobStruct*	g_pGlob;

// Constructor

void Constructor ( void ) { // Create memory here
	//InitCommonControls();
}

// Destructor

EXPORT void PreDestructor(void) { // Early shutdown stuff goes here
}

void Destructor ( void ) { // Free memory here
}

// Get GlobStruct

void ReceiveCoreDataPtr ( LPVOID pCore ) { // Get Core Data Pointer here
	g_pGlob = (GlobStruct*)pCore;
}

// List of DLL's that this plug-in depends on

const char* Dependencies[] =
{
    "DBProBasic3DDebug.dll",
	"DBProImageDebug.dll",
	"DBProBasic2DDebug.dll"
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

sFrame* dbCreateNewFrame( sObject* pObject, LPSTR pName, bool bNewMesh )
{
	static dbCreateNewFrame0 fPtr = 0;

	if ( fPtr == 0 )
		fPtr = (dbCreateNewFrame0) GetProcAddress(
			GetModuleHandle("DBProBasic3DDebug.dll"),
			"?CreateNewFrame@@YAPAUsFrame@@PAUsObject@@PAD_N@Z" );

	return fPtr(pObject, pName, bNewMesh);
}[/code]

dllmain.cpp [code]// dllmain.cpp : Defines the entry point for the DLL application.

#include "stdafx.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}[/code]

resource.h [code]#ifndef IDC_STATIC
	#define IDC_STATIC (-1)
#endif

#define IDS_STRING1		1
#define IDS_STRING2		2
#define IDS_STRING3		3
#define IDS_STRING4		4
#define IDS_STRING5		5
#define IDS_STRING6		6
#define IDS_STRING7		7
#define IDS_STRING8		8
#define IDS_STRING9		9
#define IDS_STRING10	10
#define IDS_STRING11	11
#define IDS_STRING12	12
#define IDS_STRING13	13
#define IDS_STRING14	14
#define IDS_STRING15	15
#define IDS_STRING16	16
#define IDS_STRING17	17
#define IDS_STRING18	18
#define IDS_STRING19	19
#define IDS_STRING20	20
#define IDS_STRING21	21
#define IDS_STRING22	22
#define IDS_STRING23	23
#define IDS_STRING24	24
[/code]

resource.rc [code]#include "resource.h"

STRINGTABLE
{
	// Setup functions

	IDS_STRING1		"PMAP SET MAP DATA%LLLL%pm_set_map%size,seed,height,tess"
	IDS_STRING2		"PMAP SET CORRIDOR%LLLLLL%pm_set_corridor%lmin,lmax,amin,amax,smin,smax"
	IDS_STRING3		"PMAP SET ROOM%LLLL%pm_set_room%lmin,lmax,smin,smax"

	// Edge functions

	IDS_STRING4		"PMAP SEL IN EDGE%LL%pm_sel_in_edge%seg,edge"
	IDS_STRING5		"PMAP SEL OUT EDGE%LL%pm_sel_out_edge%seg,edge"
	IDS_STRING6		"PMAP GET IN COUNT[%LL%pm_get_in_count%seg"
	IDS_STRING7		"PMAP GET OUT COUNT[%LL%pm_get_out_count%seg"
	IDS_STRING8		"PMAP EDGE CENTRE X[%L%pm_edge_centre_x%"
	IDS_STRING9		"PMAP EDGE CENTRE Y[%L%pm_edge_centre_y%"
	IDS_STRING10	"PMAP EDGE CENTRE Z[%L%pm_edge_centre_z%"

	// Setup textures

	IDS_STRING11	"PMAP TEXTURE CORRIDOR%SSS%pm_texture_corridor%floor,wall,roof"
	IDS_STRING12	"PMAP TEXTURE ROOM%SSS%pm_texture_room%floor,wall,roof"

	// Main generate function

	IDS_STRING13	"PMAP GENERATE%0%pm_generate%"

	// Return objects ID number

	IDS_STRING14	"PMAP OBJECT ID[%L%pm_get_object%"
}
[/code]

PROC_MAP.ini [code]PMAP SET MAP DATA==size,seed,height,tessflag
PMAP SET CORRIDOR==lmin,lmax,amin,amax,smin,smax
PMAP SET ROOM==lmin,lmax,smin,smax
PMAP SEL IN EDGE==seg,edge
PMAP SEL OUT EDGE==seg,edge
PMAP GET IN COUNT==(seg)
PMAP GET OUT COUNT==(seg)
PMAP EDGE CENTRE X==()
PMAP EDGE CENTRE Y==()
PMAP EDGE CENTRE Z==()
PMAP TEXTURE CORRIDOR==floor$,wall$,roof$
PMAP TEXTURE ROOM==floor$,wall$,roof$
PMAP GENERATE==*no params*
PMAP OBJECT ID==() [/code]

DBP_functions [code]#include "stdafx.h"

// storage for the PROC_MAP

_GDK_PM MAP;

// used to select an edge

_EDGE *PM_edge = 0;
_VERT *PM_verts = 0;

EXPORTC void pm_set_map( int size, int seed, int height, int tess )
{
	MAP.set_map( size, seed, height );
	MAP.tess = tess;
}

EXPORTC void pm_set_corridor( int len_min, int len_max, int ang_min, int ang_max, int seg_min, int seg_max )
{
	MAP.set_corridor( len_min, len_max, ang_min, ang_max, seg_min, seg_max );
}

EXPORTC void pm_set_room( int len_min, int len_max, int seg_min, int seg_max )
{
	MAP.set_room( len_min, len_max, seg_min, seg_max );
}

EXPORTC void pm_sel_in_edge( int seg, int edge )
{
	seg %= MAP.map_size;
	edge %= MAP.pSegList[seg].iedges.size();
	PM_edge = &MAP.pSegList[seg].iedges[edge];
	PM_verts = &MAP.pSegList[seg].verts[0];
}

EXPORTC void pm_sel_out_edge( int seg, int edge )
{
	seg %= MAP.map_size;
	edge %= MAP.pSegList[seg].oedges.size();
	PM_edge = &MAP.pSegList[seg].oedges[edge];
	PM_verts = &MAP.pSegList[seg].verts[0];
}

EXPORTC int pm_get_in_count( int seg )
{
	seg %= MAP.map_size;
	return MAP.pSegList[seg].iedges.size();
}

EXPORTC int pm_get_out_count( int seg )
{
	seg %= MAP.map_size;
	return MAP.pSegList[seg].oedges.size();
}

EXPORTC void pm_texture_corridor( char* floor, char* wall, char* roof )
{
	MAP.GDK_corridor_textures( floor, wall, roof );
}

EXPORTC void pm_texture_room( char* floor, char* wall, char* roof )
{
	MAP.GDK_room_textures( floor, wall, roof );
}

EXPORTC DWORD pm_get_object()
{
	return MAP.GDK_get_object();
}

EXPORTC DWORD pm_edge_centre_x()
{
	float result = ( PM_verts[PM_edge->v1].x + PM_verts[PM_edge->v2].x ) / 2.0f;
	return DBPro::CastToFloat( result );
}

EXPORTC DWORD pm_edge_centre_y()
{
	float result = ( PM_verts[PM_edge->v1].y + PM_verts[PM_edge->v2].y ) / 2.0f;
	return DBPro::CastToFloat( result );
}

EXPORTC DWORD pm_edge_centre_z()
{
	float result = ( PM_verts[PM_edge->v1].z + PM_verts[PM_edge->v2].z ) / 2.0f;
	return DBPro::CastToFloat( result );
}

EXPORTC void pm_generate()
{
	MAP.GDK_generate();
}
[/code]

The other four files are the main PROC_MAP code with all the GDK function calls changed to DBP calls, ie from _GDK_PM.cpp [code]void _GDK_PM::GDK_build_object()
{
	iLimbCount = map_size * 3;
	ObjID = GDK_free_object();

	DBPro::CreateNewObject( ObjID, "", 1 );
	pObject = DBPro::GetObjectData( ObjID );

	DBO_flag = false;

	GDK_dbo_floor();
	GDK_dbo_wall();
	GDK_dbo_roof();

	DBPro::SetNewObjectFinalProperties( ObjID, 10.0f );
	DBPro::TextureObject( ObjID, 0 );

	int loop, type;
	int wall_off = map_size;
	int roof_off = wall_off + map_size;

	for ( loop = 0; loop < map_size; loop ++ )
	{
		type = pSegList[loop].faces[0].type;

		int limb = loop;

		if ( type == PM_RFACE )
		{
			if ( textures[0] ) DBPro::TextureLimb( ObjID, limb, textures[0] );
			else				 DBPro::ColorLimb( ObjID, limb, 0x888888 );
			if ( textures[1] ) DBPro::TextureLimb( ObjID, limb + wall_off, textures[1] );
			else				 DBPro::ColorLimb( ObjID, limb + wall_off, 0xff8800 );
			if ( textures[2] ) DBPro::TextureLimb( ObjID, limb + roof_off, textures[2] );
			else				 DBPro::ColorLimb( ObjID, limb + roof_off, 0xff0000 );
		}
		else
		{
			if ( textures[3] ) DBPro::TextureLimb( ObjID, limb, textures[3] );
			else				 DBPro::ColorLimb( ObjID, limb, 0x00ffff );
			if ( textures[4] ) DBPro::TextureLimb( ObjID, limb + wall_off, textures[4] );
			else				 DBPro::ColorLimb( ObjID, limb + wall_off, 0x00ff88 );
			if ( textures[5] ) DBPro::TextureLimb( ObjID, limb + roof_off, textures[5] );
			else				 DBPro::ColorLimb( ObjID, limb + roof_off, 0x00ff00 );
		}
	}
}[/code]

I've also checked many times that #include "DarkGDK.h" is not there and it isn't...

Please someone help me get this finished.

I'll email the entire project if anyone would like to check it out.

///////////////////////////////////////////////////////////////////////////////

WIN32
NDEBUG
_WINDOWS
_USRDLL
PROC_MAP_EXPORTS