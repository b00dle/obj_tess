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
// nvSdkWidgets
//
//  Adaptor classes to integrate the nvWidgets UI library with the SDK windowing
// toolkit. The adaptors convert native SDK UI data to native nvWidgets data. All
// adaptor classes are implemented as in-line code in this header. The adaptor
// defaults to using the standard OpenGL paintor implementation.
//
// Author: Ignacio Castano, Samuel Gateau, Evan Hart
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef NV_SDK_WIDGETS_H
#define NV_SDK_WIDGETS_H

#include "../../sdk_swak.h"
#include "nvGLWidgets.h"


namespace nv {

    class SdkUIContext : public UIContext {

    protected:

        bool _ownStyle;

    public:

        //
        // Default UI constructor
        //
        //  Creates private OpenGL style
        //////////////////////////////////////////////////////////////////
        SdkUIContext() :
        nv::UIContext( *( new nv::UIStyleOne(new nv::GLUIDevice()))),
					
        _ownStyle(true) 
        {
			new nv::GLUIDevice();
        }

        //
        // Alternate UI constructor
        //
        //  Allows for overriding the standard style
        //////////////////////////////////////////////////////////////////
        SdkUIContext(UIStyle& style) : 
            UIContext( style ),
            _ownStyle(false)
        {
        }

        //
        // UI  destructor
        //
        //  Destroy style if it is private
        //////////////////////////////////////////////////////////////////
        ~SdkUIContext() {
            if (_ownStyle) delete getStyle();
        }

        //
        // UI method for processing GLUT mouse button events
        //
        //  Call this method from the glutMouseFunc callback, the
        // modifier parameter maps to glutGetModifiers.
        //////////////////////////////////////////////////////////////////
        virtual void mouse(int button, int state, int modifier, int x, int y) { 
            int modifierMask = 0;


            if ( modifier & nv::ModifierKey::Alt) modifierMask |= ButtonFlags_Alt;
            if ( modifier & nv::ModifierKey::Shift) modifierMask |= ButtonFlags_Shift;
            if ( modifier & nv::ModifierKey::Control) modifierMask |= ButtonFlags_Ctrl;

            if ( state == 1) state = 1; else state = 0;

            UIContext::mouse( button, state, modifierMask, x, y);
        }
        
        void mouse(int button, int state, int x, int y) {
            mouse(button, state, nv::GetModifierKeyState(), x, y);
        }

        //
        // UI method for processing key events
        //
        //  Call this method from the glutReshapeFunc callback
        //////////////////////////////////////////////////////////////////
        void specialKeyboard(int k, int x, int y) { UIContext::keyboard( translateKey(k), x, y); }

        //
        //  Translate non-ascii keys from GLUT to nvWidgets
        //////////////////////////////////////////////////////////////////
        unsigned char translateKey( int k )
        {
            switch (k)
            {
			case nv::KeyCode::Key_F1 :
                return Key_F1;
            case nv::KeyCode::Key_F2 :
                return Key_F2;
            case nv::KeyCode::Key_F3 :
                return Key_F3;
            case nv::KeyCode::Key_F4 :
                return Key_F4;
            case nv::KeyCode::Key_F5 :
                return Key_F5;
            case nv::KeyCode::Key_F6 :
                return Key_F6;
            case nv::KeyCode::Key_F7 :
                return Key_F7;
            case nv::KeyCode::Key_F8 :
                return Key_F8;
            case nv::KeyCode::Key_F9 :
                return Key_F9;
            case nv::KeyCode::Key_F10 :
                return Key_F10;
            case nv::KeyCode::Key_F11 :
                return Key_F11;
            case nv::KeyCode::Key_F12 :
                return Key_F12;
			case nv::KeyCode::Key_Left :
                return Key_Left;
            case nv::KeyCode::Key_Up :
                return Key_Up;
            case nv::KeyCode::Key_Right :
                return Key_Right;
            case nv::KeyCode::Key_Down :
                return Key_Down;
				/*
            case GLUT_KEY_PAGE_UP :
                return Key_PageUp;
            case GLUT_KEY_PAGE_DOWN :
                return Key_PageDown;
            case GLUT_KEY_HOME :
                return Key_Home;
            case GLUT_KEY_END :
                return Key_End;
            case GLUT_KEY_INSERT :
                return Key_Insert;
				*/
            default:
                return 0;
            } 
        }

		void doCVar( nv::CVar<bool>& var)
		{
			doCheckButton( nv::Rect(), var._name, &var._val,1);
		}

		void doCVar( nv::CRefVar<bool>& var)
		{
			doCheckButton( nv::Rect(), var._name, &var._val,1);
		}

		void doCVar( nv::CVar<float>& var)
		{
			if (var._clamped)
			{
				beginGroup(nv::GroupFlags_LayoutHorizontal | nv::GroupFlags_LayoutNoMargin );
        
			
				doHorizontalSlider( nv::Rect(), var._minVal, var._maxVal, &var._val);

				{
					char temp[256];
					sprintf( temp, "%s = %3.2f", var._name, var._val);
					doLabel( nv::Rect(), temp, 1);
				}

				endGroup();
			}
		}

		void doCVar( nv::CRefVar<float>& var)
		{
			if (var._clamped)
			{
				beginGroup(nv::GroupFlags_LayoutHorizontal | nv::GroupFlags_LayoutNoMargin );
        
			
				doHorizontalSlider( nv::Rect(), var._minVal, var._maxVal, &var._val);

				{
					char temp[256];
					sprintf( temp, "%s = %3.2f", var._name, var._val);
					doLabel( nv::Rect(), temp, 1);
				}

				endGroup();
			}
		}


        
        
    private:


    };
};



#endif
