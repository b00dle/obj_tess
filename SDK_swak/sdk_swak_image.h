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
#ifndef SDK_SWAK_IMAGE_H
#define SDK_SWAK_IMAGE_H

//
// Header for the SDK SWAK image loading support
//

namespace nv
{

	//
	// C-Style interface used as the layer to cross the dll boundary
	//  Read only support for now
	struct ImgStruct;

	SDK_API ImgStruct* CreateImgFromFile( const char* file);
	SDK_API void DestroyImg( ImgStruct *is);

	SDK_API int GetImgWidth( const ImgStruct *is);
	SDK_API int GetImgHeight( const ImgStruct *is);
	SDK_API int GetImgDepth( const ImgStruct *is);
	SDK_API int GetImgArraySize( const ImgStruct *is);

	SDK_API GLenum GetImgFormat( const ImgStruct *is);
	SDK_API GLenum GetImgInternalFormat( const ImgStruct *is);
	SDK_API GLenum GetImgType( const ImgStruct *is);
	SDK_API int GetImgSize( const ImgStruct *is, int level);

	SDK_API const void* GetImgData( const ImgStruct *is, int level, int slice);

	class ImageObj
	{
	private:
		ImgStruct *_is;

		//forbid copying to prevent object ownership issues
		ImageObj( const ImageObj& io) {}
		ImageObj& operator= (const ImageObj& io) {}

	public:

		ImageObj() : _is(0) {}
		ImageObj( const char* filename) : _is(0)
		{
			_is = CreateImgFromFile( filename);
		}
		~ImageObj() { if (_is) DestroyImg(_is); }

		void LoadImage( const char* filename) { if (_is) DestroyImg(_is); _is = CreateImgFromFile( filename);}

		bool hasImage() { return _is != 0;}

		int Width() { return (_is) ? GetImgWidth( _is) : 0; }
		int Height() { return (_is) ? GetImgHeight( _is) : 0; }
		int Depth() { return (_is) ? GetImgDepth( _is) : 0; }
		int ArraySize() { return (_is) ? GetImgArraySize( _is) : 0; }

		// It'd be nice to boil these down to a single type
		GLenum Format() { return (_is) ? GetImgFormat(_is) : 0;}
		GLenum InternalFormat() { return (_is) ? GetImgInternalFormat(_is) : 0;}
		GLenum Type() { return (_is) ? GetImgFormat(_is) : 0;}

		// automagically update textures
		/*
		void LoadLevelToTexture( GLuint tex, GLenum target, int level, int layer);
		void LoadToTexture( GLuint tex, GLenum target);
		*/

		const void *LevelPtr( int level, int layer) { return _is ? GetImgData( _is, level, layer) : 0;}
	};

}

#endif // SDK_SWAK_IMAGE_H
