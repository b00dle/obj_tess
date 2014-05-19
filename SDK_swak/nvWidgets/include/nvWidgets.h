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
// nvWidgets.h - User Interface library
//
//
// Author: Ignacio Castano, Samuel Gateau, Evan Hart
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
////////////////////////////////////////////////////////////////////////////////
#ifndef NV_WIDGETS_H
#define NV_WIDGETS_H

#include <time.h>
#include "../../include/nvMath.h"

#ifdef WIN32
#ifndef NVSDKENTRY
#ifdef NVWIDGETS_EXPORTS
#define NVSDKENTRY __declspec(dllexport)
#else
#define NVSDKENTRY __declspec(dllimport)
#endif
#endif
#if _MSC_VER >= 1400  
# pragma warning(disable:4996) 
#endif 
#endif


namespace nv
{
    struct Point
    {
        NVSDKENTRY Point() : x(0.f), y(0.f) {}
        NVSDKENTRY explicit Point(int ix, int iy) : x(static_cast<float> (ix)), y(static_cast<float> (iy)) {}
        NVSDKENTRY explicit Point(float fx, float fy) : x(fx), y(fy) {}
        NVSDKENTRY Point(const Point & p) : x(p.x), y(p.y) {}

        NVSDKENTRY const Point& operator= (const Point & p) { this->x = p.x; this->y = p.y; return *this; }

        float x, y;
    };

    struct Rect
    {
        NVSDKENTRY Rect() : x(0.f), y(0.f), w(0.f), h(0.f) {}
        NVSDKENTRY Rect(const Point & p) : x(p.x), y(p.y), w(0.f), h(0.f) {}
        NVSDKENTRY explicit Rect(int ix, int iy, int iw = 0, int ih = 0) : x(static_cast<float> (ix)), y(static_cast<float> (iy)), w(static_cast<float> (iw)), h(static_cast<float> (ih)) {}
        NVSDKENTRY explicit Rect(float fx, float fy, float fw = 0.f, float fh = 0.f) : x(fx), y(fy), w(fw), h(fh) {}
        NVSDKENTRY Rect(const Rect & r) : x(r.x), y(r.y), w(r.w), h(r.h) {}

        NVSDKENTRY const Rect& operator= (const Rect & r) { this->x = r.x; this->y = r.y; this->w = r.w; this->h = r.h; return *this; }

        float x, y;
        float w, h;

        NVSDKENTRY static const Rect null;
    };

    enum ButtonFlags
    {
        ButtonFlags_Off = 0x0,
        ButtonFlags_On = 0x1,
        ButtonFlags_Begin = 0x2,
        ButtonFlags_End = 0x4,
        ButtonFlags_Shift = 0x8,
        ButtonFlags_Alt = 0x10,
        ButtonFlags_Ctrl = 0x20,
    };

    struct ButtonState
    {
        int state;
        //time_t time;
        __int64 time;
        Point cursor;
    };

	/*
    // An enum to identify the mouse buttons
    enum MouseButton
    {
        MouseButton_Left,
        MouseButton_Middle,
        MouseButton_Right,
    };
	*/

    // An enum to identify the special key buttons not translated with ASCII codes
    enum Key
    {
        Key_F1 = 128,
        Key_F2,
        Key_F3,
        Key_F4,
        Key_F5,
        Key_F6,
        Key_F7,
        Key_F8,
        Key_F9,
        Key_F10,
        Key_F11,
        Key_F12,

        Key_Left,
        Key_Up,
        Key_Right,
        Key_Down,
        Key_PageUp,
        Key_PageDown,
        Key_Home,
        Key_End,
        Key_Insert,
    };

     // The various flags to modify the behavior of the groups   
    enum GroupFlags
    {
        // Layout behavior flags
        GroupFlags_LayoutNone = 0x01,
        GroupFlags_LayoutVertical = 0x02,
        GroupFlags_LayoutHorizontal = 0x04,
        GroupFlags_LayoutMask = 0x07,
        GroupFlags_LayoutXMask = 0xffff ^ GroupFlags_LayoutMask,

