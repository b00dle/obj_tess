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
#ifndef NV_CVAR_H
#define NV_CVAR_H

#include <vector>
namespace nv {
////////////////////////////////////////////////////////////////////////////////
//
// CVarBase
//
//  Base class for all CVar types. CVarBase provides the interface used by
// the CVarRegistry to perform generic processing. 
//
////////////////////////////////////////////////////////////////////////////////
struct CVarBase
{
	// Name of the property
	const char* _name;
	
	// Help text for describing the property
	const char* _text;

	// Key used in the interface for altering the property
	//  Presently <shift>+Key causes an increment, and Key causes a decrement
	const nv::KeyCode::KeyCode _key;

	// functions virtual to allow correct dispatching
	virtual void Increment() = 0;
	virtual void Decrement() = 0;

	CVarBase( const char* name, nv::KeyCode::KeyCode key, const char* text) :
	_name(name), _text(text), _key(key)
	{}

	// Note: no virtual destructor, as these are intended to be lightweight classes 
	// which do not allocate any memory
};

////////////////////////////////////////////////////////////////////////////////
//
// CVarRegistry
//
//  Singleton that manages dispatching messages to all the CVars. CVars are
// expected to register themselves on construction.
//
////////////////////////////////////////////////////////////////////////////////

class CVarRegistry
{
	// simple list of all register CVars
	std::vector<CVarBase*> _registry;

	// private constructor + static pointer to handle singleton behavior
	CVarRegistry() {}
	CVarRegistry( const CVarRegistry& );
	const CVarRegistry& operator=( const CVarRegistry& );

	SDK_API static CVarRegistry *_instance;

public:

	// Instance retrieval function
	static CVarRegistry& Instance()
	{
		if (!_instance)
			_instance = new CVarRegistry();
		return *_instance;
	}

	//
	//interface
	//
	//  Given the singleton nature, these might be better done as static members
	//

	// register a CVar (should be handled automatically by CVar constructor
	void AddVar( CVarBase& var) { _registry.push_back(&var);}

	//interpret a key press and see if it applies to any of the CVars
	//  better done via a map to reduce overhead for a large number of CVars
	void ProcessKey( nv::KeyCode::KeyCode k, nv::ModifierKey::ModifierKey mod)
	{
		bool shift = bool( nv::ModifierKey::Shift & mod);

		if (shift)
		{
			for ( std::vector<CVarBase*>::iterator it = _registry.begin(); it < _registry.end(); it++)
			{
				if ( (*it)->_key == k)
					(*it)->Increment();
			}
		}
		else
		{
			for ( std::vector<CVarBase*>::iterator it = _registry.begin(); it < _registry.end(); it++)
			{
				if ( (*it)->_key == k)
					(*it)->Decrement();
			}
		}
	}

	// Display a formatting information with info on all the presently registered CVars
	void PrintHelp( )
	{
		for ( std::vector<CVarBase*>::iterator it = _registry.begin(); it < _registry.end(); it++)
		{
			nv::KeyCode::KeyCode code = (*it)->_key;
			char key = '\0';
			if ( code >= nv::KeyCode::Key_A && nv::KeyCode::Key_Z >= code)
				key = 'a' + code - nv::KeyCode::Key_A;

			printf( "  %s - %s\n", (*it)->_name, (*it)->_text);
			printf( "    '%c' - change value (hold <shift> to increase)\n", key );
		}
	}
};



////////////////////////////////////////////////////////////////////////////////
//
// CVar
//
//  Basic CVar, templatized over types. Allows the value to be incrememted
// by a step-size chozen at construction, and value is optionally restricted
// to a range specified at construction.
//
////////////////////////////////////////////////////////////////////////////////

template< class T>
struct CVar : public CVarBase
{
private:

	//copying CVars doesn't make sense
	CVar() {}
	CVar& operator=( const CVar& v) {}
	CVar( const CVar& v){}

public:

	// present value of the CVar
	T _val;

	// increment amount for this CVar
	T _inc;

	// range for the CVar, if it is clamped
	T _minVal;
	T _maxVal;

	// whether this CVar has a clamped range
	bool _clamped;

	// simple conversion and assingment operators to make it behave like the type it represents
	operator T&() { return _val;}
	operator const T&() const { return _val;}
	const CVar& operator=( T val) { _val = val; return *this; }

	// increment and decrement operations
	void Increment()
	{
		_val += _inc;
		if (_clamped)
			_val = std::min( _maxVal, _val);
	}
	void Decrement()
	{
		_val -= _inc;
		if (_clamped)
			_val = std::max( _minVal, _val);
	}

