#include "singleton.h"
#include <cstdlib>


//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

template <typename T>
Singleton<T>::Singleton()
{
	assert( Singleton::_instance == 0 );
	Singleton::_instance = static_cast<T*>( this );
}

template<typename T>
Singleton<T>::~Singleton()
{
	if( Singleton::_instance != 0 )
	{
		delete Singleton::_instance;
	}
	Singleton::_instance = 0;
}

//--------------------------------------------------------------------------METHODS:

template<typename T>
T& Singleton<T>::Instance()
{
	if( Singleton::_instance == 0 )
	{
		Singleton::_instance = CreateInstance();
	}
	return *( Singleton::_instance );
}

template<typename T>
inline T* Singleton<T>::CreateInstance()
{
	return new T();
}
