#pragma once

#include "stdafx.h"

struct _SEGLIST
{
	// floor segment

	std::vector<_VERT> verts;
	std::vector<_EDGE> edges;
	std::vector<_FACE> faces;

	std::vector<_EDGE> oedges;		// split into these at very end
	std::vector<_EDGE> iedges;

	// roof segment (NB: add inner and outer edge lists)

	std::vector<_VERT> rverts;
	std::vector<_EDGE> redges;
	std::vector<_FACE> rfaces;

	// wall segment

	std::vector<_VERT> wverts;
	std::vector<_EDGE> wedges;
	std::vector<_FACE> wfaces;

	float	xmin, ymin, xmax, ymax, zmin, zmax;
};

class _GDK_PM : public PROC_MAP
{
public:
	_GDK_PM(void);
	~_GDK_PM(void);

	int		GDK_free_image();
	int		GDK_free_object();

	void	GDK_corridor_textures(char *floor, char *wall, char *roof);
	void	GDK_room_textures(char *floor, char *wall, char *roof);

	void	GDK_generate();
	
	void	GDK_centre_in_edge(int seg, int edge, float *x, float *y, float *z);
	void	GDK_centre_out_edge(int seg, int edge, float *x, float *y, float *z);

	float	GDK_angle_in_edge(int seg, int edge);
	float	GDK_angle_out_edge(int seg, int edge);

	int		GDK_map_size();
	int		GDK_seg_type(int seg);

	void	GDK_seg_floor_texture(int seg, int texture);
	void	GDK_seg_wall_texture(int seg, int texture);
	void	GDK_seg_roof_texture(int seg, int texture);
	
	void	GDK_seg_floor_colour(int seg, int col);
	void	GDK_seg_wall_colour(int seg, int col);
	void	GDK_seg_roof_colour(int seg, int col);

	int		GDK_get_object();

	void	GDK_clear_all();

	//
	// GDK stuff
	//

	int		ObjID;

	std::vector<_SEGLIST>	pSegList;
	int		textures[6];

private:

	std::vector<_EDGE>		tedges;
	std::vector<_VERT>		tverts;
	int		*pVertList;

	int		iLimbCount;
	
	int		iSegment;
	sObject* pObject;
	sFrame*	pFrame;

private:
	int		curr_seg;
	bool	DBO_flag;

	// private functions

	void	GDK_build_seglists();
	int		add_edge_vert( int v );
	int		check_wall_seg_vert( int v );
	void	add_normal(int v1,int v2,int v3,_VERT* vlist);
	void	div_normal(_VERT* vert, int div);
	void	GDK_create_roof();
	void	GDK_create_wall();
	void	GDK_calc_normals();
	
	void	GDK_build_object();
	void	GDK_dbo_floor();
	void	GDK_dbo_wall();
	void	GDK_dbo_roof();

	void	GDK_add_noise();
	void	GDK_tessellate();
	void	GDK_tess_section(std::vector<_VERT> &verts, 
							 std::vector<_EDGE> &edges, 
							 std::vector<_FACE> &faces);
	int		GDK_find_midvert(std::vector<_EDGE> &edges, int iESize,
							 int v1, int v2);
	void	GDK_clear_data();
	void	GDK_split_edge_lists();

	//void	GDK_DEBUG_DRAW_FLOOR();
};

