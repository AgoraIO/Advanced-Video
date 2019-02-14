#pragma once
template
< typename T >
class Singleton
{
	static T * c_instance;
public:
	static T * Get();
};
template
<typename T>
T * Singleton<T>::c_instance = NULL;
template
<typename T>
T *Singleton<T> ::Get() {
	if (NULL == c_instance)
	{
		c_instance = new T();
	}
	return c_instance;
}
