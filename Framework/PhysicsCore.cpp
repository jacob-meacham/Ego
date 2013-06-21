#include <string>
#include "PhysicsCore.h"
#include "Sprite.h"
PhysicalEntity::PhysicalEntity() : m_Weight(0), m_Elasticity(0), m_Velocity(0.f, 0.f) {
}
//////////////////////////////////////////////////////////////////////////////////
/// Setter to set the name of this entity.
void PhysicalEntity::SetName(const std::string & name) { m_Name = name; }
//////////////////////////////////////////////////////////////////////////////////
/// Setter to set the interactive state of this entity.
void PhysicalEntity::SetIsInteractive(bool i) { m_interactive = i; }
//////////////////////////////////////////////////////////////////////////////////
/// Getter to return the interactive state of this entity.
bool PhysicalEntity::GetIsInteractive() const { return m_interactive; }
//////////////////////////////////////////////////////////////////////////////////
/// Getter to return the name of this entity.
const std::string & PhysicalEntity::GetName() const { return m_Name; }
//////////////////////////////////////////////////////////////////////////////////
/// Moves the bounding sphere of this entity by a translation vector
void PhysicalEntity::Move(float x, float y) { m_BoundingSphere.center.x += x; m_BoundingSphere.center.y += y; }
//////////////////////////////////////////////////////////////////////////////////
/// Set the X velocity of this entity.
void PhysicalEntity::SetXVel(float x) {
	m_Velocity.x = x;
}
//////////////////////////////////////////////////////////////////////////////////
/// Set the Y velocity of this entity.
void PhysicalEntity::SetYVel(float y) {
	m_Velocity.y = y;
}
//////////////////////////////////////////////////////////////////////////////////
/// Set the X and Y velocity of this entity.
void PhysicalEntity::SetXYVel(float x, float y) {
	m_Velocity.x = x;
	m_Velocity.y = y;
}
//////////////////////////////////////////////////////////////////////////////////
/// Getter to return the X velocity of this entity.
float PhysicalEntity::GetXVel() const { return m_Velocity.x; }
//////////////////////////////////////////////////////////////////////////////////
/// Getter to return the name of this entity.
float PhysicalEntity::GetYVel() const { return m_Velocity.y; }
//////////////////////////////////////////////////////////////////////////////////
/// Getter to return the name of this entity.  Pass in 2 floats for the velocity.
void PhysicalEntity::GetXYVel(float &x, float &y) const {
	x = m_Velocity.x;
	y = m_Velocity.y;
}
//////////////////////////////////////////////////////////////////////////////////
//Checks if this PhysicalEntity is of type e. Returns true if that is the case
bool PhysicalEntity::checkType(EntityType e) const {
	return (e == PHYSICALENTITY);
}