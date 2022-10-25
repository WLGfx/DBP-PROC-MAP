#pragma once

#define PI 3.1415926535897932384626433832795f
#define PIdiv180 (PI/180.0f)

//
// Function declarations
//



//
// HELPER MACROS
//

// helper functions to convert from radians to degrees (GDK uses degrees)
inline float Rad2Deg (float Angle) {
  static float _degratio = 180.0f / PI;
  return Angle * _degratio;
}
inline float Deg2Rad (float Deg) {
	static float _radratio = PI / 180.0f;
	return Deg * _radratio;
}

// Perlin Noise (credits go to http://stackoverflow.com/questions/4753055/perlin-noise-generation-for-terrain)

class PerlinNoise
{
public:

	// Constructor
	PerlinNoise();
	PerlinNoise(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);

	// Get Height
	double GetHeight(double x, double y) const;
	double GetHeightTiled(float ox, float oy, int w, int h);

	// Get
	double Persistence() const { return persistence; }
	double Frequency()   const { return frequency;   }
	double Amplitude()   const { return amplitude;   }
	int    Octaves()     const { return octaves;     }
	int    RandomSeed()  const { return randomseed;  }

	// Set
	void Set(double _persistence, double _frequency, double _amplitude, int _octaves, int _randomseed);

	void SetPersistence(double _persistence) { persistence = _persistence; }
	void SetFrequency(  double _frequency)   { frequency = _frequency;     }
	void SetAmplitude(  double _amplitude)   { amplitude = _amplitude;     }
	void SetOctaves(    int    _octaves)     { octaves = _octaves;         }
	void SetRandomSeed( int    _randomseed)  { randomseed = _randomseed;   }

	/*int	GetHeightByte(float x, float y);
	int	GetHeightByteAbs(float x, float y);
	int	GetHeightByteTop(float x, float y);
	int	GetHeightSigned(float x, float y);
	int	GetHeightTiledByte(float ox, float oy, int w, int h);
	int	GetHeightTiledByteAbs(float ox, float oy, int w, int h);
	int	GetHeightTiledByteTop(float ox, float oy, int w, int h);
	int	GetHeightTiledSigned(float ox, float oy, int w, int h);*/

private:

    double Total(double i, double j) const;
    double GetValue(double x, double y) const;
    //double Interpolate_old(double x, double y, double a) const;
	//double Interpolate_cos(double a,double b,double x) const;
	double Interpolate(double a,double b,double x) const;
    double Noise(int x, int y) const;
	//double Noise_old(int x, int y) const;

	double persistence, frequency, amplitude;
    int octaves, randomseed;
};

// PROC_MAP main classes

class _VERT {
public:
	float	x,y,z;

	// these are used to setup the mesh info (makes the structure 32 bytes each)
	float	nx, ny, nz;
	float	s, t;

	inline	_VERT() { x=y=z=0; }
	inline	_VERT(float xx, float yy, float zz)			{ x=xx; y=yy; z=zz; }
	inline	void set(float xx, float yy, float zz)		{ x=xx; y=yy; z=zz; }
	inline	void setUV(float u, float v)				{ s=u;  t=v; }
	inline	void setNormals(float x, float y, float z)	{ nx=x; ny=y; nz=z; }
};

class _EDGE {
public:
	int		v1, v2;		// start and end
	int		type;		// corridor or room / inner or outer?
	int		segment;	// segment this edge belongs to

	inline	_EDGE()
	{ 
		v1=v2=type=segment=0;
	}

	inline	_EDGE(int v1, int v2, int type, int segment)
	{
		this->v1=v1;
		this->v2=v2;
		this->type=type;
		this->segment=segment;
	}

	inline	void set(int v1, int v2, int type, int segment)
	{ 
		this->v1=v1;
		this->v2=v2;
		this->type=type;
		this->segment=segment;
	}

	inline	void set(int type)
	{
		this->type=type;
	}

	inline	void set(int v1, int v2)
	{
		this->v1=v1;
		this->v2=v2;
	}
};

class _FACE {
public:
	int		v1, v2, v3;
	int		type;

	// An addition now is the segment number to be used in the occlusion culling to boost FPS
	// When the algorithm creates each limb it will create it's own verts and index list
	int		segment;

	inline	_FACE() { v1=v2=v3=0; }
	inline	_FACE(int p1, int p2, int p3, int type, int segment)		
	{ 
		v1 = p1;
		v2 = p2;
		v3 = p3;
		this->type		= type;
		this->segment	= segment;
	}
	inline	void set(int p1, int p2, int p3)	
	{ 
		v1=p1;
		v2=p2;
		v3=p3;
	}
	inline	void set(int p1, int p2, int p3, int type)
	{
		v1=p1;
		v2=p2;
		v3=p3;
		this->type=type;
	}
	inline	void set(int p1, int p2, int p3, int type, int segment)
	{
		v1=p1;
		v2=p2;
		v3=p3;
		this->type=type;
		this->segment=segment;
	}
	inline	void set(int type)
	{
		this->type=type;
	}
};

