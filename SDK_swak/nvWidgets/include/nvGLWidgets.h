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

//
// nvGLWidgets.h - User Interface library
//
//
// Author: Ignacio Castano, Samuel Gateau, Evan Hart
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#ifndef NV_GL_WIDGETS_H
#define NV_GL_WIDGETS_H

#include "nvWidgets.h"
#include "../../sdk_swak.h"
#include <vector>

namespace nv
{

    //*************************************************************************
    // GLUIDeviceOne : The GL implementation of the UIDeviceOne to display 
    //                 StyleOne in a GL application
    class  GLUIDevice : public UIDeviceOne
    {
    public:

        NVSDKENTRY void init();

        NVSDKENTRY virtual int getFontHeight() const;
        NVSDKENTRY virtual int getTextLineWidth(const char * text) const;
        NVSDKENTRY virtual int getTextSize(const char * text, int& nbLines) const;
        NVSDKENTRY virtual int getTextLineWidthAt(const char * text, int charNb) const;
        NVSDKENTRY virtual int getPickedCharNb(const char * text, const Point& at) const;
        NVSDKENTRY virtual void drawString( int x, int y, const char * text, int nbLines, int fillColorId  );
                   
        NVSDKENTRY virtual void begin( const Rect& window );
        NVSDKENTRY virtual void end();
                   
        NVSDKENTRY virtual void pushVertex( Vertex* v ) const;
        NVSDKENTRY virtual void beginStrip() const;
        NVSDKENTRY virtual void endStrip() const;
                   
        NVSDKENTRY virtual void drawTexture(const Rect & rect, const void* texID, const Rect& rt, const Rect & rz, int mipLevel, 
                                            const nv::vec4f& texelScale, const nv::vec4f& texelOffset, const nv::vec4i& texelSwizzling );
                   
        NVSDKENTRY virtual void pushLayer();
        NVSDKENTRY virtual void popLayer();
        
        NVSDKENTRY GLUIDevice();
        NVSDKENTRY ~GLUIDevice();

        GLuint m_setupStateDL;
        GLuint m_restoreStateDL;
        GLuint m_textListBase;
        GLuint m_foregroundDL;

		GLuint m_widgetPipeline;
		GLuint m_restorePipeline;
        GLuint m_widgetVShader;
		GLuint m_widgetFShader;
        GLuint m_colorsUniform;

        GLuint m_textureViewFShader;
        GLuint m_texMipLevelUniform;
        GLuint m_texelScaleUniform;
        GLuint m_texelOffsetUniform;
        GLuint m_texelSwizzlingUniform;

        vec4f m_colors[UIStyleOne::cNbColors];

        mutable std::vector< Vertex > m_vertices;
        mutable int m_drawLayer;
        mutable std::vector< mutable std::vector< unsigned int > > m_indices;

    };
};

#endif 