        // Alignment flags for the widgets inserted in the group
        GroupFlags_AlignLeft = 0x10,
        GroupFlags_AlignRight = 0x20,
        GroupFlags_AlignTop = 0x40,
        GroupFlags_AlignBottom = 0x80,
        GroupFlags_AlignMask = 0xf0,
        GroupFlags_AlignXMask = 0xffff ^ GroupFlags_AlignMask,

        // Start flags defining the starting origin of the group
        GroupFlags_StartLeft = 0x100,
        GroupFlags_StartRight = 0x200,
        GroupFlags_StartTop = 0x400,
        GroupFlags_StartBottom = 0x800,
        GroupFlags_StartMask = 0xf00,
        GroupFlags_StartXMask = 0xffff ^ GroupFlags_StartMask,

        // Optional flags
        GroupFlags_LayoutForce = 0x8000,
        GroupFlags_LayoutDefault = 0x4000,
        GroupFlags_LayoutNoMargin = 0x2000,
        GroupFlags_LayoutNoSpace = 0x1000,

        // Predefined configurations
        GroupFlags_GrowRightFromBottom   = GroupFlags_LayoutHorizontal    | GroupFlags_StartLeft   | GroupFlags_AlignLeft   | GroupFlags_StartBottom | GroupFlags_AlignBottom ,
        GroupFlags_GrowRightFromTop      = GroupFlags_LayoutHorizontal    | GroupFlags_StartLeft   | GroupFlags_AlignLeft   | GroupFlags_StartTop    | GroupFlags_AlignTop ,
        GroupFlags_GrowLeftFromBottom    = GroupFlags_LayoutHorizontal    | GroupFlags_StartRight  | GroupFlags_AlignRight  | GroupFlags_StartBottom | GroupFlags_AlignBottom,
        GroupFlags_GrowLeftFromTop       = GroupFlags_LayoutHorizontal    | GroupFlags_StartRight  | GroupFlags_AlignRight  | GroupFlags_StartTop    | GroupFlags_AlignTop,
        GroupFlags_GrowUpFromLeft        = GroupFlags_LayoutVertical      | GroupFlags_StartBottom | GroupFlags_AlignBottom | GroupFlags_StartLeft   | GroupFlags_AlignLeft ,
        GroupFlags_GrowUpFromRight       = GroupFlags_LayoutVertical      | GroupFlags_StartBottom | GroupFlags_AlignBottom | GroupFlags_StartRight  | GroupFlags_AlignRight ,
        GroupFlags_GrowDownFromLeft      = GroupFlags_LayoutVertical      | GroupFlags_StartTop    | GroupFlags_AlignTop    | GroupFlags_StartLeft   | GroupFlags_AlignLeft ,
        GroupFlags_GrowDownFromRight     = GroupFlags_LayoutVertical      | GroupFlags_StartTop    | GroupFlags_AlignTop    | GroupFlags_StartRight  | GroupFlags_AlignRight ,

        GroupFlags_LayoutDefaultFallback = GroupFlags_GrowDownFromLeft,
    };

    struct Group
    {
        Rect bounds;  // anchor point + width and height of the region
        int flags;   // group behavior 
        int margin; // border 
        int space;  // interior
    };

    //*************************************************************************
    // UIStyle
    class UIStyle
    {
    public:
        NVSDKENTRY UIStyle() {}

        NVSDKENTRY virtual void begin( const Rect& window ) = 0;
        NVSDKENTRY virtual void end() = 0;

        // These methods should be called between begin/end

        NVSDKENTRY virtual void drawFrame(const Rect & r, int margin, int style) = 0;

        NVSDKENTRY virtual Rect getLabelRect(const Rect & r, const char * text, Rect & rt, int& nbLines) const = 0;
        NVSDKENTRY virtual void drawLabel(const Rect & r, const char * text, const Rect & rt, const int& nbLines, bool isHover, int style) = 0;
      
