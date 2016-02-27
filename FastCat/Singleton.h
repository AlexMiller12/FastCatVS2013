#ifndef SINGLETON_H
#define SINGLETON_H

// Thanks Patryk on Stack Overflow
// http://stackoverflow.com/questions/11452760/singleton-c-template-class
//
// Example use: Singleton<Game>::Instance().run();

template <typename T>
class Singleton
{
//------------------------------------------------------------------------CONSTANTS:
	

//---------------------------------------------------------CONSTRUCTORS/DESTRUCTORS:

protected:
	inline explicit Singleton();
	virtual ~Singleton();

//---------------------------------------------------------------------------FIELDS:

private:
	static T* _instance;
	static T* CreateInstance();

//--------------------------------------------------------------------------METHODS:

public:
	static T& Instance();
};

template<typename T>
T* Singleton<T>::_instance = 0;

#endif // SINGLETON_H