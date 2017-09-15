// quadtree class & functions
#include <math.h>
#include <iostream>

#include <GL/glut.h>
#include <GL/gl.h>

#include "quadtree.h"

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

// insert one point into the tree. Split the tree and relocate the points ot the node if necessary
bool Quadtree::insert(pt2d insertPt)
{
	// check if the point resides in this node
	if (insertPt.x > boundary->cx+boundary->dim or insertPt.x < boundary->cx-boundary->dim or insertPt.y > boundary->cy+boundary->dim or insertPt.y < boundary->cy-boundary->dim)
		return false;

	if ((children.size() < maxAmtElements and northWest == nullptr) or this->nodeDepth == maxDepth)	// there is room in the node for this pt. Insert the point only if there is no children node available to sort into or if the maximum depth allowed has been reached
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
			children.erase(children.begin()+i);
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

// split the current node into four new (children)nodes (increment depth by one)
void Quadtree::subdivide()
{
	if (this->nodeDepth < maxDepth)	// split the node only if the maximum depth has not been reached yet
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
}

// drawing routine (used by traverse_and_draw). Used by traverse_and_draw()
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

// count the nodes of the tree
int Quadtree::count_nodes(Quadtree* t)
{
	// node has been split
	if (t->northEast != NULL)
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


std::vector<pt2d> Quadtree::fetch_points(pt2d seekPt)
{
	std::vector <pt2d> return_elements;

	// check if the point resides in this node
	if (seekPt.x > boundary->cx+boundary->dim or seekPt.x < boundary->cx-boundary->dim or seekPt.y > boundary->cy+boundary->dim or seekPt.y < boundary->cy-boundary->dim)
		return return_elements;

	// deepest node corresponding to this point reached
	if (northWest == nullptr)
	{
		// push all the points from this node into the vector to return it
		for (int i = 0; i < (int)children.size() ; i++)
			return_elements.push_back(children[i]);

		//draw the border of the node (only if elements are in this node)
		if ((int)return_elements.size() > 0 and nodeDepth > 0)
		{
			float centerx = this->parent->boundary->cx;
			float centery = this->parent->boundary->cy;
			float dim = this->parent->boundary->dim;

			float elevate = -10.0f;

			glLineWidth(15.0f);
			glBegin(GL_LINES);
				glVertex3f(centerx-dim, centery, elevate);
				glVertex3f(centerx+dim, centery, elevate);

				glVertex3f(centerx, centery-dim, elevate);
				glVertex3f(centerx, centery+dim, elevate);
			glEnd();
		}

		return return_elements;
	}

	// std::vectors holding the elements of these nodes
	std::vector <pt2d> NE_fetch = northEast->fetch_points(seekPt);
	std::vector <pt2d> NW_fetch = northWest->fetch_points(seekPt);
	std::vector <pt2d> SW_fetch = southWest->fetch_points(seekPt);
	std::vector <pt2d> SE_fetch = southEast->fetch_points(seekPt);

	// Concatenate the returned points
	return_elements.insert(return_elements.end(), NE_fetch.begin(), NE_fetch.end());
	return_elements.insert(return_elements.end(), NW_fetch.begin(), NW_fetch.end());
	return_elements.insert(return_elements.end(), SW_fetch.begin(), SW_fetch.end());
	return_elements.insert(return_elements.end(), SE_fetch.begin(), SE_fetch.end());

	return return_elements;
}


void Quadtree::clear(Quadtree* t)
{
    if(t == NULL)
        return;

	clear(t->northEast);
	clear(t->northWest);
	clear(t->southEast);
	clear(t->southWest);

	t->northEast = nullptr;
	t->northWest = nullptr;
	t->southEast = nullptr;
	t->southWest = nullptr;

	//delete t;
}

// fetch the node in which the given element resides
Quadtree * Quadtree::fetch_node(pt2d seekPt)
{
	Quadtree *ReturnNode;

	// check if the point resides in this node
	if (seekPt.x > boundary->cx+boundary->dim or seekPt.x < boundary->cx-boundary->dim or seekPt.y > boundary->cy+boundary->dim or seekPt.y < boundary->cy-boundary->dim)
	{
		//std::cout << "outside: " << ReturnNode << std::endl;
	}
	else
	{
		// deepest node corresponding to this point reached. Return the pointer to this node
		if (northWest == nullptr)
		{
			for (int i = 0; i < (int)children.size(); i++)
				if (seekPt.x == children[i].x and seekPt.y == children[i].y)
					return ReturnNode = this;
			return NULL;
		}

		ReturnNode = northEast->fetch_node(seekPt);
		ReturnNode = northWest->fetch_node(seekPt);
		ReturnNode = southWest->fetch_node(seekPt);
		ReturnNode = southEast->fetch_node(seekPt);
	}

	return ReturnNode;
}

// auxiliary function used by delete_element(). Used to collapse nodes and redistribute the elements in them
void Quadtree::concatenate_nodes(Quadtree *concat_this_node_maybe)
{
	// root node reached.
	if (concat_this_node_maybe->parent == NULL)
		return;

	int children_NE = concat_this_node_maybe->parent->northEast->children.size();
	int children_NW = concat_this_node_maybe->parent->northWest->children.size();
	int children_SE = concat_this_node_maybe->parent->southEast->children.size();
	int children_SW = concat_this_node_maybe->parent->southWest->children.size();

	unsigned int children_sum = children_NE + children_NW + children_SE + children_SW;

	bool hasChildren = false;
	if (concat_this_node_maybe->parent->northEast->northEast != NULL or concat_this_node_maybe->parent->northWest->northEast != NULL or concat_this_node_maybe->parent->southEast->northEast != NULL or concat_this_node_maybe->parent->southWest->northEast != NULL)
		hasChildren = true;

	// Concatenate because all four nodes (3 sibling nodes and the one where the point lies) are empty
	if (children_sum == 0 and hasChildren == false)
	{
		// Concatenate the parent
		clear(concat_this_node_maybe->parent);

		// check surrounding nodes recursively
		concatenate_nodes(concat_this_node_maybe->parent);
	}

	// shuffle all elements into the parent node if possible
	else if (children_sum <= maxAmtElements and hasChildren == false)
	{
		concat_this_node_maybe->parent->children.clear();

		for (int i = 0; i < children_NE; i++)
		{
			float reshufflex = concat_this_node_maybe->parent->northEast->children[i].x;
			float reshuffley = concat_this_node_maybe->parent->northEast->children[i].y;

			pt2d reinsertPt(reshufflex, reshuffley);
			concat_this_node_maybe->parent->children.push_back(reinsertPt);
		}

		for (int i = 0; i < children_NW; i++)
		{
			float reshufflex = concat_this_node_maybe->parent->northWest->children[i].x;
			float reshuffley = concat_this_node_maybe->parent->northWest->children[i].y;

			pt2d reinsertPt(reshufflex, reshuffley);
			concat_this_node_maybe->parent->children.push_back(reinsertPt);
		}

		for (int i = 0; i < children_SE; i++)
		{
			float reshufflex = concat_this_node_maybe->parent->southEast->children[i].x;
			float reshuffley = concat_this_node_maybe->parent->southEast->children[i].y;

			pt2d reinsertPt(reshufflex, reshuffley);
			concat_this_node_maybe->parent->children.push_back(reinsertPt);
		}

		for (int i = 0; i < children_SW; i++)
		{
			float reshufflex = concat_this_node_maybe->parent->southWest->children[i].x;
			float reshuffley = concat_this_node_maybe->parent->southWest->children[i].y;

			pt2d reinsertPt(reshufflex, reshuffley);
			concat_this_node_maybe->parent->children.push_back(reinsertPt);
		}

		clear(concat_this_node_maybe->parent);
		concatenate_nodes(concat_this_node_maybe->parent);
	}
}

// remove a single element of the tree
bool Quadtree::delete_element(pt2d deletePt)
{
	// try to locate the node where the point lies
	Quadtree * nodePtReside = fetch_node(deletePt);

	if (nodePtReside == NULL)
		return false;
	else
	{
		// delete the element from the children std::vector only if there is more than one element in this tree
		int del_index;
		bool foundItem = false;

		for (del_index = 0; del_index < (int)nodePtReside->children.size(); del_index++)
			if (deletePt.x == nodePtReside->children[del_index].x and deletePt.y == nodePtReside->children[del_index].y)
			{
				foundItem = true;
				break;
			}

		// remove the item
		nodePtReside->children.erase(nodePtReside->children.begin()+del_index);

		// element was not found -> deletion failed
		if (foundItem == false)
			return false;

		concatenate_nodes(nodePtReside);
	}
	return true;
}

bool Quadtree::relocate_element(pt2d ptOrigin, pt2d PtMoveTo)
{
	Quadtree * nodePtReside_Origin = fetch_node(ptOrigin);

	// PtMoveTo lies outside of the node -> remove and reinsert this element
	if (PtMoveTo.x > nodePtReside_Origin->boundary->cx+nodePtReside_Origin->boundary->dim or PtMoveTo.x < nodePtReside_Origin->boundary->cx-nodePtReside_Origin->boundary->dim or PtMoveTo.y > nodePtReside_Origin->boundary->cy+nodePtReside_Origin->boundary->dim or PtMoveTo.y < nodePtReside_Origin->boundary->cy-nodePtReside_Origin->boundary->dim)
	{
		// TODO - remove element, reinsert into the parent node not the root node
		//std::cout << "REINSERT" << std::endl;
		delete_element(ptOrigin);

		// insertion of the element was successful
		if (insert(PtMoveTo))
			return true;
		// element could not be fit into the root node, i.e., exited the root tree
		else
			return false;
	}
	//overwrite the point since it didn't move out of the node
	else
	{
		//std::cout << "MOVING" << std::endl;
		// find the position of ptOrigin and overwrite its coordinates with the ones of PtMoveTo
		int foundIndex = -1;
		for (int i = 0; i < (int)nodePtReside_Origin->children.size(); i++)
		{
			if (nodePtReside_Origin->children[i].x == ptOrigin.x and nodePtReside_Origin->children[i].y == ptOrigin.y)
			{
				foundIndex = i;
				break;
			}
		}

		// update the coordinates, i.e., move the element
		nodePtReside_Origin->children[foundIndex].x = PtMoveTo.x;
		nodePtReside_Origin->children[foundIndex].y = PtMoveTo.y;
	}

	return true;
}