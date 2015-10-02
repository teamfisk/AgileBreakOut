#ifndef DAYDREAM_PHYSICSSYSTEM_H
#define DAYDREAM_PHYSICSSYSTEM_H

#include <unordered_map>

#include "Core/System.h"
#include "Core/World.h"
#include "CRectangleShape.h"
#include "Physics/CPhysics.h"
#include <Box2D/Box2D.h>
#include "Core/CTransform.h"
#include "Transform/TransformSystem.h"
#include "Physics/EContact.h"
#include "Physics/ESetImpulse.h"
#include "Physics/CCircleShape.h"
#include "Core/EventBroker.h"
#include "Game/CPad.h"
#include "Physics/CWaterVolume.h"
#include "Game/CBall.h"
#include "Rendering/CSprite.h"
#include "Physics/CParticle.h"
#include "Physics/CParticleEmitter.h"
#include "Core/CTemplate.h"


namespace dd
{

namespace Systems
{


class PhysicsSystem : public System
{
    friend class ContractListener;

public:
    PhysicsSystem(World* world, std::shared_ptr<dd::EventBroker> eventBroker)
    : System(world, eventBroker) {}

    ~PhysicsSystem();

    EventRelay<PhysicsSystem, Events::SetImpulse> m_SetImpulse;
    bool SetImpulse(const Events::SetImpulse &event);

    void RegisterComponents(ComponentFactory* cf) override;
    void Initialize() override;

    // Called once per system every tick
    void Update(double dt) override;
    // Called once for every entity in the world every tick
    void UpdateEntity(double dt, EntityID entity, EntityID parent) override;

    // Called when components are committed to an entity
    void OnEntityCommit(EntityID entity) override;

    // Called when an entity is removed
    void OnEntityRemoved(EntityID entity) override;

private:
    b2Vec2 m_Gravity;
    b2World* m_PhysicsWorld;
    float m_TimeStep;
    float m_Accumulator;

    int m_VelocityIterations, m_PositionIterations;

    std::unordered_map<EntityID, b2Body*> m_EntitiesToBodies;
    std::unordered_map<b2Body*, EntityID> m_BodiesToEntities;

    void CreateBody(EntityID entity);

    b2ParticleSystem* m_WaterParticleSystem;
    std::vector<b2ParticleGroup*> t_ParticleGroup;

    std::unordered_map<EntityID, const b2ParticleHandle*> m_EntitiesToWaterParticleHandle;
    std::unordered_map<const b2ParticleHandle*, EntityID> m_WaterParticleHandleToEntities;

    std::vector<std::unordered_map<EntityID, const b2ParticleHandle*>> m_EntitiesToParticleHandle;
    std::vector<std::unordered_map<const b2ParticleHandle*, EntityID>> m_ParticleHandleToEntities;

    b2ParticleSystem* CreateParticleSystem(float radius, float gravityScale, int maxCount);
    void InitializeWater();
    void CreateParticleGroup(EntityID entity);
    void CreateParticleEmitter(EntityID entity);
    void UpdateParticleEmitters(double dt); //TODO: Remove them and particles if needed.

    //TODO: Fill struct with info needed.
    struct ParticleEmitter
    {
        std::vector<b2ParticleSystem*> ParticleSystem;
        std::vector<EntityID> ParticleEmitter;
        std::vector<EntityID> ParticleTemplate;
    };
    ParticleEmitter m_ParticleEmitters;

    //TODO: Struct med listor �r b�ttre �n en lista med structs
    struct Impulse
    {
        b2Body* Body;
        b2Vec2 Impulse;
        b2Vec2 Point;
    };
    std::list<Impulse> m_Impulses;



    class ContactListener : public b2ContactListener
    {
    public:
        ContactListener(PhysicsSystem* physicsSystem)
                : m_PhysicsSystem(physicsSystem) { }

        void BeginContact(b2Contact* contact)
        {
            b2WorldManifold worldManifold;

            contact->GetWorldManifold(&worldManifold);

            Events::Contact e;
            e.Entity1 = m_PhysicsSystem->m_BodiesToEntities[contact->GetFixtureA()->GetBody()];
            e.Entity2 = m_PhysicsSystem->m_BodiesToEntities[contact->GetFixtureB()->GetBody()];
            e.Normal = glm::normalize(glm::vec2(contact->GetManifold()->localNormal.x, contact->GetManifold()->localNormal.y));
            e.SignificantNormal = glm::normalize((glm::abs(e.Normal.x) > glm::abs(e.Normal.y)) ? glm::vec2(e.Normal.x, 0) : glm::vec2(0, e.Normal.y));

            m_PhysicsSystem->EventBroker->Publish(e);
        }
        void EndContact(b2Contact* contact)
        {

        }
        void PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
        {
            EntityID entityA = m_PhysicsSystem->m_BodiesToEntities[contact->GetFixtureA()->GetBody()];
            EntityID entityB = m_PhysicsSystem->m_BodiesToEntities[contact->GetFixtureB()->GetBody()];

            auto physicsComponentA = m_PhysicsSystem->m_World->GetComponent<Components::Physics>(entityA);
            auto physicsComponentB = m_PhysicsSystem->m_World->GetComponent<Components::Physics>(entityB);

            if (physicsComponentA != nullptr && physicsComponentB != nullptr) {
                if (physicsComponentA->Calculate || physicsComponentB->Calculate) {
                    // Turn of collisions
                    contact->SetEnabled(false);
                }
            }


        }
        void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
        {

        }

    private:
        PhysicsSystem* m_PhysicsSystem;
    };

    ContactListener* m_ContactListener;
};

}
}

#endif //DAYDREAM_PHYSICSSYSTEM_H
