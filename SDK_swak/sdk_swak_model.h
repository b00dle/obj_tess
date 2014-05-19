// Copyright (c) 2012 NVIDIA Corporation. All rights reserved.
//
// TO  THE MAXIMUM  EXTENT PERMITTED  BY APPLICABLE  LAW, THIS SOFTWARE  IS PROVIDED
// *AS IS*  AND NVIDIA AND  ITS SUPPLIERS DISCLAIM  ALL WARRANTIES,  EITHER  EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED  TO, NONINFRINGEMENT,IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL  NVIDIA 
// OR ITS SUPPLIERS BE  LIABLE  FOR  ANY  DIRECT, SPECIAL,  INCIDENTAL,  INDIRECT,  OR  
// CONSEQUENTIAL DAMAGES WHATSOEVER (INCLUDING, WITHOUT LIMITATION,  DAMAGES FOR LOSS 
// OF BUSINESS PROFITS, BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY 
// OTHER PECUNIARY LOSS) ARISING OUT OF THE  USE OF OR INABILITY  TO USE THIS SOFTWARE, 
// EVEN IF NVIDIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
//
// Please direct any bugs or questions to SDKFeedback@nvidia.com

#ifndef SDK_SWAK_MODEL_H
#define SDK_SWAK_MODEL_H

//
// Header for the SDK SWAK image loading support
//

namespace nv
{


// forward declaration
struct MeshStruct;

namespace AttribDataType
{
	// limited to attribs size size % 32 == 0

	//might be better to simply enumerate types (half, float, int, short, byte) and leave -mapping to the app
	enum AttribDataType
	{
		//32-bit floats
		Float1,
		Float2,
		Float3,
		Float4,

		//16-bit floats
		Half2,
		Half4,

		//32-bit ints
		Int1,
		Int2,
		Int3,
		Int4,
		Uint1,
		Uint2,
		Uint3,
		Uint4,

		//16-bit ints (signed, unsigned, normalized signed, and unsigned normalized )
		Short2,
		Short4,
		Ushort2,
		Ushort4,
		Nshort2,
		Nshort4,
		UNshort2,
		UNshort4,

		//8-bit ints
		Byte4,
		Ubyte4,
		Nbyte4,
		UNbyte4
	};
}

namespace {
struct AttribTypeParams
{
	GLenum type;
	GLint size;
	GLboolean normalized;
};

	static const AttribTypeParams AttribTypeMap[] =
	{
		//32-bit floats
		{ GL_FLOAT, 1, GL_FALSE},
		{ GL_FLOAT, 2, GL_FALSE},
		{ GL_FLOAT, 3, GL_FALSE},
		{ GL_FLOAT, 4, GL_FALSE},

		//16-bit floats
		{ GL_HALF_FLOAT, 2, GL_FALSE},
		{ GL_HALF_FLOAT, 4, GL_FALSE},

		//32-bit ints
		{ GL_INT, 1, GL_FALSE},
		{ GL_INT, 2, GL_FALSE},
		{ GL_INT, 3, GL_FALSE},
		{ GL_INT, 4, GL_FALSE},
		{ GL_UNSIGNED_INT, 1, GL_FALSE},
		{ GL_UNSIGNED_INT, 2, GL_FALSE},
		{ GL_UNSIGNED_INT, 3, GL_FALSE},
		{ GL_UNSIGNED_INT, 4, GL_FALSE},

		//16-bit ints (signed, unsigned, normalized signed, and unsigned normalized )
		{ GL_SHORT, 2, GL_FALSE},
		{ GL_SHORT, 4, GL_FALSE},
		{ GL_UNSIGNED_SHORT, 2, GL_FALSE},
		{ GL_UNSIGNED_SHORT, 4, GL_FALSE},
		{ GL_SHORT, 2, GL_TRUE},
		{ GL_SHORT, 4, GL_TRUE},
		{ GL_UNSIGNED_SHORT, 2, GL_TRUE},
		{ GL_UNSIGNED_SHORT, 4, GL_TRUE},

		//8-bit ints
		{ GL_BYTE, 4, GL_FALSE},
		{ GL_UNSIGNED_BYTE, 4, GL_FALSE},
		{ GL_BYTE, 4, GL_TRUE},
		{ GL_UNSIGNED_BYTE, 4, GL_TRUE}
	};
}

namespace Semantic
{
	enum Semantic
	{
		Position  = 0x10,
		Normal    = 0x20,
		TexCoord  = 0x30,
		Color     = 0x40,
		TangentU  = 0x50,
		TangentV  = 0x60,
		Data      = 0x70,

