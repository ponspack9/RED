#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH),width(0), height(0)
{
	name.create("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];
	//else LOG("Not in boundaries");

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const p2DynArray<iPoint>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}
 p2DynArray<iPoint>* j1PathFinding::GetLastPathNotConst()
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while(item)
	{
		if(item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while(item)
	{
		if(item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.count();

	// north
	cell.create(pos.x, pos.y + 1);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// south
	cell.create(pos.x, pos.y - 1);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// east
	cell.create(pos.x + 1, pos.y);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// west
	cell.create(pos.x - 1, pos.y);
	if(App->pathfinding->IsWalkable(cell))
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	return list_to_fill.list.count();
}

uint PathNode::FindWalkableGroundAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.count();
	bool one	= false;
	bool two	= false;
	bool three	= false;

	LOG("pos[%d,%d]", pos.x, pos.y);


	// jump right
	cell.create(pos.x+1, pos.y); //ground at right
	one = !App->pathfinding->IsWalkable(cell);
	cell.create(pos.x+1, pos.y - 2);//north + right
	two = App->pathfinding->IsWalkable(cell);
	cell.create(pos.x, pos.y - 2); //north
	three = App->pathfinding->IsWalkable(cell);
	if (one && two && three)
	list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// jump left
	cell.create(pos.x - 1, pos.y); //ground at left
	one = !App->pathfinding->IsWalkable(cell);
	cell.create(pos.x - 1, pos.y - 1);//north + left
	two = App->pathfinding->IsWalkable(cell);
	cell.create(pos.x, pos.y - 1); //north
	three = App->pathfinding->IsWalkable(cell);
	if (one && two && three)
	list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// jump down left
	cell.create(pos.x - 1, pos.y+2); //ground at left + 2down
	one = !App->pathfinding->IsWalkable(cell);
	cell.create(pos.x - 1, pos.y + 1);//down + left
	two = App->pathfinding->IsWalkable(cell);
	cell.create(pos.x, pos.y + 1); //down
	three = App->pathfinding->IsWalkable(cell);
	if (one && two && three)
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// jump down right
	cell.create(pos.x + 1, pos.y + 2); //ground at right + 2down
	one = !App->pathfinding->IsWalkable(cell);
	cell.create(pos.x + 1, pos.y + 1);//down + right
	two = App->pathfinding->IsWalkable(cell);
	cell.create(pos.x, pos.y + 1); //down
	three = App->pathfinding->IsWalkable(cell);
	if (one && two && three)
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// down
	cell.create(pos.x, pos.y+2); //two down must be ground
	one = !App->pathfinding->IsWalkable(cell);
	cell.create(pos.x, pos.y + 1);// down
	two = App->pathfinding->IsWalkable(cell);
	if (one && two)
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// left
	cell.create(pos.x - 1, pos.y + 1);//left + down
	two = !App->pathfinding->IsWalkable(cell);
	cell.create(pos.x -1 , pos.y); //left
	one = App->pathfinding->IsWalkable(cell);
	if (one && two)
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	// right
	cell.create(pos.x + 1, pos.y + 1);//left + down
	two = !App->pathfinding->IsWalkable(cell);
	cell.create(pos.x + 1, pos.y); //left
	one = App->pathfinding->IsWalkable(cell);
	if (one && two)
		list_to_fill.list.add(PathNode(-1, -1, cell, this));

	//// up
	//cell.create(pos.x, pos.y - 1);//left + down
	//one = App->pathfinding->IsWalkable(cell);
	//if (one)
	//	list_to_fill.list.add(PathNode(-1, -1, cell, this));
	

	return list_to_fill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination, bool ground)
{
	iPoint dest = destination;
	if (ground) dest.y += 1;
	// TODO 1: if origin or destination are not walkable, return -1
	if (!IsWalkable(origin)) {
		//LOG("origin not walkable");
		return -1;
	}
	if (!IsWalkable(dest)) {
		//LOG("destination not walkable");
		return -1;
	}
	
	// TODO 2: Create two lists: open, close
	// Add the origin tile to open
	// Iterate while we have tile in the open list
		PathList open;
		PathList close;

		PathNode origin_node;

		origin_node.pos = origin;

		open.list.add(origin_node);

		while (open.list.count() > NULL)
		{
	// TODO 3: Move the lowest score cell from open list to the closed list
			p2List_item<PathNode>* current_node = open.GetNodeLowestScore();

			p2List_item<PathNode>* lowest_score_node = close.list.add(current_node->data);

	// TODO 4: If we just added the destination, we are done!
	// Backtrack to create the final path
	// Use the Pathnode::parent and Flip() the path when you are finish
			if (lowest_score_node->data.pos == dest)
			{
				int steps = 0;
				while (lowest_score_node->data.parent != nullptr)
				{
					last_path.PushBack(lowest_score_node->data.pos);
					steps++;
					lowest_score_node = close.Find(lowest_score_node->data.parent->pos);
				}
				last_path.PushBack(lowest_score_node->data.pos);
				steps++;

				last_path.Flip();
				return steps;
			}
	// TODO 5: Fill a list of all adjancent nodes
			PathList neighbours;
			
			if (ground) lowest_score_node->data.FindWalkableGroundAdjacents(neighbours);
			else lowest_score_node->data.FindWalkableAdjacents(neighbours);

	// TODO 6: Iterate adjancent nodes:
	// ignore nodes in the closed list
	// If it is NOT found, calculate its F and add it to the open list
	// If it is already in the open list, check if it is a better path (compare G)
	// If it is a better path, Update the parent
			for (p2List_item<PathNode>* neighbour_node = neighbours.list.start; neighbour_node != nullptr; neighbour_node = neighbour_node->next)
			{
				iPoint neighbour_pos = neighbour_node->data.pos;
				if (close.Find(neighbour_pos) != nullptr)
					continue;

				p2List_item<PathNode>* open_node = open.Find(neighbour_pos);
				if (open_node == nullptr)
				{
					neighbour_node->data.CalculateF(dest);
					open.list.add(neighbour_node->data);
				}
				else
				{
					int new_g = neighbour_node->data.parent->g + 1;
					if (new_g < open_node->data.g)
					{
						open_node->data.g = new_g;
						open_node->data.parent = &lowest_score_node->data;
					}
				}
			}
			open.list.del(current_node);
		}	

	return -1;
}

