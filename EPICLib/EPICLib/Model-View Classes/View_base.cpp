#include "View_base.h"
#include "Human_subprocessor.h"

#include <list>

using std::list;


// if this window is destroyed, it must remove itself from all processors to which it is
// attached
View_base::~View_base()
{
	detach_from_all();
}

// add this window to the specified processor
void View_base::attach_to(Human_subprocessor * processor_ptr)
{
	processor_ptr->add_view(this);
	processor_ptrs.push_back(processor_ptr);
}

void View_base::detach_from(Human_subprocessor * processor_ptr)
{
	processor_ptr->remove_view(this);
	processor_ptrs.remove(processor_ptr);
}

void View_base::detach_from_all()
{
	for(list<Human_subprocessor *>::iterator it = processor_ptrs.begin();
		it != processor_ptrs.end(); ++it)
			(*it)->remove_view(this);
	processor_ptrs.clear();
}
