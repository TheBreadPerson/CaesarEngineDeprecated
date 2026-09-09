#include <globals.hpp>
#include <physics.hpp>
#include <glm/glm.hpp>
#include <iostream>
#include <collision.hpp>
#include <debug.hpp>


void PhysicsUpdate(Entity& p_ent, double deltaTime)
{
	Rigidbody* rb = p_ent.GetComponent<Rigidbody>();

	// While on ground, apply friction to the velocity
	if (rb->collDown) rb->frictionVector = -rb->velocity * rb->friction;
	// Air friction
	else rb->frictionVector = -glm::vec3(rb->velocity.x, 0.0f, rb->velocity.z) * rb->friction;

	// Apply gravity
	rb->velocity.y -= Physics::gravity * static_cast<float>(deltaTime);
	// Apply friction
	rb->velocity += rb->frictionVector * static_cast<float>(deltaTime);
	// Apply velocity
	p_ent.transform.position += rb->velocity * static_cast<float>(deltaTime);
	// Reset collision flags
	rb->collDown = false, rb->collUp = false, rb->collLeft = false, rb->collRight = false, rb->collForward = false, rb->collBack = false;
}

bool CheckCollision(Collider* a, Collider* b)
{
	Rigidbody* rb = a->entity->GetComponent<Rigidbody>();
	
	// Calculate half scales for easier collision checking
	float aHalfWidth = a->scale.x;
	float aHalfHeight = a->scale.y;
	float aHalfDepth = a->scale.z;

	float bHalfWidth = b->scale.x;
	float bHalfHeight = b->scale.y;
	float bHalfDepth = b->scale.z;

	// Store positions for both colliders to avoid repeated access
	float aPosX = a->entity->transform.position.x + a->offset.x;
	float aPosY = a->entity->transform.position.y + a->offset.y;
	float aPosZ = a->entity->transform.position.z + a->offset.z;

	float bPosX = b->entity->transform.position.x + b->offset.x;
	float bPosY = b->entity->transform.position.y + b->offset.y;
	float bPosZ = b->entity->transform.position.z + b->offset.z;

	// Check for collision in the X axis
	bool collisionX = (aPosX + aHalfWidth >= bPosX - bHalfWidth) &&
		(aPosX - aHalfWidth <= bPosX + bHalfWidth);

	// Check for collision in the Z axis
	bool collisionZ = (aPosZ + aHalfDepth >= bPosZ - bHalfDepth) &&
		(aPosZ - aHalfDepth <= bPosZ + bHalfDepth);

	// Check for collision in the Y axis
	bool collisionY = (aPosY + aHalfHeight >= bPosY - bHalfHeight) &&
		(aPosY - aHalfHeight <= bPosY + bHalfHeight);

	// If there is a collision in all axes
	if (collisionX && collisionZ && collisionY)
	{
		// Calculate penetration depth for each axis
		float penetrationX = std::min((aPosX + aHalfWidth) - (bPosX - bHalfWidth),
			(bPosX + bHalfWidth) - (aPosX - aHalfWidth));

		float penetrationY = std::min((aPosY + aHalfHeight) - (bPosY - bHalfHeight),
			(bPosY + bHalfHeight) - (aPosY - aHalfHeight));

		float penetrationZ = std::min((aPosZ + aHalfDepth) - (bPosZ - bHalfDepth),
			(bPosZ + bHalfDepth) - (aPosZ - aHalfDepth));

		// Determine the axis of least penetration to resolve the collision
		if (penetrationX < penetrationY && penetrationX < penetrationZ)
		{
			// Resolve collision on the X axis
			if (aPosX < bPosX) {
				a->transform->position.x -= penetrationX; // Move 'a' to the left
				rb->collRight = true;
			}
			else {
				a->transform->position.x += penetrationX; // Move 'a' to the right
				rb->collLeft = true;
			}
		}
		else if (penetrationY < penetrationX && penetrationY < penetrationZ)
		{
			// Resolve collision on the Y axis
			if (aPosY < bPosY) {
				a->transform->position.y -= penetrationY; // Move 'a' down
				rb->collUp = true;
				rb->velocity.y = 0.0f;
			}
			else {
				a->transform->position.y += penetrationY; // Move 'a' up
				rb->collDown = true;
				if (rb->velocity.y < 0.0f) rb->velocity.y = 0.0f;
			}
		}
		else
		{
			// Resolve collision on the Z axis
			if (aPosZ < bPosZ) {
				a->transform->position.z -= penetrationZ; // Move 'a' backward
				rb->collForward = true;
			}
			else {
				a->transform->position.z += penetrationZ; // Move 'a' forward
				rb->collBack = true;
			}
		}
		return true; // Collision occurred
	}

	return false; // No collision
}


namespace Physics
{
	float gravity = 9.81f;
}