	// unclamped constructors
	CVar( const char* name, T initialVal, nv::KeyCode::KeyCode key, const char *helpText) :
		CVarBase(name, key, helpText), _val(initialVal), _inc(T(1)),
		_minVal(T(0)), _maxVal(T(0)), _clamped(false)
		{ CVarRegistry::Instance().AddVar(*this); }

	CVar( const char* name, T initialVal, T increment, nv::KeyCode::KeyCode key, const char *helpText) :
		CVarBase(name, key, helpText), _val(initialVal), _inc(increment),
		_minVal(T(0)), _maxVal(T(0)), _clamped(false)
		{ CVarRegistery::Instance().AddVar(*this); }

	// clamped constructors
	CVar( const char* name, T initialVal, T minVal, T maxVal, nv::KeyCode::KeyCode key, const char *helpText) :
		CVarBase(name, key, helpText), _val(initialVal), _inc(T(1)),
		_minVal(minVal), _maxVal(maxVal), _clamped(true)
		{ CVarRegistry::Instance().AddVar(*this); }

	CVar( const char* name, T initialVal, T increment, T minVal, T maxVal, nv::KeyCode::KeyCode key, const char *helpText) :
		CVarBase(name, key, helpText), _val(initialVal), _inc(increment),
		_minVal(minVal), _maxVal(maxVal), _clamped(true)
		{ CVarRegistry::Instance().AddVar(*this); }
};


// partial specialization for bool, because +/- make no sense
void CVar<bool>::Increment() { _val = !_val; }
void CVar<bool>::Decrement() { _val = !_val; }

////////////////////////////////////////////////////////////////////////////////
//
// CRefVar
//
//  Sister class to CVar, but instead of being an alterable value, a CRefVar
// contains a reference to another value that holds the data manipulated.
// A CRefVar allows CVar behavior to be bound directly to something like a
// member of a structure.
//
////////////////////////////////////////////////////////////////////////////////

template< class T>
struct CRefVar : public CVarBase
{
private:
	CRefVar() {}
	CRefVar& operator=( const CRefVar& v) {}
	CRefVar( const CRefVar& v){}
public:

	// reference to value being controlled
	T& _val;

	//increment amount
	T _inc;

	// min and max for clamped behavior
	T _minVal;
	T _maxVal;

	//whether the value should be clamped
	bool _clamped;

	// operators helping the value blend in
	operator T&() { return _val;}
	operator const T&() const { return _val;}
	const CRefVar& operator=( T val) { _val = val; return *this; }

	void Increment()
	{
		_val += _inc;
		if (_clamped)
			_val = std::min( _maxVal, _val);
	}
	void Decrement()
	{
		_val -= _inc;
		if (_clamped)
			_val = std::max( _minVal, _val);
	}

	// unclamped constructors
	CRefVar( T& refVal, const char* name, nv::KeyCode::KeyCode key, const char *helpText) :
		CVarBase(name, key, helpText), _val(refVal), _inc(T(1)),
		_minVal(T(0)), _maxVal(T(0)), _clamped(false)
		{ CVarRegistry::Instance().AddVar(*this); }

	CRefVar( T& refVal, const char* name, T increment, nv::KeyCode::KeyCode key, const char *helpText) :
		CVarBase(name, key, helpText), _val(refVal), _inc(increment),
		_minVal(T(0)), _maxVal(T(0)), _clamped(false)
		{ CVarRegistry::Instance().AddVar( *this); }

	// clamped constructors
	CRefVar( T& refVal, const char* name, T minVal, T maxVal, nv::KeyCode::KeyCode key, const char *helpText) :
		CVarBase(name, key, helpText), _val(refVal), _inc(T(1)),
		_minVal(minVal), _maxVal(maxVal), _clamped(true)
		{ CVarRegistry::Instance().AddVar(*this); }

	CRefVar( T& refVal, const char* name, T increment, T minVal, T maxVal, nv::KeyCode::KeyCode key, const char *helpText) :
		CVarBase(name, key, helpText), _val(refVal), _inc(increment),
		_minVal(minVal), _maxVal(maxVal), _clamped(true)
		{ CVarRegistry::Instance().AddVar(*this); }
};

// specialization for bool, since +/- are not valid operations
void CRefVar<bool>::Increment() { _val = !_val; }
void CRefVar<bool>::Decrement() { _val = !_val; }

}

#endif
