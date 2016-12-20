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
};
