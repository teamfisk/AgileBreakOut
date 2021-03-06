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

#ifndef Components_PointLight_h__
#define Components_PointLight_h__

#include "Core/Component.h"
#include "Rendering/Color.h"

namespace dd
{

namespace Components
{

struct PointLight : Component
{
	PointLight() 
		: Specular(1.0f, 1.0f, 1.0f)
		, Diffuse(1.0f, 1.0f, 1.0f)
		, specularExponent(50.0f)
		, ConstantAttenuation(1.0f)
		, LinearAttenuation(0.f)
		, QuadraticAttenuation(3.f)
		, Radius(5.f)
	{ }

	float ConstantAttenuation, LinearAttenuation, QuadraticAttenuation;
	float Radius;
	Color color;

	glm::vec3 Specular;
	glm::vec3 Diffuse;
	float specularExponent;
	float Scale;
};

}
}

#endif // !Components_PointLight_h__