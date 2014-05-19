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

#ifndef SDK_SWAK
#define SDK_SWAK

#if defined(_WIN32)

#if defined(BUILD_SDK_DLL)
#define SDK_API __declspec(dllexport)
#else
#define SDK_API __declspec(dllimport)
#endif


#include "GLEW/glew.h"
#include <gl/GL.h>

//need correct export setting for non WIN32
#endif

//Header encompassing generic SDK swak functionality

#include "sdk_swak_winlayer.h"

// Modules
#include "sdk_swak_file.h"
#include "sdk_swak_model.h"
#include "sdk_swak_image.h"

#include "include\nvMath.h"

#include "include\nvCvar.h"
#include "include\nvShaderUtils.h"

// Window interface module
//  Context / window creation
//  Keyboard, mouse, joypad, touch event translation
//  GL extension support

// File interface module
//  Deals with opening files and reading data
//  Supports path searching to allow SDK distribution
//  Needed?

// Image interface module
//  Deals with loading image data

// Model interface module
//  Deals with loading model data

// Widgets

// ConVars

// Issues
//  C++ std lib issues
//  Heap management
//    Heap operations must occur consistently on the same library
//    Must present factory methods for creation / deletion of any objects
//  String representation
//    Template representation can differ between libraries, cannot pass std::string across dll boundary

#if defined(BUILD_SDK_DLL)
#define NVWIDGETS_EXPORTS
#endif 

#include "nvWidgets\include\nvSdkWidgets.h"

#endif
