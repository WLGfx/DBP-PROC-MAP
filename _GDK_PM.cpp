#include "stdafx.h"

_GDK_PM::_GDK_PM(void)
{
	ObjID = 0;

	memset( textures, 0, 6 * sizeof(int) );

	pSegList.clear();
	pVertList = NULL;
	tedges.clear();
	tverts.clear();
}

_GDK_PM::~_GDK_PM(void)
{
	// to save on a memory leak

	if ( pVertList ) { delete[] pVertList; pVertList = 0; }
}

void _GDK_PM::GDK_clear_all()
{
	// delete image textures

	for ( int loop = 0; loop < 6; loop++ )
	{
		if ( DBPro::ImageExist( textures[loop] ) )
		{
			DBPro::DeleteImage( textures[loop] );
		}
	}

	// remove the segments lists

	pSegList.clear();
	tedges.clear();
	tverts.clear();

	if ( pVertList ) { delete[] pVertList; pVertList = 0; }

	if ( ObjID ) DBPro::DeleteObject( ObjID );
}

//
// EXTRA FUNCTIONS
//

// fast square root function
inline float fastSqrt_Bab(const float x)  
{
	union
	{
		int i;
		float x;
	} u;
	u.x = x;
	u.i = (1<<29) + (u.i >> 1) - (1<<22); 

	// One Babylonian Step
	u.x = 0.5f * (u.x + x/u.x);

	return u.x;
}

//
// http://code.google.com/p/darkbasicpro/source/browse/trunk/%20darkbasicpro%20--username%20LeeBamberTGC/Dark%20Basic%20Pro%20SDK/Shared/Objects/CommonC.h
//

//bool CreateNewObject            ( int iID, LPSTR pName, int iFrame );
//sFrame* CreateNewFrame          ( sObject* pObject, LPSTR pName, bool bNewMesh );
//bool SetupStandardVertex        ( DWORD dwFVF, BYTE* pVertex, int iOffset, float x, float y, float z, float nx, float ny, float nz, DWORD dwDiffuseColour, float tu, float tv );
//bool SetupMeshFVFData           ( sMesh* pMesh, DWORD dwFVF, DWORD dwVertexCount, DWORD dwIndexCount );
//bool SetNewObjectFinalProperties( int iID, float fRadius );
//void SetTexture                 ( int iID, int iImage );

//
// PROC MAP generate GDK DBO
//
// Will create a single object with limbs
// LimbCount = Segments * 3 (floor, wall, roof)
//

int _GDK_PM::GDK_free_object()
{
	int		c = 1 + ( rand() % 0xffff );

	while ( DBPro::ObjectExist( c ) )
		c++;

	return c;
}

int _GDK_PM::GDK_free_image()
{
	int		c = 1 + ( rand() % 0xffff );

	while ( DBPro::ImageExist( c ) )
		c++;

	return c;
}

void _GDK_PM::GDK_corridor_textures(char *floor, char *wall, char *roof)
{
	DBPro::LoadImage( floor, textures[0] = GDK_free_image() );
	DBPro::LoadImage( wall,  textures[1] = GDK_free_image() );
	DBPro::LoadImage( roof,  textures[2] = GDK_free_image() );

	if ( !DBPro::ImageExist( textures[0] ) ) textures[0] = 0;
	if ( !DBPro::ImageExist( textures[1] ) ) textures[1] = 0;
	if ( !DBPro::ImageExist( textures[2] ) ) textures[2] = 0;
}

void _GDK_PM::GDK_room_textures(char *floor, char *wall, char *roof)
{
	DBPro::LoadImage( floor, textures[3] = GDK_free_image() );
	DBPro::LoadImage( wall,  textures[4] = GDK_free_image() );
	DBPro::LoadImage( roof,  textures[5] = GDK_free_image() );

	if ( !DBPro::ImageExist( textures[3] ) ) textures[3] = 0;
	if ( !DBPro::ImageExist( textures[4] ) ) textures[4] = 0;
	if ( !DBPro::ImageExist( textures[5] ) ) textures[5] = 0;
}

