#ifndef POLYLOAD_HPP
#define POLYLOAD_HPP

class PolyLoader
{
	private:
		int grip;
	protected:
		int getnumber();
		char nextchar();
	public:
		PolyLoader(world_type& world, char *filename);
		
};

#endif
