#pragma once
#include <vector>

namespace SB
{
	template<typename T>
	class SBContainer : public std::vector < T* >
	{
	public:
		~SBContainer()
		{
			for (int i = 0, l = this->size(); i < l; i++)
			{
				if (std::vector<T*>::operator[](i) != NULL)
				{
					delete std::vector<T*>::operator[](i);
					std::vector<T*>::operator[](i) = NULL;
				}
			}
		};
		T* operator [](const std::string& string)
		{
			for (int i = 0, l = this->size(); i < l; i++)
			{
				if (std::vector<T*>::operator[](i)->GetName() == string)
				{
					return std::vector<T*>::operator[](i);
				}
			}
			return NULL;
		}
		T* operator [](const char* string)
		{
			for (int i = 0, l = this->size(); i < l; i++)
			{
				if (std::vector<T*>::operator[](i)->GetName() == string)
				{
					return std::vector<T*>::operator[](i);
				}
			}
			return NULL;
		}
	};
}