void _GDK_PM::GDK_build_seglists()
{
	// Create temp vert reference list

	if ( pVertList ) { delete[] pVertList; pVertList = 0; }
	pVertList = new int[verts.size()];

	// setup the segment lists

	pSegList.clear();
	pSegList.resize(map_size);

	// loop through the segments

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		// clear the temp vert reference list

		memset( pVertList, 0, verts.size() * sizeof(int) );

		// clear segments verts, edges and faces lists

		pSegList[curr_seg].verts.clear();
		//pSegList[curr_seg].iedges.clear();
		//pSegList[curr_seg].oedges.clear();
		pSegList[curr_seg].edges.clear();
		pSegList[curr_seg].faces.clear();

		// run through the edge list

		for ( int curr_edge = 0; curr_edge < edges.size(); curr_edge++ )
		{
			// same as curr_seg?

			if ( curr_seg == edges[curr_edge].segment )
			{
				int		v1 = edges[curr_edge].v1;
				int		v2 = edges[curr_edge].v2;

				// check existence of the verts

				v1 = add_edge_vert( v1 );
				v2 = add_edge_vert( v2 );

				// add the new edge to the segment (CHANGED to edges[])

				int etype = edges[curr_edge].type;

				//if ( etype == PM_CINNER || etype == PM_RINNER || etype == PM_RENTRY )
				//{
				//	// add edge to the inner list only

				//	pSegList[curr_seg].iedges.push_back( _EDGE( v1, v2, etype, curr_seg ) );
				//}
				//else
				//{
				//	// add edge to the outer list only

				//	pSegList[curr_seg].oedges.push_back( _EDGE( v1, v2, etype, curr_seg ) );
				//}

				// Just push onto a single list

				pSegList[curr_seg].edges.push_back( _EDGE(v1,v2,etype,curr_seg) );
			}
		} // EOF for ( int curr_edge

		// run through the face list

		for ( int curr_face = 0; curr_face < faces.size(); curr_face++ )
		{
			// same as curr_seg?
			int seg = faces[curr_face].segment;

			if ( curr_seg == seg )
			{
				// get the vert references (checking whether they exist too in case of bugs)
				// NB: Should be able to remove this check!!!

				int v1 = add_edge_vert( faces[curr_face].v1 );
				int v3 = add_edge_vert( faces[curr_face].v2 );
				int v2 = add_edge_vert( faces[curr_face].v3 );

				// Add the face to the segments face list
				pSegList[curr_seg].faces.push_back( _FACE( v1, v2, v3, faces[curr_face].type, curr_seg ) );
			}

		} // EOF for ( curr_face

	} // EOF for ( curr_seg

	// clear the temp vert reference list

	delete[] pVertList;
	pVertList = 0;
}

int _GDK_PM::add_edge_vert( int v )
{
	// has the vert already been added? then return the new reference
	
	if ( pVertList[v] != 0 ) return pVertList[v] - 1;

	// grab the coords for the bounds checking

	float x = verts[v].x;
	float y = verts[v].y;
	float z = verts[v].z;

	// first vert?

	int vpos = pSegList[curr_seg].verts.size();

	if ( vpos == 0 )
	{
		// set bounding box to the start values

		pSegList[curr_seg].xmin = pSegList[curr_seg].xmax = x;
		//pSegList[curr_seg].ymin = pSegList[curr_seg].ymax = y;
		pSegList[curr_seg].zmin = pSegList[curr_seg].zmax = z;
	}
	else
	{
		// check x on the bounding box

		if ( x < pSegList[curr_seg].xmin ) pSegList[curr_seg].xmin = x;
		if ( x > pSegList[curr_seg].xmax ) pSegList[curr_seg].xmax = x;

		// check y on the bounding box

		//if ( y < pSegList[curr_seg].ymin ) pSegList[curr_seg].ymin = y;
		//if ( y > pSegList[curr_seg].ymax ) pSegList[curr_seg].ymax = y;

		// check z on the bounding box

		if ( z < pSegList[curr_seg].zmin ) pSegList[curr_seg].zmin = z;
		if ( z > pSegList[curr_seg].zmax ) pSegList[curr_seg].zmax = z;
	}

	// set the new reference in the temp list and add 1

	pVertList[v] = vpos + 1;

	// add the vert to the segments verts list

	pSegList[curr_seg].verts.push_back( _VERT(x,y,z) );

	// exit and return the new vert reference

	return vpos;
}

// GDK_create_roof()
//
// Can be optimised using memcpy()

void _GDK_PM::GDK_create_roof()
{
	int		loop;
	float	x, y, z, s, t;
	int		v1, v2, v3, type, segment;

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		// make sure the new vert list is empty

		pSegList[curr_seg].rverts.clear();

		for ( loop = 0; loop < pSegList[curr_seg].verts.size(); loop++ )
		{
			// get the current verts coords

			x = pSegList[curr_seg].verts[loop].x;
			y = pSegList[curr_seg].verts[loop].y + size_max;
			z = pSegList[curr_seg].verts[loop].z;

			// calculate UV coords

			s = x / size_max;
			t = z / size_max;

			// add the vert

			pSegList[curr_seg].rverts.push_back( _VERT( x, y, z ) );

			// set the UV coords for both the floor and the roof

			pSegList[curr_seg].verts[loop].setUV( s, t );
			pSegList[curr_seg].rverts[loop].setUV( s, t );
		}

		// copy the inner edges over now (CHANGED to just edges[])

		// NB: copy these over to their own roof inner and outer edge lists

		//for ( loop = 0; loop < pSegList[curr_seg].iedges.size(); loop++ )
		//{
		//	v1		= pSegList[curr_seg].iedges[loop].v1;
		//	v2		= pSegList[curr_seg].iedges[loop].v2;
		//	type	= pSegList[curr_seg].iedges[loop].type;
		//	segment	= pSegList[curr_seg].iedges[loop].segment;

		//	// separate lists so not changing the type anymore

		//	pSegList[curr_seg].redges.push_back( _EDGE(v1,v2,type,segment) );
		//}

		//// copy the outer edges over to the same list
		//// as the floor edges are the ones used for positioning

		//for ( loop = 0; loop < pSegList[curr_seg].oedges.size(); loop++ )
		//{
		//	v1		= pSegList[curr_seg].oedges[loop].v1;
		//	v2		= pSegList[curr_seg].oedges[loop].v2;
		//	type	= pSegList[curr_seg].oedges[loop].type;
		//	segment	= pSegList[curr_seg].oedges[loop].segment;

		//	pSegList[curr_seg].redges.push_back( _EDGE(v1,v2,type,segment) );
		//}

		// Just copy the edges list now

		for ( loop = 0; loop < pSegList[curr_seg].edges.size(); loop++ )
		{
			v1		= pSegList[curr_seg].edges[loop].v1;
			v2		= pSegList[curr_seg].edges[loop].v2;
			type	= pSegList[curr_seg].edges[loop].type;
			segment	= pSegList[curr_seg].edges[loop].segment;

			pSegList[curr_seg].redges.push_back( _EDGE(v1,v2,type,segment) );
		}

		// copy the face lists and reverse orientation

		for ( loop = 0; loop < pSegList[curr_seg].faces.size(); loop++ )
		{
			v1		= pSegList[curr_seg].faces[loop].v1;
			v2		= pSegList[curr_seg].faces[loop].v2;
			v3		= pSegList[curr_seg].faces[loop].v3;
			type	= pSegList[curr_seg].faces[loop].type;
			segment	= pSegList[curr_seg].faces[loop].segment;

			// change the orientation

			pSegList[curr_seg].rfaces.push_back( _FACE(v1,v3,v2,type,segment) );
		}

	} // EOF for ( curr_seg
}

