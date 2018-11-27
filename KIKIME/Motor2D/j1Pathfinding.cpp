#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1PathFinding.h"
#include "j1Scene.h"
#include "j1Debug.h"
#include "j1Render.h"

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
	BROFILER_CATEGORY("Pathfinding->CleanUp", Profiler::Color::OrangeRed)
	LOG("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	BROFILER_CATEGORY("Pathfinding->SetMap", Profiler::Color::OrangeRed)
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	BROFILER_CATEGORY("Pathfinding->CheckBoundaries", Profiler::Color::OrangeRed)
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos) const
{
	BROFILER_CATEGORY("Pathfinding->IsWalkable", Profiler::Color::OrangeRed)
	uchar t = GetTileAt(pos);
	return t != INVALID_WALK_CODE && t > 0;
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	BROFILER_CATEGORY("Pathfinding->GetTileAt", Profiler::Color::OrangeRed)
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];
	//else LOG("Not in boundaries");

	return INVALID_WALK_CODE;
}

bool j1PathFinding::Update(float dt) {
//	int a = 1;
	if (App->debug->show_colliders) DrawMap();
	return true;
}

void j1PathFinding::DrawMap()
{
	for (uint y = 0; y < height; ++y)
	{
		for (uint x = 0; x < width; ++x)
		{
			int i = (y*width) + x;
			//LOG("PRIIIINT [%d,%d]", x, y);
			//LOG("I: %d", i);
			if (map[i] == 0) {
				App->render->Blit(App->debug->not_tex, x*32, y*32);
			}
		}
	}
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
	BROFILER_CATEGORY("PathList->Find", Profiler::Color::Orange)
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
	BROFILER_CATEGORY("PathList->GetNodeLowestScore", Profiler::Color::Orange)
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
	BROFILER_CATEGORY("PathNode->FindWalkableAdjacents", Profiler::Color::Orange)
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
	BROFILER_CATEGORY("PathNode->FindWalkableGroundAdjacents", Profiler::Color::Orange)
	iPoint cell;
	uint before = list_to_fill.list.count();
	bool north	= false;
	bool south	= false;
	bool left   = false;
	bool right	= false;

	//LOG("pos[%d,%d]", pos.x, pos.y);

	// left
	if (!App->pathfinding->IsWalkable({ pos.x - 1, pos.y + 1 })) { // ground at left down
		cell.create(pos.x - 1, pos.y);
		if (App->pathfinding->IsWalkable(cell)) { // free at left
			list_to_fill.list.add(PathNode(-1, -1, cell, this));
			left = true;
		}
	}
	// right
	if (!App->pathfinding->IsWalkable({ pos.x + 1, pos.y + 1 })) { //right + down == ground
		cell.create(pos.x + 1, pos.y); //right
		if (App->pathfinding->IsWalkable(cell)) {
			list_to_fill.list.add(PathNode(-1, -1, cell, this));
			right = true;
		}
	}

	// Simple jump right
	if (!App->pathfinding->IsWalkable({pos.x + 1, pos.y})) { //ground at right
		if (App->pathfinding->IsWalkable({pos.x + 1, pos.y - 1})) { // free north + right
			cell.create(pos.x, pos.y - 1); // free north
			if (App->pathfinding->IsWalkable(cell)) {
				list_to_fill.list.add(PathNode(-1, -1, cell, this));
				north = true;
			}
		}
	}
	// Simple jump left
	if (!north && !App->pathfinding->IsWalkable({ pos.x - 1, pos.y })) { //ground at left
		if (App->pathfinding->IsWalkable({ pos.x - 1, pos.y - 1 })) { // free north + left
			cell.create(pos.x, pos.y - 1); // free north
			if (App->pathfinding->IsWalkable(cell)) {
				list_to_fill.list.add(PathNode(-1, -1, cell, this));
				north = true;
			}
		}
	}

	// double jump right
	if (!north && !App->pathfinding->IsWalkable({pos.x + 1, pos.y})) { //ground at right
		if (!App->pathfinding->IsWalkable({pos.x + 1, pos.y - 1})) { //ground at right north
			if (App->pathfinding->IsWalkable({pos.x+1, pos.y - 2})) { //free at 2north + right
				if (App->pathfinding->IsWalkable({pos.x, pos.y - 2})) { //free at 2north
					cell.create(pos.x, pos.y - 1); //north
					if (App->pathfinding->IsWalkable(cell)) {
						list_to_fill.list.add(PathNode(-1, -1, cell, this));
						north = true;
					}
				}
			}
		}
	}
	// double jump left
	if (!north && !App->pathfinding->IsWalkable({ pos.x - 1, pos.y })) { //ground at left
		if (!App->pathfinding->IsWalkable({ pos.x - 1, pos.y - 1 })) { //ground at left north
			if (App->pathfinding->IsWalkable({ pos.x - 1, pos.y - 2 })) { //free at 2north + left
				if (App->pathfinding->IsWalkable({ pos.x, pos.y - 2 })) { //free at 2north
					cell.create(pos.x, pos.y - 1); //north
					if (App->pathfinding->IsWalkable(cell)) {
						list_to_fill.list.add(PathNode(-1, -1, cell, this));
						north = true;
					}
				}
			}
		}
	}

	// Simple descend right
	if (!right && !App->pathfinding->IsWalkable({pos.x + 1, pos.y + 2})) { //ground at right 2down
		if (App->pathfinding->IsWalkable({pos.x + 1, pos.y + 1})) { //free at right down
			cell.create(pos.x + 1, pos.y); //free at right
			if (App->pathfinding->IsWalkable(cell)) {
				list_to_fill.list.add(PathNode(-1, -1, cell, this));
				//list_to_fill.list.add(PathNode(-1, -1, {pos.x+1,pos.y+1}, this));
				right = true;
			}
		}
	}
	// Simple descend left
	if (!left && !App->pathfinding->IsWalkable({pos.x - 1, pos.y + 2})) { //ground at left 2down
		if (App->pathfinding->IsWalkable({pos.x - 1, pos.y + 1})) { //free at left down
			cell.create(pos.x - 1, pos.y); //free at left
			if (App->pathfinding->IsWalkable(cell)) {
				list_to_fill.list.add(PathNode(-1, -1, cell, this));
				//list_to_fill.list.add(PathNode(-1, -1, { pos.x - 1,pos.y + 1 }, this));
				left = true;
			}
		}
	}
	// double descend right
	if (!right && !App->pathfinding->IsWalkable({ pos.x, pos.y +1 })) { //ground at south
		if (!App->pathfinding->IsWalkable({ pos.x, pos.y + 2 })) { //ground at 2 south
			if (App->pathfinding->IsWalkable({ pos.x + 1, pos.y + 1})) { //free at right south
				if (App->pathfinding->IsWalkable({ pos.x +1, pos.y + 2 })) { //free at right 2north
					cell.create(pos.x+1, pos.y); //free at right
					if (App->pathfinding->IsWalkable(cell)) {
						list_to_fill.list.add(PathNode(-1, -1, cell, this));
						right = true;
					}
				}
			}
		}
	}
	// double descend left
	if (!left && !App->pathfinding->IsWalkable({ pos.x, pos.y + 1 })) { //ground at south
		if (!App->pathfinding->IsWalkable({ pos.x, pos.y + 2 })) { //ground at 2 south
			if (App->pathfinding->IsWalkable({ pos.x - 1, pos.y + 1 })) { //free at left south
				if (App->pathfinding->IsWalkable({ pos.x - 1, pos.y + 2 })) { //free at left 2north
					cell.create(pos.x - 1, pos.y); //free at left
					if (App->pathfinding->IsWalkable(cell)) {
						list_to_fill.list.add(PathNode(-1, -1, cell, this));
						left = true;
					}
				}
			}
		}
	}
	// Simple descend south
	if (!App->pathfinding->IsWalkable({ pos.x, pos.y + 2 })) {
		cell.create(pos.x, pos.y + 1);// down
		if (App->pathfinding->IsWalkable(cell)) {
			list_to_fill.list.add(PathNode(-1, -1, cell, this));
			south = true;
		}
	}
	// Double descend south
	if (!south && !App->pathfinding->IsWalkable({ pos.x, pos.y + 3 })) { // Ground at 3 south
		if (App->pathfinding->IsWalkable({ pos.x, pos.y + 2 })) {	// Free at 2 north
			cell.create(pos.x, pos.y + 1);// free at north
			if (App->pathfinding->IsWalkable(cell)) {
				list_to_fill.list.add(PathNode(-1, -1, cell, this));
			}
		}
	}

	

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
	BROFILER_CATEGORY("Pathfinding->CreatePath", Profiler::Color::Orange)
	iPoint dest = destination;
	if (ground) dest.y += 1;

	if (!IsWalkable(origin)) {
		//LOG("origin not walkable");
		return -1;
	}
	if (!IsWalkable(dest)) {
		//LOG("destination not walkable");
		return -2;
	}
	
	
	// Add the origin tile to open
	// Iterate while we have tile in the open list
		PathList open;
		PathList close;

		PathNode origin_node;

		origin_node.pos = origin;

		open.list.add(origin_node);

		while (open.list.count() > NULL)
		{
	// Move the lowest score cell from open list to the closed list
			p2List_item<PathNode>* current_node = open.GetNodeLowestScore();

			p2List_item<PathNode>* lowest_score_node = close.list.add(current_node->data);

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

			PathList neighbours;
			if (ground) lowest_score_node->data.FindWalkableGroundAdjacents(neighbours);
			else lowest_score_node->data.FindWalkableAdjacents(neighbours);

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

