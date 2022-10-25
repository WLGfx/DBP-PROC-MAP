#include "stdafx.h"
#include "_GDK_PM.h"

// storage for the PROC_MAP (only using the one for now)

_GDK_PM MAP;

// used to select an edge

int		edge_v1, edge_v2;
_VERT	*PM_verts;

EXPORT void pm_set_map( int size, int seed, int height, int tess )
{
	MAP.set_map( size, seed, height );
	MAP.tess = tess;
}

EXPORT void pm_set_corridor( int len_min, int len_max, int ang_min, int ang_max, int seg_min, int seg_max )
{
	MAP.set_corridor( len_min, len_max, ang_min, ang_max, seg_min, seg_max );
}

EXPORT void pm_set_room( int len_min, int len_max, int seg_min, int seg_max )
{
	MAP.set_room( len_min, len_max, seg_min, seg_max );
}

EXPORT void pm_sel_in_edge( int seg, int edge )
{
	seg		%= MAP.map_size;
	edge	%= MAP.pSegList[seg].iedges.size();

	//PM_edge	 = &MAP.pSegList[seg].iedges[edge];

	edge_v1 = MAP.pSegList[seg].iedges[edge].v1;
	edge_v2 = MAP.pSegList[seg].iedges[edge].v2;

	PM_verts = &MAP.pSegList[seg].verts[0];
}

EXPORT void pm_sel_out_edge( int seg, int edge )
{
	seg		%= MAP.map_size;
	edge	%= MAP.pSegList[seg].oedges.size();

	//PM_edge  = &MAP.pSegList[seg].oedges[edge];

	edge_v1 = MAP.pSegList[seg].oedges[edge].v1;
	edge_v2 = MAP.pSegList[seg].oedges[edge].v2;

	PM_verts = &MAP.pSegList[seg].verts[0];
}

EXPORT int pm_get_in_count( int seg )
{
	seg %= MAP.map_size;
	return MAP.pSegList[seg].iedges.size();
}

EXPORT int pm_get_out_count( int seg )
{
	seg %= MAP.map_size;
	return MAP.pSegList[seg].oedges.size();
}

EXPORT void pm_texture_corridor( char* floor, char* wall, char* roof )
{
	MAP.GDK_corridor_textures( floor, wall, roof );
}

EXPORT void pm_texture_room( char* floor, char* wall, char* roof )
{
	MAP.GDK_room_textures( floor, wall, roof );
}

EXPORT DWORD pm_get_object()
{
	return MAP.GDK_get_object();
}

EXPORTC DWORD pm_edge_centre_x()
{
	float result = ( PM_verts[edge_v1].x + PM_verts[edge_v2].x ) / 2.0f;
	return DBPro::CastToFloat( result );
}

EXPORTC DWORD pm_edge_centre_y()
{
	float result = ( PM_verts[edge_v1].y + PM_verts[edge_v2].y ) / 2.0f;
	return DBPro::CastToFloat( result );
}

EXPORTC DWORD pm_edge_centre_z()
{
	float result = ( PM_verts[edge_v1].z + PM_verts[edge_v2].z ) / 2.0f;
	return DBPro::CastToFloat( result );
}

EXPORT void pm_generate()
{
	MAP.GDK_generate();
}

EXPORTC void pm_clear_all()
{
	MAP.GDK_clear_all();
}

//
// additions
//

EXPORTC DWORD pm_edge_angle()
{
	float zdist = PM_verts[edge_v2].z - PM_verts[edge_v1].z;
	float xdist = PM_verts[edge_v2].x - PM_verts[edge_v1].x;
	float result = Rad2Deg( atan2( zdist, xdist ) );
	return DBPro::CastToFloat( result );
}

EXPORTC DWORD pm_max_size()
{
	return MAP.size_max;
}

EXPORTC void pm_texture_corridor_ids( int floor, int wall, int roof )
{
	MAP.textures[0] = floor;
	MAP.textures[1] = wall;
	MAP.textures[2] = roof;
}

EXPORTC void pm_texture_room_ids( int floor, int wall, int roof )
{
	MAP.textures[3] = floor;
	MAP.textures[4] = wall;
	MAP.textures[5] = roof;
}

EXPORTC void pm_texture_segment_floor( int seg, int texture )
{
	seg %= MAP.map_size;
	DBPro::TextureLimb( MAP.ObjID, seg, texture );
}

EXPORTC void pm_texture_segment_wall( int seg, int texture )
{
	seg %= MAP.map_size;
	DBPro::TextureLimb( MAP.ObjID, seg + MAP.map_size, texture );
}

EXPORTC void pm_texture_segment_roof( int seg, int texture )
{
	seg %= MAP.map_size;
	DBPro::TextureLimb( MAP.ObjID, seg + (MAP.map_size << 1), texture );
}

EXPORTC int pm_segment_type( int seg )
{
	seg %= MAP.map_size;
	return MAP.pSegList[seg].faces[0].type;
}

EXPORTC void pm_color_segment_floor( int seg, DWORD color )
{
	seg %= MAP.map_size;
	DBPro::ColorLimb( MAP.ObjID, seg, color );
}

EXPORTC void pm_color_segment_wall( int seg, DWORD color )
{
	seg %= MAP.map_size;
	DBPro::ColorLimb( MAP.ObjID, seg + MAP.map_size, color );
}

EXPORTC void pm_color_segment_roof( int seg, DWORD color )
{
	seg %= MAP.map_size;
	DBPro::ColorLimb( MAP.ObjID, seg + (MAP.map_size << 1), color );
}