void _GDK_PM::GDK_create_wall()
{
	// 1. make temp list out of all outer edges only
	// 2. sort them
	// 3. create temp vert list
	// 4. create edge UV coords in the temp verts list
	// 5. create segment edge and vert lists
	// 6. remove temp edge list and verts list
	// 7. create faces

	// Ensure this function is called before creating the roof

	// 1. make temp edge list

	tedges.clear();
	int loop;

	for ( loop = 0; loop < edges.size(); loop++ )
	{
		// is current edge an outer edge (ignoring PM_RENTRY)
		
		int		v1, v2;
		int		type = edges[loop].type;
		int		segment;

		if ( type == PM_ROUTER || type == PM_COUTER )
		{
			v1 = edges[loop].v1;
			v2 = edges[loop].v2;
			segment = edges[loop].segment;

			tedges.push_back( _EDGE(v1,v2,type,segment) );
		}
	} // EOF for ( loop

	// 2. sort the edge list

	int		v1;
	int		v2;
	bool	found;
	int		iloop;

	for ( loop = 0; loop < tedges.size() - 1; loop++ )
	{
		v2 = tedges[loop].v2;
		found = false;

		iloop = loop + 1;

		while ( iloop < tedges.size() && found == false )
		{
			// same vert? (edited to check coords instead)

			v1 = tedges[iloop].v1;

			if ( verts[v2].x == verts[v1].x &&
				 verts[v2].z == verts[v1].z )
			{
				found = true;

				//if ( iloop != loop + 1 )
				{
					// swap the edges over

					swap( tedges[iloop], tedges[loop+1] );	// hmmm, handy that
				}
			}

			iloop++;
		}

	} // EOF for ( loop

	// 3. create temp vert list

	//std::vector<_VERT> tverts;

	tverts.clear();

	if ( pVertList ) { delete[] pVertList; pVertList = 0; }
	pVertList = new int[verts.size()];

	memset( pVertList, 0, verts.size() * sizeof(int) );

	for ( loop = 0; loop < tedges.size(); loop++ )
	{
		v1 = tedges[loop].v1;
		v2 = tedges[loop].v2;

		// process first vert

		if ( pVertList[v1] ) v1 = pVertList[v1] - 1;
		else
		{
			pVertList[v1] = tverts.size() + 1;
			tverts.push_back( _VERT(verts[v1].x, verts[v1].y, verts[v1].z) );
			v1 = tverts.size() - 1;
		}

		// process second vert

		if ( pVertList[v2] ) v2 = pVertList[v2] - 1;
		else
		{
			pVertList[v2] = tverts.size() + 1;
			tverts.push_back( _VERT(verts[v2].x, verts[v2].y, verts[v2].z) );
			v2 = tverts.size() - 1;
		}

		// now change edges vert references

		tedges[loop].v1 = v1;
		tedges[loop].v2 = v2;
	}

	// 4. create UV coords along the new outer edges

	float UV = 0.0f;
	float edge_len;

	// set first edges start UV (only the s UV coord until the wall faces are created)

	v1 = tedges[0].v1;
	tverts[v1].s = UV;

	for ( loop = 0; loop < tedges.size(); loop++ )
	{
		// set first edges start UV (only the s UV coord until the wall faces are created)

		v1 = tedges[loop].v1;
		tverts[v1].setUV( UV, 1.0f );

		// get next vert reference

		v2 = tedges[loop].v2;

		// calc distance

		float xd = tverts[v1].x - tverts[v2].x;
		float zd = tverts[v1].z - tverts[v2].z;

		edge_len = fastSqrt_Bab( xd * xd + zd * zd );

		// set the new UV according to max edge length from setup vars

		UV += edge_len / size_max;

		// set the UV s of v2 of current edge

		tverts[v2].setUV( UV, 1.0f );
	}

	// 5. create the segments for the walls

	// resize the pVertList verts reference list to the new temp verts list

	if ( pVertList ) { delete[] pVertList; pVertList = 0; }
	pVertList = new int[tverts.size()];

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		// clear the verts reference list

		memset( pVertList, 0, tverts.size() * sizeof(int) );

		// double check wverts[] and wedges[] are clear?

		//pSegList[curr_seg].wverts.clear();
		//pSegList[curr_seg].wedges.clear();

		for ( loop = 0; loop < tedges.size(); loop++ )
		{
			// is it the same segment

			if ( tedges[loop].segment == curr_seg )
			{
				// get the vertex references (add them to the verts list if not already)

				v1 = check_wall_seg_vert( tedges[loop].v1 );
				v2 = check_wall_seg_vert( tedges[loop].v2 );

				// now add the edge to the list

				pSegList[curr_seg].wedges.push_back( _EDGE(v1,v2,tedges[loop].type,curr_seg) );
			}
		}
	} // EOF for ( curr_seg

	// 6. remove the temp verts and edge lists (all done with them)

	if ( pVertList ) { delete[] pVertList; pVertList = 0; }
	tverts.clear();
	tedges.clear();

	// 7. create the faces for the wall

	// at this point now I won't be adding any more edges to the walls edge list
	// as any calls to return coordinates can be referenced from what's already in
	// the list (ie the ground edges)

	// for the other 2 verts added

	int v3, v4;
	int type;
	int loop_size;

	// for the verts coordinates

	float x, y, z, s, t;

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		// get the type of wall for adding the faces

		type = pSegList[curr_seg].wedges[0].type;

		loop_size = pSegList[curr_seg].wedges.size();

		for ( loop = 0; loop < loop_size; loop++ )
		{
			// grab the vert references for the edge on the ground

			v1 = pSegList[curr_seg].wedges[loop].v1;
			v2 = pSegList[curr_seg].wedges[loop].v2;

			// set up v3 vert

			v3 = pSegList[curr_seg].wverts.size();
			pSegList[curr_seg].wverts.resize( v3 + 1 );
			pSegList[curr_seg].wverts[v3].set(  pSegList[curr_seg].wverts[v1].x,
												pSegList[curr_seg].wverts[v1].y + size_max,
												pSegList[curr_seg].wverts[v1].z );
			pSegList[curr_seg].wverts[v3].setUV(pSegList[curr_seg].wverts[v1].s, 0.0f );

			// Set up v4 vert

			v4 = v3 + 1;
			pSegList[curr_seg].wverts.resize( v4 + 1 );
			pSegList[curr_seg].wverts[v4].set(  pSegList[curr_seg].wverts[v2].x,
												pSegList[curr_seg].wverts[v2].y + size_max,
												pSegList[curr_seg].wverts[v2].z );
			pSegList[curr_seg].wverts[v4].setUV(pSegList[curr_seg].wverts[v2].s, 0.0f );

			// v1, v2, v3 & v4 done, now add the two faces

			pSegList[curr_seg].wfaces.push_back( _FACE(v1,v4,v3,type,curr_seg) );
			pSegList[curr_seg].wfaces.push_back( _FACE(v1,v2,v4,type,curr_seg) );

			// now add the edges (must be done for tessellation)

			pSegList[curr_seg].wedges.push_back( _EDGE(v1,v4,type,curr_seg) );
			pSegList[curr_seg].wedges.push_back( _EDGE(v4,v3,type,curr_seg) );
			pSegList[curr_seg].wedges.push_back( _EDGE(v4,v2,type,curr_seg) );
			pSegList[curr_seg].wedges.push_back( _EDGE(v3,v1,type,curr_seg) );
		}
	} // EOF for ( curr_seg
}

