// quadtree class & functions
#include <math.h>
#include <iostream>

#include <GL/glut.h>
#include <GL/gl.h>

#include "octree.h"

// constructor
Quadtree::Quadtree(BoundaryBox *BB_gen, Quadtree* parent, int _nodeDepth)
{
	northWest = nullptr;
	northEast = nullptr;
	southWest = nullptr;
	southEast = nullptr;

	this->boundary  = BB_gen;
	this->parent    = parent;
	this->nodeDepth = _nodeDepth;
}

// drawing routine (used by traverse_and_draw)
void Quadtree::colorPick(float elevate, Quadtree* t, float *depthColor, int depthColorLen)
{
	if (t->nodeDepth*3+2 > depthColorLen)	// default color when the depth exceeds the available colors from the array
		glColor4f(0.0f, 0.0f, 0.0f, 1.0f);
	else	// pick a color according to the array
		glColor4f(depthColor[t->nodeDepth*3], depthColor[t->nodeDepth*3+1], depthColor[t->nodeDepth*3+2], 1.0f);

	float centerx = t->boundary->cx;
	float centery = t->boundary->cy;
	float dim = t->boundary->dim;

	glBegin(GL_LINES);
		glVertex3f(centerx-dim, centery, elevate);
		glVertex3f(centerx+dim, centery, elevate);

		glVertex3f(centerx, centery-dim, elevate);
		glVertex3f(centerx, centery+dim, elevate);
	glEnd();
}

void Quadtree::traverse_and_draw(Quadtree* t, float widthRootNode)
{
	// adjust the height (z-coordinate) of the quadtree
	float elevate = -10.0f;

	// pick the colors according to the depth
	float depthColor [] = 
	{
	1.0f, 0.0f, 0.0f,		// depth 1 = red
	0.0f, 0.392f, 0.0f,		// depth 2 = darkgreen
	0.0f, 0.0f, 1.0f,		// depth 3 = blue
	1.0f, 0.0f, 1.0f,		// depth 4 = purple
	0.0f, 1.0f, 1.0f, 		// depth 5 = cyan
	0.294f, 0.0f, 0.51f,	// depth 6 = indigo
	0.863f, 0.078f, 0.235f,	// depth 6 = Crimson
	};

	glLineWidth(2.0f);

	if (t->northEast != NULL)
	{
		colorPick(elevate, t, depthColor, sizeof(depthColor)/sizeof(*depthColor));
		northEast->traverse_and_draw(northEast, widthRootNode);
	}

	if (t->northWest != NULL)
	{
		colorPick(elevate, t, depthColor, sizeof(depthColor)/sizeof(*depthColor));
		northWest->traverse_and_draw(northWest, widthRootNode);
	}

	if (t->southEast != NULL)
	{
		colorPick(elevate, t, depthColor, sizeof(depthColor)/sizeof(*depthColor));
		southEast->traverse_and_draw(southEast, widthRootNode);
	}

	if (t->southWest != NULL)
	{
		colorPick(elevate, t, depthColor, sizeof(depthColor)/sizeof(*depthColor));
		southWest->traverse_and_draw(southWest, widthRootNode);
	}
}

// split the current node into four new (children)nodes (increment depth by one)
void Quadtree::subdivide()
{
	// subdivide NW
	BoundaryBox *BB_init_NW = new BoundaryBox(boundary->cx-boundary->dim*0.5, boundary->cy+boundary->dim*0.5, boundary->dim*0.5);
	northWest = new Quadtree(BB_init_NW, this, this->nodeDepth+1);

	// subdivide NE
	BoundaryBox *BB_init_NE = new BoundaryBox(boundary->cx+boundary->dim*0.5, boundary->cy+boundary->dim*0.5, boundary->dim*0.5);
	northEast = new Quadtree(BB_init_NE, this, this->nodeDepth+1);

	// subdivide SE
	BoundaryBox *BB_init_SE = new BoundaryBox(boundary->cx+boundary->dim*0.5, boundary->cy-boundary->dim*0.5, boundary->dim*0.5);
	southEast = new Quadtree(BB_init_SE, this, this->nodeDepth+1);

	// subdivide SW
	BoundaryBox *BB_init_SW = new BoundaryBox(boundary->cx-boundary->dim*0.5, boundary->cy-boundary->dim*0.5, boundary->dim*0.5);
	southWest = new Quadtree(BB_init_SW, this, this->nodeDepth+1);
}

// insert one point into the tree. Split the tree and relocate the points ot the node if necessary
bool Quadtree::insert(pt2d insertPt)
{
	// check if the point resides in this node
	if (insertPt.x > boundary->cx+boundary->dim or insertPt.x < boundary->cx-boundary->dim or insertPt.y > boundary->cy+boundary->dim or insertPt.y < boundary->cy-boundary->dim)
		return false;

	if (children.size() < maxAmtElements and northWest == nullptr)	// there is room in the node for this pt. Only, ONLY insert the point if there is no children node available to sort into
	{
		children.push_back(insertPt);
		return true;
	}

	// maximum amount of pts in this node reached -> split into 4 new nodes
	if (northWest == nullptr)	// this node has not been split yet -> nullptr
	{
		subdivide();

		// remove all points from the parent node, and sort this points into the child nodes
		for (int i = 0; i < (int)children.size(); i++)
		{
			// sort this element into the according quadrant(NE, NW, SE, SW)
			pt2d queryPt(children[i].x, children[i].y);	// point to insert into child node

			// sort the point
			insert(queryPt);
		}

		// delete all points in the parent node, so th
		for (int i = 0; i < (int)children.size(); i++)
			children.erase (children.begin()+i);
	}

	if (northEast->insert(insertPt)) 
	{
		return true;
	}
	
	if (northWest->insert(insertPt))
	{
		return true;
	}
	
	if (southWest->insert(insertPt))
	{
		return true;
	}

	if (southEast->insert(insertPt))
	{
		return true;
	}

	return false;
}

// count the nodes of the tree
int Quadtree::count_nodes(Quadtree* t)
{
	if (t->northEast != NULL)	// node has been split
	{
		int nodes_NE = northEast->count_nodes(northEast);
		int nodes_SE = southEast->count_nodes(southEast);
		int nodes_SW = southWest->count_nodes(southWest);
		int nodes_NW = northWest->count_nodes(northWest);
		return nodes_NE + nodes_SE + nodes_SW + nodes_NW;
	}

	return 1;
}

// count the elements residing in the tree
int Quadtree::count_elements(Quadtree* t)
{
	int fetch_elements = 0;

	if (t->northEast != NULL)	// node has been split - continue with the recursion
	{
		fetch_elements += northEast->count_elements(northEast);
		fetch_elements += southEast->count_elements(southEast);
		fetch_elements += southWest->count_elements(southWest);
		fetch_elements += northWest->count_elements(northWest);
	}
	else	// deepest (child)node possible
		if (t->children.size() > 0)	// there are elements in this node
			fetch_elements += t->children.size();

	return fetch_elements;
}