        NVSDKENTRY virtual Rect getButtonRect(const Rect & r, const char * text, Rect & rt) const = 0;
        NVSDKENTRY virtual void drawButton(const Rect & r, const char * text, const Rect & rt, bool isDown, bool isHover, bool isFocus, int style) = 0;
    
        NVSDKENTRY virtual Rect getCheckRect(const Rect & r, const char * text, Rect & rt, Rect & rc) const = 0;
        NVSDKENTRY virtual void drawCheckButton(const Rect & r, const char * text, const Rect & rt, const Rect & rr, bool isChecked, bool isHover, bool isFocus, int style) = 0;

        NVSDKENTRY virtual Rect getRadioRect(const Rect & r, const char * text, Rect & rt, Rect & rr) const = 0;
        NVSDKENTRY virtual void drawRadioButton(const Rect & r, const char * text, const Rect & rt, const Rect & rr, bool isOn, bool isHover, bool isFocus, int style) = 0;

        NVSDKENTRY virtual Rect getHorizontalSliderRect(const Rect & r, Rect& rs, float v, Rect& rc) const = 0;
        NVSDKENTRY virtual void drawHorizontalSlider(const Rect & r, Rect& rs, float v, Rect& rc, bool isHover, int style) = 0;

        NVSDKENTRY virtual Rect getHorizontalProgressbarRect(const Rect & r, Rect& rs, float v, Rect& rc) const = 0;
        NVSDKENTRY virtual void drawHorizontalProgressbar(const Rect & r, Rect& rs, float v, Rect& rc, bool isHover, int style) = 0;

        NVSDKENTRY virtual Rect getItemRect(const Rect & r, const char * text, Rect & rt) const = 0;
        NVSDKENTRY virtual void drawListItem(const Rect & r, const char * text, const Rect & rt, bool isSelected, bool isHover, int style) = 0;

        NVSDKENTRY virtual Rect getListRect(const Rect & r, int numOptions, const char * options[], Rect& ri, Rect & rt) const = 0;
        NVSDKENTRY virtual void drawListBox(const Rect & r, int numOptions, const char * options[], const Rect& ri, const Rect & rt, int selected, int hovered, int style) = 0;
      
        NVSDKENTRY virtual Rect getComboRect(const Rect & r, int numOptions, const char * options[], int selected, Rect& rt, Rect& ra) const = 0;
        NVSDKENTRY virtual Rect getComboOptionsRect(const Rect & rCombo, int numOptions, const char * options[], Rect& ri, Rect & rit) const = 0;
        NVSDKENTRY virtual void drawComboBox(const Rect & rect, int numOptions, const char * options[], const Rect & rt, const Rect & ra, int selected, bool isHover, bool isFocus, int style) = 0;
        NVSDKENTRY virtual void drawComboOptions(const Rect & rect, int numOptions, const char * options[], const Rect& ri, const Rect & rit, int selected, int hovered, bool isHover, bool isFocus, int style) = 0;

        NVSDKENTRY virtual Rect getLineEditRect(const Rect & r, const char * text, Rect & rt) const = 0;
        NVSDKENTRY virtual void drawLineEdit(const Rect & r, const char * text, const Rect & rt, int caretPos, bool isSelected, bool isHover, int style) = 0;

        NVSDKENTRY virtual Rect getPanelRect(const Rect & r, const char * text, Rect& rt, Rect& ra) const = 0;
        NVSDKENTRY virtual void drawPanel(const Rect & rect, const char * text, const Rect & rt, const Rect & ra, bool isUnfold, bool isHover, bool isFocus, int style) = 0;

        NVSDKENTRY virtual Rect getTextureViewRect(const Rect & rect, Rect& rt) const = 0;
        NVSDKENTRY virtual void drawTextureView(const Rect & rect, const void* texID, const Rect& rt, const Rect & rz, int mipLevel, 
                                                const nv::vec4f& texelScale, const nv::vec4f& texelOffset, const nv::vec4i& texelSwizzling,
                                                int style) = 0;