int _GDK_PM::check_wall_seg_vert(int v)
{
	// has vert already been referenced?
	
	if ( pVertList[v] ) return pVertList[v] - 1;

	// add new vert and set it's reference

	int vpos = pSegList[curr_seg].wverts.size();

	pSegList[curr_seg].wverts.push_back( _VERT( tverts[v].x, tverts[v].y, tverts[v].z ) );
	pSegList[curr_seg].wverts[vpos].setUV( tverts[v].s, 1.0f );

	pVertList[v] = vpos + 1;

	return vpos;
}

//
// GDK_calc_normals();
//

void _GDK_PM::GDK_calc_normals()
{
	int loop;

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		// clear the floor normals

		for ( loop = 0; loop < pSegList[curr_seg].verts.size(); loop++ )
		{
			pSegList[curr_seg].verts[loop].setNormals(0.0f, 0.0f, 0.0f);
		}

		// clear the wall normals

		for ( loop = 0; loop < pSegList[curr_seg].wverts.size(); loop++ )
		{
			pSegList[curr_seg].wverts[loop].setNormals(0.0f, 0.0f, 0.0f);
		}

		// clear the roof normals

		for ( loop = 0; loop < pSegList[curr_seg].rverts.size(); loop++ )
		{
			pSegList[curr_seg].rverts[loop].setNormals(0.0f, 0.0f, 0.0f);
		}

		// creat temp counters for each segment piece, floor, wall and roof

		int *floor_count = new int[pSegList[curr_seg].verts.size()];
		int * wall_count = new int[pSegList[curr_seg].wverts.size()];
		int * roof_count = new int[pSegList[curr_seg].rverts.size()];

		// set the memory to 0

		memset( floor_count, 0, pSegList[curr_seg].verts.size()  * sizeof(int) );
		memset ( wall_count, 0, pSegList[curr_seg].wverts.size() * sizeof(int) );
		memset ( roof_count, 0, pSegList[curr_seg].rverts.size() * sizeof(int) );

		// add face normals to the verts normals and increment counter after each face

		int v1, v2, v3;
		float nx, ny, nz;

		// floor normals

		for ( loop = 0; loop < pSegList[curr_seg].faces.size(); loop++ )
		{
			// get vert references for the face

			v1 = pSegList[curr_seg].faces[loop].v1;
			v2 = pSegList[curr_seg].faces[loop].v2;
			v3 = pSegList[curr_seg].faces[loop].v3;

			// add the face normal to the verts normal

			add_normal(v1,v2,v3,&pSegList[curr_seg].verts[0]);

			// increment the verts normal count

			floor_count[v1]++;
			floor_count[v2]++;
			floor_count[v3]++;
		}

		for ( loop = 0; loop < pSegList[curr_seg].verts.size(); loop++ )
		{
			div_normal( &pSegList[curr_seg].verts[loop], floor_count[loop] );
		}

		// wall normals

		for ( loop = 0; loop < pSegList[curr_seg].wfaces.size(); loop++ )
		{
			v1 = pSegList[curr_seg].wfaces[loop].v1;
			v2 = pSegList[curr_seg].wfaces[loop].v2;
			v3 = pSegList[curr_seg].wfaces[loop].v3;

			add_normal(v1,v2,v3,&pSegList[curr_seg].wverts[0]);

			wall_count[v1]++;
			wall_count[v2]++;
			wall_count[v3]++;
		}

		for ( loop = 0; loop < pSegList[curr_seg].wverts.size(); loop++ )
		{
			div_normal( &pSegList[curr_seg].wverts[loop], wall_count[loop] );
		}

		// roof normals

		for ( loop = 0; loop < pSegList[curr_seg].rfaces.size(); loop++ )
		{
			v1 = pSegList[curr_seg].rfaces[loop].v1;
			v2 = pSegList[curr_seg].rfaces[loop].v2;
			v3 = pSegList[curr_seg].rfaces[loop].v3;

			add_normal(v1,v2,v3,&pSegList[curr_seg].rverts[0]);

			roof_count[v1]++;
			roof_count[v2]++;
			roof_count[v3]++;
		}

		for ( loop = 0; loop < pSegList[curr_seg].rverts.size(); loop++ )
		{
			div_normal( &pSegList[curr_seg].rverts[loop], roof_count[loop] );
		}

		// clear the temp counter lists

		delete[] floor_count;
		delete[]  wall_count;
		delete[]  roof_count;

	} // EOF for ( curr_seg
}