// definitions for the edges list
#define PM_CINNER	0		// corridor inner edge
#define PM_COUTER	1		// corridor outer edge
#define PM_RINNER	2		// room inner edge
#define PM_ROUTER	3		// room outer edge
#define PM_RENTRY	4		// room entry edge (ie the doorway)

#define PM_COUTERR	5		// corridor outer edge on roof
#define PM_ROUTERR	6		// room outer edge on roof

#define PM_COUTERW	7		// corridor outer edge on wall
#define PM_ROUTERW	8		// room outer edge on wall


#define PM_CFACE	0		// corridor face
#define PM_RFACE	1		// room face

#define PM_CFACER	2		// corridor face roof
#define PM_RFACER	3		// room face on roof

#define PM_CFACEW	4		// corridor face on wall
#define PM_RFACEW	5		// room face on wall

//
// defaults for the map setup
//

#define PM_MAPSIZE	100		// default map size in segments to add

#define PM_SIZEMIN	80		// corridor wall length min and max range
#define PM_SIZEMAX	120
#define PM_ANGMIN	60		// new corridor angle min max range
#define PM_ANGMAX	120
#define PM_CSEGMIN	6		// add corridor segments min max range
#define PM_CSEGMAX	16

#define PM_NOISEHGT	200		// default height adjusted by the perlin noise

#define PM_ROOMMIN	80		// room edges range for size
#define PM_ROOMMAX	150
#define PM_RSEGMIN	10		// room edges count range (seems safe with min of 7)
#define PM_RSEGMAX	20

#define PM_RNDSEED	14		// random seed default (was originally zero)

#define PM_NOISE_SCALE 100.0f	// scale down the perlin noise to stop glitches

struct _temp_edge_list {
	int v1, v2, mid_vert;
};

#include <vector>

using namespace std;

class PROC_MAP {
public:
	PROC_MAP(void);
	~PROC_MAP(void);

	void	setup(int size, int seed, int lmin, int lmax, int amin, int amax);
	void	generate();

	void	get_edge_centre(int edge, float *x, float *y, float *z);

	void	set_map(int size, int seed, int height);
	void	set_corridor(int len_min, int len_max, int aang_min, int aang_max, int seg_min, int seg_max);
	void	set_room(int len_min, int len_max, int seg_min, int seg_max);

	vector<_VERT>	verts;		// main verts list
	vector<_EDGE>	edges;		// main edges list
	vector<_FACE>	faces;		// main faces list

	int		map_size;			// ammount of corridors and rooms that make the map
	int		map_seed;

	int		size_min, size_max;	// corridor length min and max range
	int		ang_min, ang_max;	// corridor angle range
	int		cseg_min, cseg_max;	// corridor segment range

	int		room_min, room_max;	// length of the edges
	int		rseg_min, rseg_max;	// number of sides to the room

	int		noise_hgt;			// height adjusted by perlin noise
	int		tess;				// tessellation count

	PerlinNoise pn;				// used to slope the dungeon

	int		curr_segment;

	void	start_poly();

	int		add_corridor_seg( int edge );	// adds a single corridor segment

	bool	add_corridor();		// attempts to add a corridor
	bool	add_room();			// attempts to add a room

	bool	edge_intersect(float x1, float z1, float x2, float z2);
	bool	line_intersect(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4);
	float	get_noise(float x, float y);	// get the Y height for the level verts
	void	vvv_circle(	float px, float py,
						float qx, float qy,
						float rx, float ry,
						float *x, float *y); //, float &r );

	//void	tessellate(int levels);
	//void	vbo_create();

private:
	//int		verts_count_floor;
	//int		edges_count_floor;
	//int		faces_count_floor;

	//void	_TESSELLATE();
	//void	_NORMALISE();
};

/*

I balls'd up a bit when creating the wall vbo.
I will need to create a temp list of the edges that actually fall on the outer edge.

Balls'd up a second time and dunno what happened.
3rd time lucky, taking each step at a time.
1. Count the outer edges.
2. Create the outer edges list and copy the data into it.
3. Bubble sort the array.

Another problem which I've only just figured out. 


1. Taken out the noise calc per vert from the initial generation function
2. Added new mesh setup function
3. Tessellation now takes place on floor, roof and walls at the same time
4. Texture coords done during normalisation function.
5. Noise height for Y coord added during normalisation function.

*/