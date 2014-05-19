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
#ifndef SDK_SWAK_FILE_H
#define SDK_SWAK_FILE_H

namespace nv
{
	namespace FileMode
	{
		enum FileMode
		{
			Block = 0x1,
			Stream = 0x2,
			Read = 0x10,
			Write = 0x20,
			Compressed = 0x100
		};
	}

	// Class abstracts reading/writing of files
	class FilePtr
	{
	private:
		struct Rep;
		Rep *_rep;
		FilePtr() {}

	public:

		

		SDK_API static FilePtr* Create( const char* name, FileMode::FileMode m);

		SDK_API int Read( size_t numBytes, void* data);
		SDK_API int Write( size_t numBytes, const void* data);
		SDK_API void Flush();
		SDK_API size_t GetSize();
		
		SDK_API virtual ~FilePtr();
	};
};

#endif
