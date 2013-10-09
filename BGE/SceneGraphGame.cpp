#include "SceneGraphGame.h"
#include "Content.h"
#include "VectorDrawer.h"
#include "XBoxController.h"
#include "Steerable3DController.h"
#include "SteeringControler.h"
#include "Params.h"

using namespace BGE;



SceneGraphGame::SceneGraphGame(void)
{
	physicsFactory = NULL;
	dynamicsWorld = NULL;
	broadphase = NULL;
	dispatcher = NULL;
	solver = NULL;
	
}

SceneGraphGame::~SceneGraphGame(void)
{
}

glm::vec3 SceneGraphGame::NextPosition(float step, float steps)
{
	float radius = 30.0f;
	float y = 5.0f;
	float inc = (glm::pi<float>() * 2.0f) / steps;
	float angle = step * inc;
	glm::vec3 pos;
	
	pos.x = glm::sin(angle) * radius;
	pos.y = y;
	pos.z = - glm::cos(angle) * radius;
	waypoints.push_back(pos);
	return pos;
}

bool SceneGraphGame::Initialise()
{
	Params::Load("default");
	float componentCount = 10.0f;
	float current = 0.0f;
	// Set up the collision configuration and dispatcher
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);

	// The world.
	btVector3 worldMin(-1000,-1000,-1000);
	btVector3 worldMax(1000,1000,1000);
	broadphase = new btAxisSweep3(worldMin,worldMax);
	solver = new btSequentialImpulseConstraintSolver();
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,0,0));

	camera->position = glm::vec3(0,10,0);
	camera->look = glm::vec3(0, 0, 1);

	physicsFactory = make_shared<PhysicsFactory>(dynamicsWorld);

	physicsFactory->CreateCameraPhysics();
	physicsFactory->CreateGroundPhysics();

	fullscreen = false;
	riftEnabled = false;
	width = 800;
	height = 600;

	// from_self
	selfExample = make_shared<GameComponent>();
	selfExample->Attach(Content::LoadModel("ferdelance", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	selfExample->Attach(make_shared<VectorDrawer>(glm::vec3(5,5,5)));
	selfExample->position = NextPosition(current ++, componentCount);
	Attach(selfExample);

	// from_self_with_parent
	// Create a hierarchy
	station = make_shared<GameComponent>();
	station->worldMode = world_modes::from_self;
	station->ambient = glm::vec3(0.2f, 0.2, 0.2f);
	station->specular = glm::vec3(0,0,0);
	station->scale = glm::vec3(1,1,1);
	std::shared_ptr<Model> cmodel = Content::LoadModel("coriolis", glm::rotate(glm::mat4(1), 90.0f, GameComponent::basisUp));	
	station->Attach(cmodel);
	station->Attach(make_shared<VectorDrawer>(glm::vec3(5,5,5)));
	station->position = NextPosition(current ++, componentCount);
	Attach(station);

	// Add a child to the station and update by including the parent's world transform
	std::shared_ptr<GameComponent> ship1 = make_shared<GameComponent>();
	ship1->worldMode = world_modes::from_self_with_parent;
	ship1->ambient = glm::vec3(0.2f, 0.2, 0.2f);
	ship1->specular = glm::vec3(1.2f, 1.2f, 1.2f);
	std::shared_ptr<Model> ana = Content::LoadModel("anaconda", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp));	
	ship1->Attach(ana);
	ship1->position = glm::vec3(0, 0, -10);
	station->Attach(ship1); // NOTE the ship is attached to the station at an offset of 10.
	
	// Create a component  with an XBOX Controller attached
	shared_ptr<GameComponent> ship2 = make_shared<GameComponent>();
	ship2->worldMode = world_modes::from_child;
	ship2->Attach(make_shared<XBoxController>());
	ship2->Attach(Content::LoadModel("cobramk3", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0))));
	ship2->Attach(make_shared<VectorDrawer>(glm::vec3(5,5,5)));
	ship2->GetController()->position = NextPosition(current ++, componentCount);
	Attach(ship2);

	// Create a component with a steerable 3D controller attached
	shared_ptr<GameComponent> ship3 = make_shared<GameComponent>();
	ship3->worldMode = world_modes::from_child;
	ship3->scale = glm::vec3(3,3,3);
	shared_ptr<Model> s3Model = Content::LoadModel("moray", glm::rotate(glm::mat4(1), 180.0f, glm::vec3(0,1,0)));
	ship3->Attach(make_shared<Steerable3DController>(s3Model));
	ship3->Attach(s3Model);
	ship3->Attach(make_shared<VectorDrawer>(glm::vec3(5,5,5)));
	ship3->GetController()->position = NextPosition(current ++, componentCount);
	Attach(ship3);

	// Create some physics components using the factory
	physicsFactory->CreateBox(5,5,5, NextPosition(current ++, componentCount), glm::quat());
	physicsFactory->CreateFromModel("monkey", NextPosition(current ++, componentCount), glm::quat(), glm::vec3(5,5,5));

	// Create a physics car
	physicsFactory->CreateVehicle(NextPosition(current ++, componentCount));

	// Create a physics component and attach a non-physics component to it
	shared_ptr<PhysicsController> carController = physicsFactory->CreateVehicle(NextPosition(current ++, componentCount));
	carController->Attach(Content::LoadModel("transporter", glm::translate(glm::mat4(1), glm::vec3(0,5,0))));

	// Create some steering components to chase each other
	shared_ptr<GameComponent> ship4 = make_shared<GameComponent>();
	ship4->tag = "Steerable";
	ship4->scale = glm::vec3(2, 2, 2);
	shared_ptr<SteeringController> ship4Controller = make_shared<SteeringController>();
	ship4Controller->position = NextPosition(current ++, componentCount);
	ship4Controller->TurnOffAll();
	ship4Controller->TurnOn(SteeringController::behaviour_type::follow_path);
	ship4Controller->route->waypoints.push_back(ship4Controller->position);
	ship4Controller->route->waypoints.push_back(ship4Controller->position + glm::vec3(15, 0, 0));
	ship4Controller->route->waypoints.push_back(ship4Controller->position + glm::vec3(15, 0, -15));
	ship4Controller->route->waypoints.push_back(ship4Controller->position + glm::vec3(0, 0, -15));
	ship4Controller->route->looped = true;
	ship4Controller->route->draw = true;
	ship4->Attach(ship4Controller);
	ship4->Attach(Content::LoadModel("krait", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp)));
	Attach(ship4);

	// Create a component that follows a path from component to component
	shared_ptr<GameComponent> ship6 = make_shared<GameComponent>();
	ship6->tag = "Steerable";
	ship6->scale = glm::vec3(2, 2, 2);
	shared_ptr<SteeringController> ship6Controller = make_shared<SteeringController>();
	ship6Controller->position = NextPosition(current ++, componentCount);
	ship6Controller->TurnOffAll();
	ship6Controller->TurnOn(SteeringController::behaviour_type::follow_path);
	ship6Controller->route->diffuse = glm::vec3(0,0,1);
	ship6Controller->route->draw = true;

	// Add some waypoints
	for (int i = 0 ; i < waypoints.size() ; i ++)
	{
		ship6Controller->route->waypoints.push_back(waypoints[i]);
		ship6Controller->route->looped = true;
	}
	ship6->Attach(ship6Controller);
	ship6->Attach(Content::LoadModel("python", glm::rotate(glm::mat4(1), 180.0f, GameComponent::basisUp)));
	Attach(ship6);

	return Game::Initialise();
}

void SceneGraphGame::Update(float timeDelta)
{
	dynamicsWorld->stepSimulation(timeDelta,100);

	selfExample->Yaw(timeDelta * speed * speed);
	station->Yaw(timeDelta * speed * speed);
	Game::Update(timeDelta);
}

void SceneGraphGame::Cleanup()
{
	Game::Cleanup();
}

