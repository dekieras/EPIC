// Event pointers are passed around, not event objects.
// Originator allocates them with new.
// Coordinator will deallocate them after they are sent.
// Event receivers should declare parameter as const Event_type *.

#ifndef EVENT_BASE_H
#define EVENT_BASE_H


// forward declarations
class Processor;

class Event {
public:
	virtual ~Event() {}
	virtual void send_self(Processor *) const = 0; // this class is abstract
	long get_time() const 
		{return time;}
	Processor * get_destination() const 
		{return destination;}

	bool operator< (const Event& y) const {
		if (time < y.time)
			return true;
		else if(time == y.time)
			return serial_number < y.serial_number;
		else
			return false;
		}

	// just for measurement purposes
	static long get_counter() {return counter;}

protected:
	Event(long t, Processor * d) : time(t), serial_number(counter++), destination(d) {}

private:
	long time;
	long serial_number;	// used to break ties on time - lowest is earliest
	Processor * destination;
	static long counter;
};

// comparitor function object for Event *s - greater for the priority queue
struct greater_Event_ptr
{
	bool operator() (const Event * x, const Event * y) const
	{ return *y < *x; }
};

struct less_Event_ptr
{
	bool operator() (const Event * x, const Event * y) const
	{ return *x < *y; }
};


#endif