		// numbered semantics
		Position0 = 0x10,  Position1 = 0x11,  Position2 = 0x12,  Position3 = 0x13,
		Normal0   = 0x20,  Normal1   = 0x21,  Normal2   = 0x22,  Normal3   = 0x23,
		TexCoord0 = 0x30,  TexCoord1 = 0x31,  TexCoord2 = 0x32,  TexCoord3 = 0x33,
		Color0    = 0x40,  Color1    = 0x41,  Color2    = 0x42,  Color3    = 0x43,
		TangentU0 = 0x50,  TangentU1 = 0x51,  TangentU2 = 0x52,  TangentU3 = 0x53,
		TangentV0 = 0x60,  TangentV1 = 0x61,  TangentV2 = 0x62,  TangentV3 = 0x63,
		Data0     = 0x70,  Data1     = 0x71,  Data2     = 0x72,  Data3     = 0x73,

		Max = 0xff // restrict semantics to one byte
	};
}


namespace IndexType
{
	enum IndexType
	{
		Int,
		Short,
		Byte
	};
}

// ToDo
//
//   C-Style functions could use a better naming convention
//

SDK_API MeshStruct *CreateMeshFromFile( const char *fn);
SDK_API void DestroyMesh( MeshStruct *m);

//bound box
SDK_API void GetBoundingBox( const MeshStruct *m, float *min, float * max);

//Vertex count
SDK_API int GetVertexCount( const MeshStruct *m);

// indices and primitive data
SDK_API const void *GetIndexPointer( const MeshStruct* m);

SDK_API IndexType::IndexType GetIndexType( const MeshStruct *m);
SDK_API int GetPrimitiveType( const MeshStruct *m);
SDK_API int GetPrimitiveCount( const MeshStruct *m);

// attributes
SDK_API int GetAttributeCount( const MeshStruct* m);
SDK_API AttribDataType::AttribDataType GetAttribType( const MeshStruct *m, int attrib);
SDK_API int GetAttributeStream( const MeshStruct *m, int attrib);
SDK_API int GetAttributeOffset( const MeshStruct *m, int attrib);
SDK_API int GetAttributeSemantic( const MeshStruct *m, int attrib);

// streams
SDK_API int GetStreamCount( const MeshStruct* m);
SDK_API int GetStreamStride( const MeshStruct* m, int stream);
SDK_API const void* GetStreamPointer( const MeshStruct* m, int stream);

// Creating / writing
SDK_API MeshStruct *CreateEmptyMesh( int streams, int attribs);
SDK_API void SetMeshTopologyData( MeshStruct* m, int pt, IndexType::IndexType it, int numPrim, const void* indices);
SDK_API void SetMeshStreamData( MeshStruct* m, int stream, int vertexCount, int stride, const void* data);
SDK_API void SetMeshAttrib( MeshStruct* m, int attrib, int stream, int offset, AttribDataType::AttribDataType at, Semantic::Semantic s);
SDK_API void SetMeshBoundingBox( MeshStruct* m, const float *min, const float *max);

SDK_API int WriteMesh( const MeshStruct* m, const char *fn);

/*
 * C++ API
 *
 */

struct Attribute
{
	int offset;
	GLenum type;
	int size;
	Semantic::Semantic semantic;
	int stride;
	int stream;
	GLboolean normalized;

	//need conversion operations
/*
	void SetOffset( GLuint vbo, const void* baseoffet)
	{
}
*/
};

class ModelObj
{
private:
	MeshStruct *_ms;

	//prevent copies to avoid ownership issues
	ModelObj( const ModelObj& m) {}
	const ModelObj& operator=( const ModelObj& m) {}

	// map semantics to attribute store locations
	int _semanticMap[Semantic::Max + 1];

public:

