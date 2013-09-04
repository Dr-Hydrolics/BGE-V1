#include "Route.h"
#include "LineDrawer.h"

using namespace BGE;

Route::Route(void)
{
	worldMode = world_modes::from_self;
	looped = false;
	draw = true;
	next = 0;
	tag = "Route";
}

Route::~Route(void)
{
}

void Route::Draw()
{
	for (int i = 1 ; i < waypoints.size() ; i ++)
	{
		LineDrawer::DrawLine(waypoints[i - 1], waypoints[i], glm::vec3(1, 0, 0));
	}
	if (looped)
	{
		LineDrawer::DrawLine(waypoints[0], waypoints[waypoints.size() - 1], glm::vec3(1, 0, 0));
	}

}

glm::vec3 Route::NextWaypoint()
{
	return waypoints[next];
}

bool Route::IsLast()
{

	return (next == waypoints.size() - 1);
}

void Route::AdvanceToNext()
{
	if (looped)
	{                
		next = (next + 1) % waypoints.size();
	}
	else
	{
		if (next != waypoints.size() - 1)
		{
			next = next + 1;
		}
	}
}