        // Eval widget dimensions
        NVSDKENTRY virtual int getCanvasMargin() const = 0;
        NVSDKENTRY virtual int getCanvasSpace() const = 0;
        NVSDKENTRY virtual int getPickedCharNb(const char * text, const Point& at) const = 0;

    protected:
    };


    class UIContext
    {
    public:
        NVSDKENTRY UIContext( UIStyle& style );

        //
        // UI method for processing window size events
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY void reshape(int w, int h);
       
        //
        // Check if the UI is currently on Focus
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY bool isOnFocus() const { return m_uiOnFocus; }

        //
        // UI method for processing mouse events
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY void mouse(int button, int state, int modifier, int x, int y);
        NVSDKENTRY void mouse(int button, int state, int x, int y) { mouse( button, state, 0, x, y); }
 
        //
        // UI method for processing mouse motion events
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY void mouseMotion(int x, int y);

        //
        // UI method for processing key events
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY void keyboard(unsigned char k, int x, int y);

        //
        // UI method for entering UI processing mode
        //
        //  This function must be used to begin the UI processing
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY void begin();

        //
        // UI method for leaving UI processing mode
        //
        //  This function must be used to end the UI processing
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY void end();

        ////////////////////////////////////////////////////////////////////////////
        //
        // UI element processing
        //
        // The following methods provide the interface for rendering and querying
        // UI objects. These methods must be called between begin/end.
        ////////////////////////////////////////////////////////////////////////////

        //
        // UI method for drawing a static text label
        // The label display a non interactive text.
        // The text can have multiple lines
        //
        // rect - optionally provides a location and size for the label
        // text - Text to display for the label (can have several lines)
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY void doLabel(const Rect & rect, const char * text, int style = 0);

        //
        // UI method for rendering and processing a push button
        //
        // rect - optionally provides a location and size for the button
        // text - text to display on the button
        // state -  whether the button is depressed
        //          if state is NULL, the buttoin behave like a touch button
        //          else, the button behave like a toggle button
        // style - optional style flag to modify the look
        //
        // @return  True if the button'state changed
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY bool doButton(const Rect & rect, const char * text, bool * state = 0, int style = 0);
        
        //
        // UI method for rendering and processing a check button
        // Check button behaves similarly as a toggle button 
        // used to display and edit a bool property.
        //
        // rect - optionally provides a location and size for the button
        // text - text to display on the button
        // state -  whether the check button is checked or not
        //          if state is NULL, the buttoin behave like if a touch button unchecked
        // style - optional style flag to modify the look
        //
        // @return  True if the check button'state changed
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY bool doCheckButton(const Rect & rect, const char * text, bool * state, int style = 0);

        //
        // UI method for rendering and processing a radio button
        // Radio buttons are typically used in groups to diplay and edit 
        // the possible reference values taken by an int value.
        //
        // One radio button is representing a possible reference value taken by the current value.
        // It is displaying a boolean state true if the current value is equal to the reference value.
        //
        // reference - The reference int value represented by this radio button.
        // rect - optionally provides a location and size for the button
        // text - text to display on the button
        // value -  The value parameter compared to the reference value parameter.
        //          if value is NULL, the radio button is off
        // style - optional style flag to modify the look
        //
        // @return  True if the radio button's value changed
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY bool doRadioButton(int reference, const Rect & r, const char * text, int * value, int style = 0);

        //
        // UI method for rendering and processing a horizontal slider
        // Horizontal slider is used to edit and display a scalar value in the specified range [min, max].
        //
        // rect - optionally provides a location and size for the widget
        // min - min bound of the varying range of the value
        // max - max bound of the varying range of the value
        // value -  the value edited byt the widget
        //          if value is NULL, the value is set to min
        // style - optional style flag to modify the look
        //
        // @return  True if the slider's value changed
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY bool doHorizontalSlider(const Rect & rect, float min, float max, float * value, int style = 0);
        NVSDKENTRY bool doHorizontalProgressbar(const Rect & rect, float min, float max, float * value, int style = 0);