//int _GDK_PM::makeGDKDBO()
//{
//	iLimbCount = this->map_size * 3;
//
//	ObjID = GDK_free_object();
//
//	CreateNewObject( ObjID, "", 1 );
//	pObject = dbGetObject( ObjID );
//
//	for ( iSegment = 0; iSegment < map_size; iSegment++ )
//	{
//		GDKDBO_addfloor();
//		GDKDBO_addwall();
//		GDKDBO_addroof();
//	}
//
//	SetNewObjectFinalProperties( ObjID, 0.0f );
//	SetTexture( ObjID, 0 );
//
//	dbConvertObjectFVF( ObjID, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_DIFFUSE | D3DFVF_SPECULAR );
//
//	for ( int loop = 0; loop < map_size; loop++ )
//	{
//		if ( segtype[loop] == 1 && textures[0] == 0)
//		{
//			dbColorLimb( ObjID, loop * 3, 0xffffff );
//			dbColorLimb( ObjID, loop * 3 + 1, 0xff8800 );
//			dbColorLimb( ObjID, loop * 3 + 2, 0xff0000 );
//		}
//		if ( segtype[loop] == 0 && textures[3] == 0 )
//		{
//			dbColorLimb( ObjID, loop * 3, 0x88ffff );
//			dbColorLimb( ObjID, loop * 3 + 1, 0x00ff88 );
//			dbColorLimb( ObjID, loop * 3 + 2, 0x00ff00 );
//		}
//		if ( segtype[loop] == 1 && textures[0] != 0)
//		{
//			dbTextureLimb( ObjID, loop * 3, textures[0] );
//			dbTextureLimb( ObjID, loop * 3 + 1, textures[1] );
//			dbTextureLimb( ObjID, loop * 3 + 2, textures[2] );
//		}
//		if ( segtype[loop] == 0 && textures[3] != 0 )
//		{
//			dbTextureLimb( ObjID, loop * 3, textures[3] );
//			dbTextureLimb( ObjID, loop * 3 + 1, textures[4] );
//			dbTextureLimb( ObjID, loop * 3 + 2, textures[5] );
//		}
//	}
//
//	dbSetObjectSpecular( ObjID, 0xffffff );
//	dbSetObjectSpecularPower( ObjID, 5 );
//
//	return ObjID;
//}
//
//// NB: verts[] don't have a segment so I'll have to do it the long way
//
//void _GDK_PM::GDKDBO_addfloor()
//{
//	// count verts and faces
//
//	int iVCount = 0, iFCount = 0, loop;
//
//	_VERT*	vptr = &verts[0];
//	_FACE*	fptr = &faces[0];
//
//	int stype;
//
//	for ( loop = faces.size(); loop;  )
//	{
//		loop--;
//
//		int seg = fptr[loop].segment;
//		int type= fptr[loop].type;
//
//		if ( seg == iSegment && (type == PM_CFACE || type == PM_RFACE) )
//		{
//			iFCount++;
//			stype = type;
//		}
//	}
//
//	segtype[iSegment] = 0 + ( 1 && stype == PM_RFACE );
//
//	iVCount = iFCount * 3;
//
//	sFrame* pFrame = 0;
//
//	if ( iSegment == 0 )
//		pFrame = pObject->pFrame;
//	else
//		pFrame = CreateNewFrame( pObject, "", true );
//
//	sMesh* pMesh = pFrame->pMesh;
//
//	SetupMeshFVFData( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, iVCount, iFCount * 3 );
//
//	int curr = 0;
//
//	for ( loop = 0; loop < faces.size(); loop++ )
//	{
//		int seg = fptr[loop].segment;
//		int type= fptr[loop].type;
//
//		if ( seg == iSegment && (type == PM_CFACE || type == PM_RFACE) )
//		{
//			_VERT* v = &verts[faces[loop].v1];
//			SetupStandardVertex( pMesh->dwFVF, pMesh->pVertexData, ( curr * 3 ),
//				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0xffffff, v->s, v->t );
//
//			v = &verts[faces[loop].v3];
//			SetupStandardVertex( pMesh->dwFVF, pMesh->pVertexData, ( curr * 3 ) + 1,
//				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0xffffff, v->s, v->t );
//
//			v = &verts[faces[loop].v2];
//			SetupStandardVertex( pMesh->dwFVF, pMesh->pVertexData, ( curr * 3 ) + 2,
//				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0xffffff, v->s, v->t );
//
//			pMesh->pIndices[curr * 3]	  = curr * 3;
//			pMesh->pIndices[curr * 3 + 1] = curr * 3 + 1;
//			pMesh->pIndices[curr * 3 + 2] = curr * 3 + 2;
//
//			curr++;
//		}
//	}
//
//	pMesh->iPrimitiveType	= D3DPT_TRIANGLELIST;
//	pMesh->iDrawVertexCount	= iVCount;
//	pMesh->iDrawPrimitives	= iFCount;
//}
//
//void _GDK_PM::GDKDBO_addwall()
//{
//	// count verts and faces
//
//	int iVCount = 0, iFCount = 0, loop;
//
//	_VERT*	vptr = &verts[0];
//	_FACE*	fptr = &faces[0];
//
//	int stype;
//
//	for ( loop = faces.size(); loop;  )
//	{
//		loop--;
//
//		int seg = fptr[loop].segment;
//		int type= fptr[loop].type;
//
//		if ( seg == iSegment && (type == PM_CFACEW || type == PM_RFACEW) )
//		{
//			iFCount++;
//			stype = type;
//		}
//	}
//
//	segtype[iSegment] = 0 + ( 1 && stype == PM_RFACEW );
//
//	iVCount = iFCount * 3;
//
//	sFrame* pFrame = CreateNewFrame( pObject, "", true );
//
//	sMesh* pMesh = pFrame->pMesh;
//
//	SetupMeshFVFData( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, iVCount, iFCount * 3 );
//
//	int curr = 0;
//
//	for ( loop = 0; loop < faces.size(); loop++ )
//	{
//		int seg = fptr[loop].segment;
//		int type= fptr[loop].type;
//
//		if ( seg == iSegment && (type == PM_CFACEW || type == PM_RFACEW) )
//		{
//			_VERT* v = &verts[faces[loop].v1];
//			SetupStandardVertex( pMesh->dwFVF, pMesh->pVertexData, ( curr * 3 ),
//				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0xffff00, v->s, v->t );
//
//			v = &verts[faces[loop].v3];
//			SetupStandardVertex( pMesh->dwFVF, pMesh->pVertexData, ( curr * 3 ) + 1,
//				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0xffff00, v->s, v->t );
//
//			v = &verts[faces[loop].v2];
//			SetupStandardVertex( pMesh->dwFVF, pMesh->pVertexData, ( curr * 3 ) + 2,
//				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0xffff00, v->s, v->t );
//
//			pMesh->pIndices[curr * 3]	  = curr * 3;
//			pMesh->pIndices[curr * 3 + 1] = curr * 3 + 1;
//			pMesh->pIndices[curr * 3 + 2] = curr * 3 + 2;
//
//			curr++;
//		}
//	}
//
//	pMesh->iPrimitiveType	= D3DPT_TRIANGLELIST;
//	pMesh->iDrawVertexCount	= iVCount;
//	pMesh->iDrawPrimitives	= iFCount;
//}
//
//void _GDK_PM::GDKDBO_addroof()
//{
//	// count verts and faces
//
//	int iVCount = 0, iFCount = 0, loop;
//
//	_VERT*	vptr = &verts[0];
//	_FACE*	fptr = &faces[0];
//
//	int stype;
//
//	for ( loop = faces.size(); loop;  )
//	{
//		loop--;
//
//		int seg = fptr[loop].segment;
//		int type= fptr[loop].type;
//
//		if ( seg == iSegment && (type == PM_CFACER || type == PM_RFACER) )
//		{
//			iFCount++;
//			stype = type;
//		}
//	}
//
//	segtype[iSegment] = 0 + ( 1 && stype == PM_RFACER );
//
//	iVCount = iFCount * 3;
//
//	sFrame* pFrame = CreateNewFrame( pObject, "", true );
//
//	sMesh* pMesh = pFrame->pMesh;
//
//	SetupMeshFVFData( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, iVCount, iFCount * 3 );
//
//	int curr = 0;
//
//	for ( loop = 0; loop < faces.size(); loop++ )
//	{
//		int seg = fptr[loop].segment;
//		int type= fptr[loop].type;
//
//		if ( seg == iSegment && (type == PM_CFACER || type == PM_RFACER) )
//		{
//			_VERT* v = &verts[faces[loop].v1];
//			SetupStandardVertex( pMesh->dwFVF, pMesh->pVertexData, ( curr * 3 ),
//				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0x00ffff, v->s, v->t );
//
//			v = &verts[faces[loop].v3];
//			SetupStandardVertex( pMesh->dwFVF, pMesh->pVertexData, ( curr * 3 ) + 1,
//				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0x00ffff, v->s, v->t );
//
//			v = &verts[faces[loop].v2];
//			SetupStandardVertex( pMesh->dwFVF, pMesh->pVertexData, ( curr * 3 ) + 2,
//				v->x, v->y, v->z, v->nx, v->ny, v->nz, 0x00ffff, v->s, v->t );
//
//			pMesh->pIndices[curr * 3]	  = curr * 3;
//			pMesh->pIndices[curr * 3 + 1] = curr * 3 + 1;
//			pMesh->pIndices[curr * 3 + 2] = curr * 3 + 2;
//
//			curr++;
//		}
//	}
//
//	pMesh->iPrimitiveType	= D3DPT_TRIANGLELIST;
//	pMesh->iDrawVertexCount	= iVCount;
//	pMesh->iDrawPrimitives	= iFCount;
//}

