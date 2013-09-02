#include "Sphere.h"
#include "Model.h"
#include "Content.h"

using namespace BGE;

Sphere::Sphere(float radius)
{
	std::shared_ptr<GameComponent> model (Content::LoadModel("sphere"));
	model->Initialise();
	drawMode = Model::draw_modes::single_material;
	diffuse = glm::vec3(RandomFloat(),RandomFloat(),RandomFloat());
	specular = glm::vec3(1.2f,1.2f, 1.2f);
	id = "Sphere";
	Attach(model);
	scale = glm::vec3(radius, radius, radius);
}


Sphere::~Sphere(void)
{
}