	ModelObj() : _ms(0)
	{
		for (int i=0; i< Semantic::Max + 1; i++)
			_semanticMap[i]= -1;
	}
	ModelObj( const char *filename)
	{
		for (int i=0; i< Semantic::Max + 1; i++)
			_semanticMap[i]= -1;
		
		_ms = CreateMeshFromFile( filename);
		
		if (_ms)
		{
			int attributeCount = GetAttributeCount( _ms);
			for (int i=0; i< attributeCount; i++)
			{
				int semantic = GetAttributeSemantic( _ms, i);
				_semanticMap[semantic] = i;
			}
		}
	}

	void LoadModelFromFile( const char *filename)
	{
		DestroyMesh(_ms);
		_ms = CreateMeshFromFile( filename);

		for (int i=0; i< Semantic::Max + 1; i++)
			_semanticMap[i]= -1;

		if (_ms)
		{
			int attributeCount = GetAttributeCount( _ms);
			for (int i=0; i< attributeCount; i++)
			{
				int semantic = GetAttributeSemantic( _ms, i);
				_semanticMap[semantic] = i;
			}
		}
	}

	bool Valid() { return _ms != 0; }

	int VertexCount() { return _ms ? GetVertexCount( _ms) : 0; }
	int PrimitiveCount() { return _ms ? GetPrimitiveCount( _ms) : 0; }
	int PrimitiveType() { return _ms ? GetPrimitiveType( _ms) : 1; }
	IndexType::IndexType IndexType() { return _ms ? GetIndexType(_ms) : IndexType::Int; }

	int StreamCount() { return _ms ? GetStreamCount( _ms) : 0; }
	const void* StreamPointer( int i) { return _ms ? GetStreamPointer( _ms, i) : 0; }
	int StreamStride( int i) { return _ms ? GetStreamStride( _ms, i) : 0; }

	const void* IndexPointer() { return _ms ? GetIndexPointer(_ms) : 0; }

	int GetAttribCount() {return _ms ? GetAttributeCount( _ms) : 0; }
	int AttributeStream( int i) {return _ms ? GetAttributeStream( _ms, i) : 0; }
	AttribDataType::AttribDataType AttributeType( int i) { return _ms ? GetAttribType( _ms, i) : AttribDataType::Float1; }
	int AttributeOffset( int i) { return _ms ? GetAttributeOffset( _ms, i) : 0; }
	Semantic::Semantic Semantic(int i) { return Semantic::Semantic( _ms ? GetAttributeSemantic( _ms, i) : 0); }

	//map a semantic to an attribute channel
	int FindAttribute( Semantic::Semantic s) { return _semanticMap[s];}

	Attribute GetAttribute( int i)
	{
		Attribute a;
		AttribTypeParams atp = AttribTypeMap[ AttributeType(i)];
		a.type = atp.type;
		a.size = atp.size;
		a.normalized = atp.normalized;
		a.stream = AttributeStream( i);
		a.stride = StreamStride( a.stream);
		a.offset = AttributeOffset(i);
		a.semantic = Semantic(i);

		return a;
	}

	Attribute GetAttributeBySemantic( Semantic::Semantic s)
	{
		return GetAttribute( FindAttribute( s));
	}

	// maybe convert to nv::vec3 here?
	void GetBoundingBox( float* min, float *max)
	{
		if (_ms)
		{
			nv::GetBoundingBox( _ms, min, max); 
		}
	}

	//writing methods
	ModelObj( int streams, int attribs)
	{
		_ms = CreateEmptyMesh( streams, attribs); 
	}

	// model will not take ownership of pointers, but will reference them, with the exception of the BBox, that is copied
	void SetTopologyData( int pt, IndexType::IndexType it, int numPrim, const void* indices)
	{
		SetMeshTopologyData( _ms, pt, it, numPrim, indices);
	}

	void SetStreamData( int stream, int vertexCount, int stride, const void* data)
	{
		SetMeshStreamData( _ms, stream, vertexCount, stride, data);
	}

	void SetAttrib( int attrib, int stream, int offset, AttribDataType::AttribDataType at, Semantic::Semantic s)
	{
		SetMeshAttrib( _ms, attrib, stream, offset, at, s);
	}

	void SetBoundingBox( const float *min, const float *max)
	{
		SetMeshBoundingBox( _ms, min, max);
	}

	bool WriteModel( const char* filename)
	{
		return WriteMesh( _ms, filename) == 0;
	}
};

}
#endif
