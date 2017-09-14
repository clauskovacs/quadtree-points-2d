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
		// Children nodes
		Quadtree* northWest;
		Quadtree* northEast;
		Quadtree* southWest;
		Quadtree* southEast;

		// dimensions of the node
		BoundaryBox *boundary;

		// elements in this node
		std::vector<pt2d> children;

		// minimum amount of pts to split the node
		unsigned int maxAmtElements = 1;

		// maximum depth of the children nodes
		int maxDepth = 6;

		// depth of the node (0...root node)
		int nodeDepth;

		// drawing routine (used by traverse_and_draw)
		void colorPick(float elevate, Quadtree* t, float *depthColor, int depthColorLen);

		// pointer to the parent node
		Quadtree *parent;

		// auxiliary function used by delete_element()
		void concatenate_nodes(Quadtree *concat_this_node_maybe);

		// fetch the node in which the given element resides
		// auxiliary function used by delete_element()
		Quadtree * fetch_node(pt2d seekPt);

	public:
		// constructor
		Quadtree(BoundaryBox *BB_init, Quadtree* parent, int _nodeDepth);

		// insert a point into the tree
		bool insert(pt2d insertPt);

		// create four children that fully divide this quad into four quads of equal area
		void subdivide();

		// draw the tree using OpenGL
		void traverse_and_draw(Quadtree* t, float widthRootNode);

		// count the nodes of the tree
		int count_nodes(Quadtree* t);

		// count the elements residing in the tree
		int count_elements(Quadtree* t);

		// returns all points corresponding to the node in which this point resides.
		std::vector<pt2d> fetch_points(pt2d seekPt);

		// clear the tree
		void clear(Quadtree* t);

		// remove a single element of the tree
		bool delete_element(pt2d deletePt);
};
#endif