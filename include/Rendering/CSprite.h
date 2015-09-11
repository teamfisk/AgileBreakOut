/*
	This file is part of Daydream Engine.
	Copyright 2014 Adam Byléhn, Tobias Dahl, Simon Holmberg, Viktor Ljung
	
	Daydream Engine is free software: you can redistribute it and/or modify
	it under the terms of the GNU Lesser General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
	
	Daydream Engine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Lesser General Public License for more details.
	
	You should have received a copy of the GNU Lesser General Public License
	along with Daydream Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef Components_Sprite_h__
#define Components_Sprite_h__

#include <string>

#include "Core/Component.h"

namespace dd
{

namespace Components
{

struct Sprite : Component
{
	Sprite() : Color(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)) { }

	std::string SpriteFile;
	std::string NormalTexture;
	std::string SpecularTexture;
	glm::vec4 Color;
};

}
}

#endif // !Components_Sprite_h__