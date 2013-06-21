#pragma once

#include <list>
#include "Math.h"
#include "Define.h"

class Sprite;

struct BoundingSphere {
	float radius;
	Vector2 center;
};

//Each child of PhysicalEntity will have an EntityType assosciated with it.
enum EntityType {
	PHYSICALENTITY, //The entity is not a derived class
	SPRITE, //The entity is a sprite
	BALL, //The entity is the player's ball
	WALL, 
	ENEMY
};

class PhysicalEntity {
	protected:
		std::string		m_Name; ///< Base name of the entity, through which it is known.
		bool			m_interactive; ///< true if the physical entity should be included in collision tests.
		float			m_Elasticity;	///< Elasticity of the object (unused)
		float			m_Weight;		///< Weight of the object (unused)
		BoundingSphere  m_BoundingSphere;

		/// Structure which represents current Sprite movement.
		/** SpriteMovement is a 2-d vector detailing the velocity of the sprite in
			the x and y direction.
		*/
		Vector2 m_Velocity;

	public:
		PhysicalEntity();		
		void SetXVel(float x);
		void SetYVel(float y);
		void SetXYVel(float x, float y);
		float GetXVel() const;
		float GetYVel() const;
		void GetXYVel(float &x, float &y) const;
		const BoundingSphere* GetBoundingSphere() const { return &m_BoundingSphere; }
		void UpdateBoundingSphere(Vector2 center) { m_BoundingSphere.center.x = center.x; m_BoundingSphere.center.y = center.y; }
		virtual void Collide(PhysicalEntity* other) = 0;
		void Move(float x, float y);
		void SetName(const std::string & name);
		void SetIsInteractive(bool i);
		bool GetIsInteractive() const;
		const std::string & GetName() const;

		virtual bool checkType(EntityType e) const; //Checks if this PhysicalEntity is of type e. Returns true if true.

};

/// Collision layer from which all other layers derive.
/* HACK: This design is not all that clever, and should probably be changed.
   It uses the assumption that only sprites will be used in the inter-test (which should be true,
   since walls/world geometry never move and so should always be the caller of an inter-test).  However,
   it is sort of lame, so if a better methodology presents itself, let's switch!
*/
template <class T>
class BaseLayer {
	private:
		std::string m_Name;
		bool m_static;
	public:
		std::list<T*> m_members;
		BaseLayer() { SetName("Base"); }
		~BaseLayer() { FreeLayer(); }
		void RegisterEntity(T* entity) { m_members.push_back(entity); }
		void FreeLayer() { m_members.clear(); }
		bool GetStatic() { return m_static; }
		void SetStatic(bool s) { m_static = s; }
		std::string GetName() { return m_Name; }
		void SetName(std::string name) { m_Name = name; }
		virtual void InterTest(BaseLayer<Sprite>* other) = 0;
		virtual void IntraTest() = 0;
		bool BoundingSphereCollision(PhysicalEntity* entity1, PhysicalEntity* entity2)
		{
			BoundingSphere bs1 = *(entity1->GetBoundingSphere());
			BoundingSphere bs2 = *(entity2->GetBoundingSphere());

			// Determine the distance between the centers
			float distance = sqrt((float)(bs1.center.x - bs2.center.x)*(bs1.center.x - bs2.center.x) + 
										(bs1.center.y - bs2.center.y)*(bs1.center.y - bs2.center.y));

			float sumRadii = bs1.radius + bs2.radius;
	
			float diff = distance - sumRadii;

			if(diff >= 0 - FERROR) { return false; }

			// Penetration vector will be along line connecting the centers.
			float normalPenetrationVectorX = (bs1.center.x - bs2.center.x)/distance;
			float normalPenetrationVectorY = (bs1.center.y - bs2.center.y)/distance;

			entity1->Move(-normalPenetrationVectorX*diff, -normalPenetrationVectorY*diff); 
			return true;
		}
};