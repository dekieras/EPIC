#ifndef UTILITY_TEMPLATES_H
#define UTILITY_TEMPLATES_H

/* See Scott Meyers, Effective STL, Item 7 for a discussion
of this general purpose Function Object class - will work for deleting any pointer */
struct Delete {
template<typename T>
void operator() (const T* ptr) const
	{
		delete ptr;
	}
};

struct Delete_second {
template<typename T>
void operator() (const T thePair) const
	{
		delete thePair.second;
	}
};

#endif