        NVSDKENTRY bool doListItem(int index, const Rect & rect, const char * text, int * selected, int style = 0);
        NVSDKENTRY bool doListBox(const Rect & rect, int numOptions, const char * options[], int * selected, int style = 0);

        NVSDKENTRY bool doComboBox(const Rect & rect, int numOptions, const char * options[], int * selected, int style = 0);

        NVSDKENTRY bool doLineEdit(const Rect & rect, char * text, int maxTextLength, int * nbCharsReturned, int style = 0);

        //
        // UI method for rendering and processing a group of widgets
        // The group is just a logical groupment of widgets allowing to 
        // create a specific layout for the ui within the group.
        //
        // Start the group by calling "beginGroup"
        // Then add all the other ui items with standard calls
        // Finally close the group calling "endGroup"
        //
        // groupFlags - optionally defines the layout behavior for the group
        // rect - optionally provides a location and size for the widget
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY void beginGroup(int groupFlags = GroupFlags_LayoutDefault, const Rect& rect = Rect::null);
        NVSDKENTRY void endGroup();

        //
        // UI method for rendering and processing a frame of widgets
        // A frame is a group with extra decorations
        //
        // Start the frame by calling "beginFrame"
        // Then add all the other ui items with standard calls
        // Finally close the group calling "endFrame"
        //
        // groupFlags - optionally defines the layout behavior for the group
        // rect - optionally provides a location and size for the widget
        // style - optional style flag to modify the look
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY void beginFrame(int groupFlags = GroupFlags_LayoutDefault, const Rect& rect = Rect::null, int style = 0);
        NVSDKENTRY void endFrame();

        //
        // UI method for rendering and processing a foldable panel
        // A panel is composed of a header button and a frame.
        // When fold, only the header button is visible, acting like a regular button.
        // When unfold, both the header button and the frame are visible.
        //
        // Start the panel by calling "beginPanel" and checking the returned value.
        // If Unfold then the frame automatically start.
        // fill the frame with the widgets to be displayed
        // before leaving the if block, close the panel's frame by calling "endPanel".
        //
        //      // Here is a standard panel usage
        //      static bool isUnfold = false;
        //      if ( beginPanel( nv::Rect(), "The panel name", &isUnfold ) )
        //      {
        //          //Add whatever widgets in the frame of the panel
        //
        //          endPanel();
        //      }
        //
        // The header button is composed of 2 areas, the label with the name and the fold/unfold button.
        // The panel can be dragged around in the window by clicking on the label area of the header button.
        // For this behavior to work well, make sure to begin the panel in a Group with no auto position 
        // and to use a rect parameter which is persistent from frame to frame.
        //
        //      // Here is a draggable panel
        //      static bool isUnfold = false;
        //      static Rect panelRect;
        //      if ( beginPanel( panelRect, "The panel name", &isUnfold ) )
        //      {
        //          //Add whatever widgets in the frame of the panel
        //
        //          endPanel();
        //      }
        //
        // rect - optionally provides a location and size for the widget
        // text - the name of the panel displayed in the header button
        // isUnfold - the fold/unfold state, the value is equal to the returned value of the beginPanel call.
        // groupFlags - the group flags applyed to manage the layout inside the frame of the panel
        // style - optional style flag to modify the look
        //
        // @return  True if the panel is unfolded
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY bool beginPanel(Rect & rect, const char * text, bool * isUnfold, int groupFlags = GroupFlags_LayoutDefault, int style = 0);
        NVSDKENTRY void endPanel();

        NVSDKENTRY int getGroupWidth() { return static_cast<int> (m_groupStack[m_groupIndex].bounds.w); }
        NVSDKENTRY int getGroupHeight() { return static_cast<int> (m_groupStack[m_groupIndex].bounds.h); }

