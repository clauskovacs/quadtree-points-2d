## A simple Quadtree

The backbone to create a simple Quadtree/Octree. If the elements in the node surpass the number *maxAmtElements*, this node will be split and the elements are redistributed, so that all elements only reside in the deepest nodes possible.

**a tree can be initialized with:**
>Quadtree *quadtreeTest = new Quadtree(BB_init, nullptr, 0);

**push a single point into the tree:**
>pt2d *insertPt = new pt2d(0, 0);
>quadtreeTest->insert(*insertPt);

**draw the tree using OpenGL:**
>quadtreeTest->traverse_and_draw(quadtreeTest, BB_dim);

**count all the nodes of the tree**
>int tree_size = quadtreeTest->count_nodes(quadtreeTest);

**count all the elements(points) of the tree**
>int tree_count_elements = quadtreeTest->count_elements(quadtreeTest);


**More Informations:**
http://www.phys.ik.cx/programming/cpp/quadtree/index.php <br />
https://www.youtube.com/watch?v=eyOuED5dhuo <br />
https://www.youtube.com/watch?v=9vMSsbjsHnQ
