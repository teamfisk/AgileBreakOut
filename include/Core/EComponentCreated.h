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

#ifndef Event_ComponentCreated_h__
#define Event_ComponentCreated_h__

#include "EventBroker.h"
#include "Entity.h"
#include "Component.h"

namespace dd
{

namespace Events
{

/** Thrown when a new component is created and attached to an entity. */
struct ComponentCreated : Event
{
	/** The entity the component was attached to. */
	EntityID Entity;
	/** Pointer to the newly created component. */
	std::shared_ptr<dd::Component> Component;
};

}

}

#endif // Event_ComponentCreated_h__