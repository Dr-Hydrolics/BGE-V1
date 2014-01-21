#pragma once

#include<glm.hpp>
#include<glm.hpp>
#include<sdl.h>
#include <windows.h>
#include <windowsx.h>
#include <io.h>
#include <GL/glew.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include<glm.hpp>
#include<list>
#include<string>
#include <gtc/matrix_transform.hpp>
#include <gtc/quaternion.hpp>
#include <gtx/quaternion.hpp>
#include <memory>
#include "Exception.h"
#include "Transform.h"

namespace BGE 
{
	float RandomFloat();

	class GameComponent
	{
	private:		
		void RotateVectors();
	protected:
		
	public:
		GameComponent(void);

		virtual bool Initialise();
		virtual void Update(float timeDelta);
		virtual void Draw();
		virtual void Cleanup();

		GameComponent * parent;
		std::string tag;

		enum draw_modes {materials, textured, single_material};
		enum world_modes {from_self, from_self_with_parent, from_child, to_parent, from_parent};

		draw_modes drawMode;
		world_modes worldMode;
		
		std::list<std::shared_ptr<GameComponent>> children;
		
		float speed;
		bool alive;
		bool initialised;	

		std::shared_ptr<Transform> transform;


		void Attach(std::shared_ptr<GameComponent> child);
		std::list<std::shared_ptr<GameComponent>> * GetChildren();
		std::shared_ptr<GameComponent> GetController();
	};
}