void _GDK_PM::div_normal( _VERT *vert, int div )
{
	vert->nx /= (float)div; vert->ny /= (float)div; vert->nz /= (float)div;
}

void _GDK_PM::add_normal(int v1, int v2, int v3, _VERT *vlist)
{
	float x1 = vlist[v1].x, y1 = vlist[v1].y, z1 = vlist[v1].z;
	float x2 = vlist[v2].x, y2 = vlist[v2].y, z2 = vlist[v2].z;
	float x3 = vlist[v3].x, y3 = vlist[v3].y, z3 = vlist[v3].z;

	float normx = (z1-z2)*(y3-y2)-(y1-y2)*(z3-z2);
	float normy = (x1-x2)*(z3-z2)-(z1-z2)*(x3-x2);
	float normz = (y1-y2)*(x3-x2)-(x1-x2)*(y3-y2);

	float normlength = fastSqrt_Bab((normx*normx)+(normy*normy)+(normz*normz));

	normx /= normlength;
	normy /= normlength;
	normz /= normlength;

	vlist[v1].nx += normx; vlist[v1].ny += normy; vlist[v1].nz += normz;
	vlist[v2].nx += normx; vlist[v2].ny += normy; vlist[v2].nz += normz;
	vlist[v3].nx += normx; vlist[v3].ny += normy; vlist[v3].nz += normz;
}

// NOTES: Limbs for floor, wall and roof sections
//
// 0 to (map_size-1)					= all floor segments
// map_size to (map_size+map_size-1)	= all wall segments
// (map_size+map_size) to (map_size*3-1)= all roof segments

void _GDK_PM::GDK_build_object()
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
}

void _GDK_PM::GDK_dbo_floor()
{
	int iVerts, iIndices, loop;

	sFrame *pFrame = 0;

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		// get the mesh size

		iVerts	 = pSegList[curr_seg].verts.size();
		iIndices = pSegList[curr_seg].faces.size() * 3;

		// create a new frame? (Not if very first mesh to add)

		if ( DBO_flag == false)
		{
			DBO_flag = true;
			pFrame = pObject->pFrame;
		}
		else
			pFrame = WL_dbCreateNewFrame( pObject, "", true );

		// setup the new mesh data

		sMesh *pMesh = pFrame->pMesh;
		DBPro::SetupMeshFvfData( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, iVerts, iIndices );

		// add verts

		_VERT *v = &pSegList[curr_seg].verts[0];
		int size = pSegList[curr_seg].verts.size();
		
		for ( loop = 0; loop < size; loop++ )
		{
			DBPro::SetupStandardVertex(pMesh->dwFVF, pMesh->pVertexData, loop,
				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0xffffff, v->s, v->t );
			v++;
		}

		// add faces

		size = pSegList[curr_seg].faces.size();
		_FACE * f = &pSegList[curr_seg].faces[0];

		for ( loop = 0; loop < size; loop++ )
		{
			pMesh->pIndices[loop*3]		= f->v1;
			pMesh->pIndices[loop*3+1]	= f->v2;
			pMesh->pIndices[loop*3+2]	= f->v3;
			f++;
		}

		// finalise limb

		pMesh->iPrimitiveType	= D3DPT_TRIANGLELIST;
		pMesh->iDrawVertexCount = iVerts;
		pMesh->iDrawPrimitives	= pSegList[curr_seg].faces.size();
	} // EOF for ( curr_seg
}

