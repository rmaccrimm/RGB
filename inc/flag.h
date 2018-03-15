#ifndef FLAG_H
#define FLAG_H

class Flag
/* Basically just a more readable bool, with set and reset methods to match 
   terms used by GB manual
*/
{
public:
	Flag();
	bool is_set();
	void set();
	void reset();
	void operator=(int val);
private:
	bool f;
};

#endif
