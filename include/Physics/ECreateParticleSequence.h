#ifndef EVENTS_ECREATEPARTICLESEQUENCE_H__
#define EVENTS_ECREATEPARTICLESEQUENCE_H__

#include "Core/EventBroker.h"
#include "Physics/CParticle.h"

namespace dd
{
namespace Events
{

struct CreateParticleSequence : public Event
{
    //Emitter
    double EmitterLifeTime = 100;
    glm::vec3 Position = glm::vec3(0);
    float GravityScale = 0.f;
    float SpawnRate = 1.f;
    int NumberOfTicks = 100;
    float Speed = 1.f;
    int ParticlesPerTick = 1.f;
    float Spread = glm::pi<float>();
    float EmittingAngle = glm::two_pi<float>();
    float MaxCount = 0;

    //Particle
    std::string SpriteFile = "Texures/Core/ErrorTexture.png";
    double ParticleLifeTime = 3.f;
    ParticleFlags::Type Flags = ParticleFlags::Type::powderParticle | ParticleFlags::Type::particleContactFilterParticle | ParticleFlags::Type::fixtureContactFilterParticle;
    float Radius = 1.f;
};

}
}

#endif

