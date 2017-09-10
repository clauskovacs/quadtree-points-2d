// quadtree header
#ifndef __QUADREE_H_INCLUDED__
#define __QUADREE_H_INCLUDED__

#include <vector>

struct pt2d
{
	float x = 0.0;
	float y = 0.0;

	// constructor
	pt2d(float _cx, float _cy)
	{
		x = _cx;
		y = _cy;
	}
};

struct BoundaryBox
{
	float cx;	// center of the node (y-coordinate)
	float cy;	// center of the node (y-coordinate)
	float dim;	// width of the node

	// constructor
	BoundaryBox(float _cx, float _cy, float _dim)
	{
		cx  = _cx;
		cy  = _cy;
		dim = _dim;
	}
};

class Quadtree
{
	private:
		// Children
		Quadtree* northWest;
		Quadtree* northEast;
		Quadtree* southWest;
		Quadtree* southEast;

		// dimensions of the Node
		BoundaryBox *boundary;

		// elements in this node
		std::vector<pt2d> children;

		// minimum amount of pts to split the node
		unsigned int maxAmtElements = 5;

		// pointer to the parent node
		Quadtree *parent;

	public:
		// constructor
		Quadtree(BoundaryBox *BB_init, Quadtree* parent);

		// insert a point into the tree
		bool insert(pt2d insertPt);

		// create four children that fully divide this quad into four quads of equal area
		void subdivide();

		// draw the tree using OpenGL
		void traverse_and_draw(Quadtree* t);

		// count the nodes of the tree
		int count_nodes(Quadtree* t);

		// count the elements residing in the tree
		int count_elements(Quadtree* t);

};
#endif