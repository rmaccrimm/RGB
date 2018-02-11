#ifndef FLAG_H
#define FLAG_H

class Flag
{
public:
	Flag();
	bool is_set();
	void set();
	void reset();
	
private:
	bool f;
};

#endif
