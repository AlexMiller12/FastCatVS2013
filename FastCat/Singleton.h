#ifndef SINGLETON_H
#define SINGLETON_H

// Thanks Patryk on Stack Overflow
// http://stackoverflow.com/questions/11452760/singleton-c-template-class
//
// Example use: Singleton<Game>::Instance().playGame();

template <typename T>
class Singleton
{
//---------------------------------CONSTANTS:
	

//------------------CONSTRUCTORS/DESTRUCTORS:

protected:
	inline explicit Singleton();
	virtual ~Singleton();

//------------------------------------FIELDS:

private:
	static T* _instance;
	static T* CreateInstance();

//-----------------------------------METHODS:

public:
	static T& instance();
	static void cleanUp();
};

template<typename T>
T* Singleton<T>::_instance = 0;

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
void Singleton<T>::cleanUp()
{
	delete _instance;
}

template<typename T>
T& Singleton<T>::instance()
{
	if( Singleton::_instance == 0 )
	{
		Singleton::_instance = CreateInstance();
	}
	return *( Singleton::_instance );
}

//--------------------------------------------------------------------------HELPERS:

template<typename T>
inline T* Singleton<T>::CreateInstance()
{
	return new T();
}

#endif // SINGLETON_H