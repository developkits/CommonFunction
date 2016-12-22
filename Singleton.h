#pragma once

template<typename T>
class Singleton
{
public:
	static T* Instance()
	{
		static T s_instance;
		return &s_instance;
	}

	virtual bool Init() = 0;
	virtual void Uninit() = 0;

protected:
	Singleton() {};
	virtual ~Singleton(){};
};