        NVSDKENTRY int getCursorX() { return static_cast<int> (m_currentCursor.x);}
        NVSDKENTRY int getCursorY() { return static_cast<int> (m_currentCursor.y);}

        NVSDKENTRY const ButtonState& getMouseState( int button) { return m_mouseButton[button]; }

        //
        // UI method for drawing a texture view
        // Several parameters control the equation used to display the texel
        //      texel = texture2DFetch(...);
        //      pixel = texelSwizzling( texel * texelScale + texelOffset );
        //
        // rect - provides a location and size for the texture view
        // texID - texture identifier (Graphics API specific)
        // zoomRect - rectangle area of the texture displayed
        // mipLevel - mip Level of the texture displayed
        // texelScale - scale applyed to the texel fetch
        // texelOffset - offset applyed to the texel after scale
        // texelSwizzling - swizzle applyed to the texel (after scale and offset)
        // style - optional style flag to modify the look
        //////////////////////////////////////////////////////////////////
        NVSDKENTRY void doTextureView(  const Rect & rect, const void* texID, Rect & zoomRect, int mipLevel = -1, 
                                        const nv::vec4f& texelScale = nv::vec4f(1.0, 1.0, 1.0, 1.0), 
                                        const nv::vec4f& texelOffset = nv::vec4f(0.0, 0.0, 0.0, 0.0), 
                                        const nv::vec4i& texelSwizzling = nv::vec4i(0, 1, 2, 3),
                                        int style = 0);

    protected:
        NVSDKENTRY UIStyle* getStyle() { return m_style; }

        const Rect  & window() const { return m_window; }

    private:
        NVSDKENTRY void setCursor(int x, int y);

        NVSDKENTRY static bool overlap(const Rect & rect, const Point & p);

        NVSDKENTRY bool hasFocus(const Rect & rect);
        NVSDKENTRY bool isHover(const Rect & rect);

        NVSDKENTRY Rect placeRect(const Rect & r);

    private:
        UIStyle * m_style;
        
        int m_groupIndex;
        Group m_groupStack[64];

        Rect m_window;

        Point m_currentCursor;
        ButtonState m_mouseButton[3];
        unsigned char m_keyBuffer[32];
        int m_nbKeys;

        int m_focusCaretPos;
        Point m_focusPoint;
        bool m_twoStepFocus;
        bool m_uiOnFocus;
    };



    //*************************************************************************
    // StyleOne : This is the first Style implementation we did,
    //            hence it's name.
    //            These are all the components for the style
    //            This is the style used by default.
    //*************************************************************************

    //*************************************************************************
    // UIDeviceOne
    // The abstract interface used by the UIStyleOne
    // to effectively draw the widgets
    class  UIDeviceOne
    {
    public:

        // The vertex class
        // When drawing the widgets elements, all shapes, push new vertices
        struct Vertex
        {
            float x, y;
            float u, v;
            float ex, ey;
            float m_turn;
            float m_radius;
            unsigned char c0, c1, m, spare;

            Vertex() :
                x(0), y(0), u(0), v(0),
                ex(0), ey(0), m_turn(0), 
                m_radius(0),
                c0(0), c1(0), m(0)
                {}

            void xy( float fx, float fy )
            {
                x = fx; y = fy;
            }

            void xyuv( float fx, float fy, float fu, float fv )
            {
                x = fx; y = fy; u = fu; v = fv;
            }

            void exey( float fex, float fey )
            {
                ex = fex; ey = fey;
            }

            void exeyt( float fex, float fey, float ft )
            {
                ex = fex; ey = fey; m_turn = ft;
            }
        };
    
        NVSDKENTRY virtual int getFontHeight() const = 0;
        NVSDKENTRY virtual int getTextLineWidth(const char * text) const = 0;
        NVSDKENTRY virtual int getTextSize(const char * text, int& nbLines) const = 0;
        NVSDKENTRY virtual int getTextLineWidthAt(const char * text, int charNb) const = 0;
        NVSDKENTRY virtual int getPickedCharNb(const char * text, const Point& at) const = 0;
        NVSDKENTRY virtual void drawString( int x, int y, const char * text, int nbLines, int fillColorId  ) = 0;