void _GDK_PM::GDK_dbo_wall()
{
	int iVerts, iIndices, loop;

	sFrame *pFrame = 0;

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		// get the mesh size

		iVerts	 = pSegList[curr_seg].wverts.size();
		iIndices = pSegList[curr_seg].wfaces.size() * 3;

		// create a new frame

		if ( DBO_flag == false)
		{
			DBO_flag = true;
			pFrame = pObject->pFrame;
		}
		else
			pFrame = WL_dbCreateNewFrame( pObject, "", true );

		// setup the new mesh data

		sMesh *pMesh = pFrame->pMesh;
		DBPro::SetupMeshFvfData( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, iVerts, iIndices );

		// add verts

		_VERT *v = &pSegList[curr_seg].wverts[0];
		int size = pSegList[curr_seg].wverts.size();
		
		for ( loop = 0; loop < size; loop++ )
		{
			DBPro::SetupStandardVertex(pMesh->dwFVF, pMesh->pVertexData, loop,
				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0xffffff, v->s, v->t );
			v++;
		}

		// add faces

		size = pSegList[curr_seg].wfaces.size();
		_FACE * f = &pSegList[curr_seg].wfaces[0];

		for ( loop = 0; loop < size; loop++ )
		{
			pMesh->pIndices[loop*3]		= f->v1;
			pMesh->pIndices[loop*3+1]	= f->v2;
			pMesh->pIndices[loop*3+2]	= f->v3;
			f++;
		}

		// finalise limb

		pMesh->iPrimitiveType	= D3DPT_TRIANGLELIST;
		pMesh->iDrawVertexCount = iVerts;
		pMesh->iDrawPrimitives	= pSegList[curr_seg].wfaces.size();
	} // EOF for ( curr_seg
}

void _GDK_PM::GDK_dbo_roof()
{
	int iVerts, iIndices, loop;

	sFrame *pFrame = 0;

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		// get the mesh size

		iVerts	 = pSegList[curr_seg].rverts.size();
		iIndices = pSegList[curr_seg].rfaces.size() * 3;

		// create a new frame? (Not if very first mesh to add)

		if ( DBO_flag == false)
		{
			DBO_flag = true;
			pFrame = pObject->pFrame;
		}
		else
			pFrame = WL_dbCreateNewFrame( pObject, "", true );

		// setup the new mesh data

		sMesh *pMesh = pFrame->pMesh;
		DBPro::SetupMeshFvfData( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, iVerts, iIndices );

		// add verts

		_VERT *v = &pSegList[curr_seg].rverts[0];
		int size = pSegList[curr_seg].rverts.size();
		
		for ( loop = 0; loop < size; loop++ )
		{
			DBPro::SetupStandardVertex(pMesh->dwFVF, pMesh->pVertexData, loop,
				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0xffffff, v->s, v->t );
			v++;
		}

		// add faces

		size = pSegList[curr_seg].rfaces.size();
		_FACE * f = &pSegList[curr_seg].rfaces[0];

		for ( loop = 0; loop < size; loop++ )
		{
			pMesh->pIndices[loop*3]		= f->v1;
			pMesh->pIndices[loop*3+1]	= f->v2;
			pMesh->pIndices[loop*3+2]	= f->v3;
			f++;
		}

		// finalise limb

		pMesh->iPrimitiveType	= D3DPT_TRIANGLELIST;
		pMesh->iDrawVertexCount = iVerts;
		pMesh->iDrawPrimitives	= pSegList[curr_seg].rfaces.size();
	} // EOF for ( curr_seg
}

void _GDK_PM::GDK_add_noise()
{
	int loop, loop_size;

	float x, z;

	_VERT *v;

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		// process the floor segment

		v = &pSegList[curr_seg].verts[0];
		loop_size = pSegList[curr_seg].verts.size();

		for ( loop = 0; loop < loop_size; loop++ )
		{
			v[loop].y += get_noise( v[loop].x, v[loop].z );
		}

		// process the wall segment

		v = &pSegList[curr_seg].wverts[0];
		loop_size = pSegList[curr_seg].wverts.size();

		for ( loop = 0; loop < loop_size; loop++ )
		{
			v[loop].y += get_noise( v[loop].x, v[loop].z );
		}

		v = &pSegList[curr_seg].rverts[0];
		loop_size = pSegList[curr_seg].rverts.size();

		for ( loop = 0; loop < loop_size; loop++ )
		{
			v[loop].y += get_noise( v[loop].x, v[loop].z );
		}

	} // EOF for ( curr_seg
}

