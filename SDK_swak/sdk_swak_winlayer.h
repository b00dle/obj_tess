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

#ifndef SDK_SWAK_WINLAYER_H
#define SDK_SWAK_WINLAYER_H

namespace nv
{
	//keyboard support
	namespace KeyCode
	{
		enum KeyCode
		{
			Key_Nul = 0,

			Key_A = 'a',  Key_B = 'b',  Key_C = 'c',  Key_D = 'd',  Key_E = 'e',
			Key_F = 'f',  Key_G = 'g',  Key_H = 'h',  Key_I = 'i',  Key_J = 'j',
			Key_K = 'k',  Key_L = 'l',  Key_M = 'm',  Key_N = 'n',  Key_O = 'o',
			Key_P = 'p',  Key_Q = 'q',  Key_R = 'r',  Key_S = 's',  Key_T = 't',
			Key_U = 'u',  Key_V = 'v',  Key_W = 'w',  Key_X = 'x',  Key_Y = 'y',
			Key_Z = 'z',

			Key_0 = '0',  Key_1 = '1',  Key_2 = '2',  Key_3 = '3',  Key_4 = '4',
			Key_5 = '5',  Key_6 = '6',  Key_7 = '7',  Key_8 = '8',  Key_9 = '9',

			// Special chars
			Key_Space = ' ', Key_Escape = '\033',

			// Need to add punctuation keys

			// Numpad
			Key_Kp0 = 0x300, Key_Kp1 = 0x301, Key_Kp2 = 0x302, Key_Kp3 = 0x303,
			Key_Kp4 = 0x304, Key_Kp5 = 0x305, Key_Kp6 = 0x306, Key_Kp7 = 0x307,
			Key_Kp8 = 0x308, Key_Kp9 = 0x309,

			// Function keys
			Key_F1  = 0x400,  Key_F2  = 0x401,  Key_F3  = 0x402,  Key_F4  = 0x403,
			Key_F5  = 0x404,  Key_F6  = 0x405,  Key_F7  = 0x406,  Key_F8  = 0x407,
			Key_F9  = 0x408,  Key_F10 = 0x409,  Key_F11 = 0x40a,  Key_F12 = 0x40b,

			// Arrow keys
			Key_Up = 0x500, Key_Down = 0x501, Key_Left = 0x502, Key_Right = 0x503,


		};
	};

	namespace MouseButton
	{
		enum MouseButton
		{
			Mouse_Left,
			Mouse_Right,
			Mouse_Middle,
			Mouse_Aux0,
			Mouse_Aux1,
			Mouse_Aux2,
			Mouse_Aux3,
			Mouse_Aux4
		};
	};

	namespace ModifierKey
	{
		enum ModifierKey
		{
			None     = 0x00000000,

			Shift    = 0x00000001,
			Control  = 0x00000002,
			Alt      = 0x00000004,
			LShift   = 0x00000101,
			LControl = 0x00000102,
			LAlt     = 0x00000104,
			RShift   = 0x00000201,
			RControl = 0x00000202,
			RAlt     = 0x00000204,

			ForceUInt = 0xffffffff
		};
	};

	SDK_API bool InitWindowSystem();
	SDK_API void ShutdownWindowSystem();

	// creates the window for operation
	SDK_API void CreateWindow();

	SDK_API void DestroyWindow();

	SDK_API void SwapBuffers();
	SDK_API void SwapInterval( int interval);

	//callbacks
	SDK_API void RegisterKeydownCallback( void (*fn)(KeyCode::KeyCode kc) );
	SDK_API void RegisterKeyupCallback( void (*fn)(KeyCode::KeyCode kc) );
	SDK_API void RegisterResizeCallback( void (*fn)(int x, int y) );
	SDK_API void RegisterMouseButtonCallback( void (*fn)( MouseButton::MouseButton b, bool down));
	SDK_API void RegisterMousePositionCallback( void (*fn)( int x,int y));
	SDK_API void RegisterMouseWheelCallback( void (*fn)( int n));

	//Note these callbacks only apply to the looped mode
	SDK_API void RegisterDisplayCallback(void (*fn)());
	SDK_API void RegisterIdleCallback(void (*fn)());

	// user-interface queries
	SDK_API void GetMousePosition( int &x, int &y);
	SDK_API bool GetMouseButtonState( MouseButton::MouseButton b);
	SDK_API bool GetKeyState( KeyCode::KeyCode kc);
	SDK_API void GetWindowSize( int &width, int &height);
	SDK_API int GetMouseWheelPosition();

	SDK_API ModifierKey::ModifierKey GetModifierKeyState();


	//loop mode
	SDK_API void RunLoop();
	SDK_API void TerminateLoop();
	SDK_API void ForceRefresh();
};

#endif