        NVSDKENTRY virtual void begin( const Rect& window ) = 0;
        NVSDKENTRY virtual void end() = 0;

        NVSDKENTRY virtual void pushVertex( Vertex* v ) const = 0;
        NVSDKENTRY virtual void beginStrip() const = 0;
        NVSDKENTRY virtual void endStrip() const = 0;

        NVSDKENTRY virtual void drawTexture(const Rect & rect, const void* texID, const Rect& rt, const Rect & rz, int mipLevel, 
                                    const nv::vec4f& texelScale, const nv::vec4f& texelOffset, const nv::vec4i& texelSwizzling ) = 0;

        NVSDKENTRY virtual void pushLayer() = 0;
        NVSDKENTRY virtual void popLayer() = 0;
    };

    //*************************************************************************
    // UIStyleOne
    class UIStyleOne : public UIStyle
    {
        typedef UIDeviceOne::Vertex Vertex;

    public:

        NVSDKENTRY UIStyleOne(UIDeviceOne* device);
        NVSDKENTRY ~UIStyleOne();

        NVSDKENTRY virtual void begin( const Rect& window );
        NVSDKENTRY virtual void end();

        // These methods should be called between begin/end
        NVSDKENTRY virtual void drawFrame(const Rect & r, int margin, int style);

        NVSDKENTRY virtual Rect getLabelRect(const Rect & r, const char * text, Rect & rt, int& nbLines) const;
        NVSDKENTRY virtual void drawLabel(const Rect & r, const char * text, const Rect & rt, const int& nbLines, bool isHover, int style);
     
        NVSDKENTRY virtual Rect getButtonRect(const Rect & r, const char * text, Rect & rt) const;
        NVSDKENTRY virtual void drawButton(const Rect & r, const char * text, const Rect & rt, bool isDown, bool isHover, bool isFocus, int style);
    
        NVSDKENTRY virtual Rect getCheckRect(const Rect & r, const char * text, Rect & rt, Rect & rc) const;
        NVSDKENTRY virtual void drawCheckButton(const Rect & r, const char * text, const Rect & rt, const Rect & rr, bool isChecked, bool isHover, bool isFocus, int style);

        NVSDKENTRY virtual Rect getRadioRect(const Rect & r, const char * text, Rect & rt, Rect & rr) const;
        NVSDKENTRY virtual void drawRadioButton(const Rect & r, const char * text, const Rect & rt, const Rect & rr, bool isOn, bool isHover, bool isFocus, int style);

        NVSDKENTRY virtual Rect getHorizontalSliderRect(const Rect & r, Rect& rs, float v, Rect& rc) const;
        NVSDKENTRY virtual void drawHorizontalSlider(const Rect & r, Rect& rs, float v, Rect& rc, bool isHover, int style);

        NVSDKENTRY virtual Rect getHorizontalProgressbarRect(const Rect & r, Rect& rs, float v, Rect& rc) const;
        NVSDKENTRY virtual void drawHorizontalProgressbar(const Rect & r, Rect& rs, float v, Rect& rc, bool isHover, int style);

        NVSDKENTRY virtual Rect getItemRect(const Rect & r, const char * text, Rect & rt) const;
        NVSDKENTRY virtual Rect getListRect(const Rect & r, int numOptions, const char * options[], Rect& ri, Rect & rt) const;
        NVSDKENTRY virtual void drawListItem(const Rect & r, const char * text, const Rect & rt, bool isSelected, bool isHover, int style);
        NVSDKENTRY virtual void drawListBox(const Rect & r, int numOptions, const char * options[], const Rect& ri, const Rect & rt, int selected, int hovered, int style);