void _GDK_PM::GDK_tessellate()
{
	//int loop;

	//for ( loop = 0; loop < this->tess; loop++ )
	if ( tess )
	{
		for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
		{
			// process floor section

			GDK_tess_section( pSegList[curr_seg].verts, 
							  pSegList[curr_seg].edges, 
							  pSegList[curr_seg].faces );

			// process wall section

			GDK_tess_section( pSegList[curr_seg].wverts, 
							  pSegList[curr_seg].wedges, 
							  pSegList[curr_seg].wfaces );

			// process roof section

			GDK_tess_section( pSegList[curr_seg].rverts, 
							  pSegList[curr_seg].redges, 
							  pSegList[curr_seg].rfaces );

		} // EOF for ( curr_seg
	} // EOF for ( loop
}

void _GDK_PM::GDK_tess_section(std::vector<_VERT> &verts, 
							   std::vector<_EDGE> &edges, 
							   std::vector<_FACE> &faces)
{
	// get the sizes of the arrays

	int		iVSize = verts.size();
	int		iESize = edges.size();
	int		iFSize = faces.size();

	// resize the verts array by adding the edges size (1 extra per edge)

	verts.resize( iVSize + iESize );

	// resize the edges array by doubling it (1 extra per edge)

	edges.resize( iESize + iESize );

	// resize the faces array by multiplying by 4 (3 extra per face)

	faces.resize( iFSize * 4 );

	// assign some vars used in this function

	int		loop, v1, v2, v3, m1, m2, m3, type, segment;

	// run through the edges and create mid verts and new edges

	for ( loop = 0; loop < iESize; loop++ )
	{
		// get the edges vert refs

		v1 = edges[loop].v1;
		v2 = edges[loop].v2;

		// set the new verts mid position

		verts[loop + iVSize].set(		 ( verts[v1].x  + verts[v2].x  ) / 2.0f,
										 ( verts[v1].y  + verts[v2].y  ) / 2.0f,
										 ( verts[v1].z  + verts[v2].z  ) / 2.0f );

		// set the new normals (PROB NOT NEEDED)

		verts[loop + iVSize].setNormals( ( verts[v1].nx + verts[v2].nx ) / 2.0f,
										 ( verts[v1].ny + verts[v2].ny ) / 2.0f,
										 ( verts[v1].nz + verts[v2].nz ) / 2.0f );

		// set the new UV coords (PROB NOT NEEDED)

		verts[loop + iVSize].setUV(		 ( verts[v1].s  + verts[v2].s  ) / 2.0f,
										 ( verts[v1].t  + verts[v2].t  ) / 2.0f);

		// set the new edge

		edges[loop + iESize].set(loop + iVSize, v2, edges[loop].type, edges[loop].segment);
	}

	// run the the faces and create new faces

	for ( loop = 0; loop < iFSize; loop++ )
	{
		// get faces vert refs

		v1 = faces[loop].v1;
		v2 = faces[loop].v2;
		v3 = faces[loop].v3;

		// get the mid vert refs

		m1 = iVSize + GDK_find_midvert( edges, iESize, v1, v2 );
		m2 = iVSize + GDK_find_midvert( edges, iESize, v2, v3 );
		m3 = iVSize + GDK_find_midvert( edges, iESize, v3, v1 );

		// get type and segment to cut down lines of code

		type	= faces[loop].type;
		segment = faces[loop].segment;

		// resize first face

		faces[loop].set(v1,m1,m3);

		// set the top face

		faces[loop + iFSize].set(m1, v2, m2, type, segment);

		// set the right face

		faces[loop + iFSize + iFSize].set(m2, v3, m3, type, segment);

		// set the middle face

		faces[loop + iFSize + iFSize + iFSize].set(m1, m2, m3, type, segment);
	}

	// Now resize the original edges

	for ( loop = 0; loop < iESize; loop++ )
	{
		edges[loop].v2 = loop + iVSize;
	}
}

int _GDK_PM::GDK_find_midvert(std::vector<_EDGE> &edges, int iESize,
							  int v1, int v2)
{
	// vars being used in this function

	int loop, v3, v4;

	// run through to find a matching edge

	for ( loop = 0; loop < iESize; loop++ )
	{
		v3 = edges[loop].v1;
		v4 = edges[loop].v2;

		// check both orientations

		if ( ( v1 == v3 && v2 == v4 ) || 
			 ( v1 == v4 && v2 == v3 ) )
			return loop;
	}

	// fingers crossed it shouldn't get to this point whatsoever
	// because if it does the code will go tits up and I'll know about it
	// especially after running a 200 segment level check (actually even a 5 segment)

	// It gets to this point when more than one level of tessellation is done

	return 0;
}

//
// GDK_generate() - MAIN FUNCTION to generate the 3d map completly
//

void _GDK_PM::GDK_generate()
{
	generate();

	pSegList.resize(map_size);

	GDK_build_seglists();
	GDK_create_wall();
	GDK_create_roof();

	GDK_tessellate();
	GDK_add_noise();

	GDK_calc_normals();
	GDK_build_object();

	GDK_split_edge_lists();		// for easier data access

	GDK_clear_data();			// let's give some memory back to the system
}

void _GDK_PM::GDK_split_edge_lists()
{
	int loop;

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		_EDGE *edges = &pSegList[curr_seg].edges[0];
		
		int edge_size = pSegList[curr_seg].edges.size();

		for ( loop = 0; loop < edge_size; loop++ )
		{
			int		v1 = edges[loop].v1;
			int		v2 = edges[loop].v2;
			int		type = edges[loop].type;
			int		segment = edges[loop].segment;

			if ( type == PM_RINNER || type == PM_CINNER )
				pSegList[curr_seg].iedges.push_back( _EDGE(v1,v2,type,segment) );
			else
				pSegList[curr_seg].oedges.push_back( _EDGE(v1,v2,type,segment) );
		}
	}
}