//
// BUILD SEGLISTS Pseudo Code
//

/*
	1. Create temp vert ref list
	2. Setup main segments
	3. Run through segment lists
		a. Clear vert ref list to 0
		b. Clear verts, edges and faces list in segment
		c. Run through edge lists (ADD VERTS AND EDGES)
			1. Same as current segment
				a. Check v1 already exists
				b. Check v2 already exists
				c. Add to Inner or outer edge list
		d. Run through face list (ADD FACES)
			1. Same as current segment
				a. Check v1 already exists
				b. Check v2 already exists
				c. Check v3 already exists
				d. Add face to segment
		e. DEBUG - Draw Seg Lines to screen
	4. Delete temp vert reference list (set to 0)
*/

/*
 1. make temp list out of all outer edges only
	a. copy only outer edges to the temp list
 2. sort them
 3. create temp vert list
 4. create edge UV coords in the temp verts list
 5. create segment edge and vert lists
 6. remove temp edge list and verts list
 7. create faces
 */

// MATTY H - CREATE DBO

/*bool CreateDBO(int iID, const aiScene *scene)
{
	DWORD dwMeshCount = scene->mNumMeshes;
    DWORD dwFrameCount = dwMeshCount;
    // create a new object
    CreateNewObject ( iID, "test", 1);

    // setup general object data
    sObject*        pObject = dbGetObject ( iID );

	for(int frame = 0; frame < dwFrameCount; frame++){
		//bool hasTex = scene->mMeshes[frame]->HasTextureCoords(0);
		//int channels = scene->mMeshes[frame]->GetNumUVChannels();
        sFrame* pFrame = 0;
		if(frame == 0){
           pFrame  = pObject->pFrame;
		}
		else{
           pFrame = CreateNewFrame(pObject, "", true);
		}
        sMesh* pMesh   = pFrame->pMesh;
		
		// create vertex memory
		DWORD dwVertexCount = scene->mMeshes[frame]->mNumVertices;  // store number of vertices
		DWORD dwFaceCount   = scene->mMeshes[frame]->mNumFaces;
		DWORD dwIndexCount  = dwFaceCount * 3;                      // store number of indices
		SetupMeshFVFData ( pMesh, D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1, dwVertexCount, dwIndexCount);

		// VERTICES
		for(int i = 0; i < dwVertexCount; i++){
			aiVector3D pos = scene->mMeshes[frame]->mVertices[i];
			aiVector3D norm = scene->mMeshes[frame]->mNormals[i];
			aiVector3D uv = scene->mMeshes[frame]->mTextureCoords[0][i];
			SetupStandardVertex ( pMesh->dwFVF, pMesh->pVertexData, i, pos.x, pos.y, pos.z, norm.x, norm.y, norm.z, D3DCOLOR_ARGB ( 255, 255, 255, 255 ), uv.x, uv.y );
		}

		bool faces = scene->mMeshes[frame]->HasFaces();
		// INDICES
		int iIndex = 0;
		for(int i = 0; i < dwFaceCount; i++){
			aiFace face = scene->mMeshes[frame]->mFaces[i];
			for(int index = 0; index < face.mNumIndices; index++){
				pMesh->pIndices[iIndex] = face.mIndices[index];
				iIndex++;
			}
		}

		// setup mesh drawing properties
		pMesh->iPrimitiveType   = D3DPT_TRIANGLELIST;
		pMesh->iDrawVertexCount = dwVertexCount;
		pMesh->iDrawPrimitives  = dwFaceCount;  // if indexdata?
	}
    // setup new object and introduce to buffers
    SetNewObjectFinalProperties ( iID, 10 );

    // give the object a default texture
    SetTexture ( iID, 0 );
	return true;
}*/