        NVSDKENTRY virtual Rect getComboRect(const Rect & r, int numOptions, const char * options[], int selected, Rect& rt, Rect& ra) const;
        NVSDKENTRY virtual Rect getComboOptionsRect(const Rect & rCombo, int numOptions, const char * options[], Rect& ri, Rect & rit) const;
        NVSDKENTRY virtual void drawComboBox(const Rect & rect, int numOptions, const char * options[], const Rect & rt, const Rect& rd, int selected, bool isHover, bool isFocus, int style);
        NVSDKENTRY virtual void drawComboOptions(const Rect & rect, int numOptions, const char * options[], const Rect& ri, const Rect & rit, int selected, int hovered, bool isHover, bool isFocus, int style);

        NVSDKENTRY virtual Rect getLineEditRect(const Rect & r, const char * text, Rect & rt) const;
        NVSDKENTRY virtual void drawLineEdit(const Rect & r, const char * text, const Rect & rt, int caretPos, bool isSelected, bool isHover, int style);

        NVSDKENTRY virtual Rect getPanelRect(const Rect & r, const char * text, Rect& rt, Rect& ra) const;
        NVSDKENTRY virtual void drawPanel(const Rect & rect, const char * text, const Rect & rt, const Rect & ra, bool isUnfold, bool isHover, bool isFocus, int style);

        NVSDKENTRY virtual Rect getTextureViewRect(const Rect & rect, Rect& rt) const;
        NVSDKENTRY virtual void drawTextureView(const Rect & rect, const void* texID, const Rect& rt, const Rect & rz, int mipLevel, 
                                                const nv::vec4f& texelScale, const nv::vec4f& texelOffset, const nv::vec4i& texelSwizzling, 
                                                int style);

        // Eval widget dimensions
        NVSDKENTRY virtual int getCanvasMargin() const;
        NVSDKENTRY virtual int getCanvasSpace() const;
        NVSDKENTRY virtual int getFontHeight() const;
        NVSDKENTRY virtual int getTextLineWidth(const char * text) const;
        NVSDKENTRY virtual int getTextSize(const char * text, int& nbLines) const;
        NVSDKENTRY virtual int getTextLineWidthAt(const char * text, int charNb) const;
        NVSDKENTRY virtual int getPickedCharNb(const char * text, const Point& at) const;

        NVSDKENTRY enum Color
        {
            cBase = 0,
            cBool = 8,
            cOutline = 16,
            cFont = 20,
            cFontBack = 24,
            cTranslucent = 32,
            cNbColors = 33,
        };

    protected:

        // Draw primitive shapes
        NVSDKENTRY void drawText( const Rect& r , const char * text, int nbLines = 1, int caretPos = -1, bool isHover = false, bool isOn = false, bool isFocus = false, bool outline = true );
        NVSDKENTRY void drawFrame( const Rect& rect, const Point& corner, bool isHover = false, bool isOn = false, bool isFocus = false, bool outline = true ) const;
        NVSDKENTRY void drawBoolFrame( const Rect& rect, const Point& corner, bool isHover = false, bool isOn = false, bool isFocus = false, bool outline = true ) const;

        NVSDKENTRY void drawRect( const Rect& rect, int fillColorId, int outlineColorId ) const;
        NVSDKENTRY void drawRoundedRect( const Rect& rect, const Point& corner, int fillColorId, int outlineColorId ) const;
        NVSDKENTRY void drawRoundedRectOutline( const Rect& rect, const Point& corner, int outlineColorId ) const;
        NVSDKENTRY void drawCircle( const Rect& rect, int fillColorId, int outlineColorId ) const;
        NVSDKENTRY void drawMinus( const Rect& rect, int width, int fillColorId, int outlineColorId ) const;
        NVSDKENTRY void drawPlus( const Rect& rect, int width, int fillColorId, int outlineColorId ) const;
        NVSDKENTRY void drawDownArrow( const Rect& rect, int width, int fillColorId, int outlineColorId ) const;
        NVSDKENTRY void drawUpArrow( const Rect& rect, int width, int fillColorId, int outlineColorId ) const;

    private:
        UIDeviceOne* m_device;
    };
};





#endif  // NV_WIDGETS_H