void _GDK_PM::GDK_clear_data()
{
	// from PROC_MAP

	verts.clear();
	edges.clear();
	faces.clear();

	// from _GDK_PM

	tedges.clear();
	tverts.clear();

	// clear the segments walls and roof lists (leave floor for data retrieval)

	for ( curr_seg = 0; curr_seg < map_size; curr_seg++ )
	{
		pSegList[curr_seg].wverts.clear();
		pSegList[curr_seg].wedges.clear();
		pSegList[curr_seg].wfaces.clear();

		pSegList[curr_seg].rverts.clear();
		pSegList[curr_seg].redges.clear();
		pSegList[curr_seg].rfaces.clear();
	}
}

////////////////////////////////////////////////////////////////////////////////
//
// GET DATA FUNCTIONS
//
////////////////////////////////////////////////////////////////////////////////

void _GDK_PM::GDK_centre_in_edge(int seg, int edge, float *x, float *y, float *z)
{
	// wrap seg and edge just in case

	seg  %= map_size;
	edge %= pSegList[seg].iedges.size();

	_VERT	*verts = &pSegList[seg].verts[0];
	_EDGE	*edges = &pSegList[seg].iedges[0];

	int		v1 = edges[edge].v1;
	int		v2 = edges[edge].v2;

	*x = ( verts[v1].x + verts[v2].x ) / 2.0f;
	*y = ( verts[v1].y + verts[v2].y ) / 2.0f;
	*z = ( verts[v1].z + verts[v2].z ) / 2.0f;
}

void _GDK_PM::GDK_centre_out_edge(int seg, int edge, float *x, float *y, float *z)
{
	// wrap seg and edge just in case

	seg  %= map_size;
	edge %= pSegList[seg].oedges.size();

	_VERT	*verts = &pSegList[seg].verts[0];
	_EDGE	*edges = &pSegList[seg].oedges[0];

	int		v1 = edges[edge].v1;
	int		v2 = edges[edge].v2;

	*x = ( verts[v1].x + verts[v2].x ) / 2.0f;
	*y = ( verts[v1].y + verts[v2].y ) / 2.0f;
	*z = ( verts[v1].z + verts[v2].z ) / 2.0f;
}

float _GDK_PM::GDK_angle_in_edge(int seg, int edge)
{
	// fix seg and edge just in case

	seg  %= map_size;
	edge %= pSegList[seg].iedges.size();

	_VERT	*verts = &pSegList[seg].verts[0];
	_EDGE	*edges = &pSegList[seg].iedges[0];

	// get vert refs

	int		v1 = edges[edge].v1;
	int		v2 = edges[edge].v2;

	// get edges coords

	float	x1 = verts[v1].x;
	float	z1 = verts[v1].z;
	float	x2 = verts[v2].x;
	float	z2 = verts[v2].z;

	// get angle of edge in degrees

	float	angle = Rad2Deg( atan2( z2 - z1, x2 - x1 ) );

	return	angle;
}

float _GDK_PM::GDK_angle_out_edge(int seg, int edge)
{
	// fix seg and edge just in case

	seg  %= map_size;
	edge %= pSegList[seg].oedges.size();

	_VERT	*verts = &pSegList[seg].verts[0];
	_EDGE	*edges = &pSegList[seg].oedges[0];

	// get vert refs

	int		v1 = edges[edge].v1;
	int		v2 = edges[edge].v2;

	// get edges coords

	float	x1 = verts[v1].x;
	float	z1 = verts[v1].z;
	float	x2 = verts[v2].x;
	float	z2 = verts[v2].z;

	// get angle of edge in degrees

	float	angle = Rad2Deg( atan2( z2 - z1, x2 - x1 ) );

	return	angle;
}

int _GDK_PM::GDK_map_size()
{
	return map_size;
}

int _GDK_PM::GDK_seg_type(int seg)
{
	seg %= map_size;

	// 0 = corridor, 1 = room

	return pSegList[seg].faces[0].type;
}

void _GDK_PM::GDK_seg_floor_texture(int seg, int texture)
{
	seg %= map_size;

	DBPro::TextureLimb( ObjID, seg, texture);
}

void _GDK_PM::GDK_seg_wall_texture(int seg, int texture)
{
	seg %= map_size;

	DBPro::TextureLimb( ObjID, seg + map_size, texture );
}

void _GDK_PM::GDK_seg_roof_texture(int seg, int texture)
{
	seg %= map_size;

	DBPro::TextureLimb( ObjID, seg + map_size + map_size, texture );
}

void _GDK_PM::GDK_seg_floor_colour(int seg, int col)
{
	seg %= map_size;

	DBPro::ColorLimb( ObjID, seg, col);
}

void _GDK_PM::GDK_seg_wall_colour(int seg, int col)
{
	seg %= map_size;

	DBPro::ColorLimb( ObjID, seg + map_size, col );
}

void _GDK_PM::GDK_seg_roof_colour(int seg, int col)
{
	seg %= map_size;

	DBPro::ColorLimb( ObjID, seg + map_size + map_size, col );
}

int _GDK_PM::GDK_get_object()
{
	return ObjID;
}
