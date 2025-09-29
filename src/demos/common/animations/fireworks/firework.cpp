#include <demos/common/animations/fireworks/firework.h>
#include <demos/common/core/demo-utils.h>

namespace demos::common::animations::fireworks
{

using namespace gfx::core::types;
using namespace gfx::math;
using namespace demos::common::core;

void Firework::process(const double dt)
{
    switch (state)
    {
        case State::Ascending:
            do_ascending(dt);
            break;

        case State::Exploding:
            do_exploding(dt);
            break;

        default:
            break;
    }
}

void Firework::explode()
{
    state = State::Exploding;
    renderer->remove_item(shape);

    int num_particles { utils::random_int(max_particles / 2, max_particles) };
    for (int i = 0; i < num_particles; ++i)
    {
        double angle { utils::random_double(0, 360) };
        double speed { utils::random_double(particle_speed * 0.1, particle_speed * 1.25) };
        Vec2d velocity { Vec2d::from_angle_degrees(angle, speed) - Vec2d { 0, particle_speed / 10 } };

        double size { utils::random_double(particle_size * 0.5, particle_size * 1.5) };
        double lifespan { utils::random_double(particle_lifespan_ms * 0.75, particle_lifespan_ms * 1.25) };

        std::vector<Color4> color { colors[rand() % colors.size()] };
        particles.emplace_back(renderer, position, velocity, Vec2d { size * particle_x_factor, size / particle_x_factor }, color, lifespan);
    }
}

void Firework::do_ascending(const double dt)
{
    shape->set_rotation(velocity.angle());
    update_position(dt);
    apply_gravity(dt);

    do_smoke(dt);

    for (auto& particle : smoke_particles)
    {
        particle.process(dt);
    }

    if (velocity.y >= 0.0)
    {
        explode();
    }
}

void Firework::do_exploding(const double dt)
{
    for (auto& particle : particles)
    {
        particle.process(dt);
    }
    for (auto& particle : smoke_particles)
    {
        particle.process(dt);
    }

    particles.erase(std::remove_if(particles.begin(), particles.end(), 
                                   [](const Particle& p) { return p.done; }),
                    particles.end());

    smoke_particles.erase(std::remove_if(smoke_particles.begin(), smoke_particles.end(), 
                                          [](const Particle& p) { return p.done; }),
                          smoke_particles.end());

    if (particles.empty() && smoke_particles.empty())
    {
        state = State::Done;
    }
}

void Firework::do_smoke(const double dt)
{
    if (utils::time_ms() - last_smoke_time_ms > smoke_trail_interval_ms)
    {
        double angle { utils::random_double(-smoke_angle_variation_degrees, smoke_angle_variation_degrees) + 90 };
        double speed { utils::random_double(smoke_speed * 0.75, smoke_speed * 1.25) };
        Vec2d velocity { Vec2d::from_angle_degrees(angle, speed) };

        double size { utils::random_double(smoke_size * 0.5, smoke_size * 1.5) };
        double lifespan { utils::random_double(particle_lifespan_ms, particle_lifespan_ms * 2.0) };

        smoke_particles.emplace_back(renderer, position + Vec2d { 0, size }, velocity, Vec2d { size * smoke_x_factor, size / smoke_x_factor }, std::vector<Color4> { smoke_color }, lifespan);
        last_smoke_time_ms = utils::time_ms();
    }

}

void Firework::update_position(const double dt)
{
    position += velocity * dt;
    shape->set_pos(position);
}

void Firework::apply_gravity(const double dt)
{
    const double gravity = 9.81;
    velocity.y += gravity * dt;
}

}
