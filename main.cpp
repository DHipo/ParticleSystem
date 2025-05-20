#include <iostream>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <SDL3/SDL.h>
#include <SDL3/SDL_camera.h>

#define NUMBER_OF_PARTICLES 3
#define COEFFICIENT_OF_RESTITUTION 0.8f

struct Vector2
{
    float x;
    float y;

    Vector2 operator+(Vector2 other)
    {
        return Vector2{x + other.x, y + other.y};
    }

    Vector2 operator-(Vector2 other)
    {
        return Vector2{x - other.x, y - other.y};
    }
    Vector2 operator+=(Vector2 other)
    {
        return Vector2{x += other.x, y += other.y};
    }
    Vector2 operator-=(Vector2 other)
    {
        return Vector2{x -= other.x, y -= other.y};
    }
    Vector2 operator*(float scalar)
    {
        return Vector2{x * scalar, y * scalar};
    }

    float operator*(Vector2 other)
    {
        return x * other.x + y * other.y;
    }

    Vector2 operator/(float scalar)
    {
        return Vector2{x / scalar, y / scalar};
    }

    Vector2 operator/(Vector2 other)
    {
        if (other.x == 0 || other.y == 0)
        {
            return Vector2{(float)(rand() % 10) / 10, (float)(rand() % 10) / 10};
        }
        return Vector2{x / other.x, y / other.y};
    }

    float norma()
    {
        return sqrt(x * x + y * y);
    }

    Vector2 normalize()
    {
        float n = norma();
        if (n == 0)
            return Vector2{0, 0};
        return Vector2{x / n, y / n};
    }
};

Vector2 windowSize = {640, 480};
struct Particle
{
    Vector2 position = {0, 0};
    Vector2 velocity = {0, 0};
    Vector2 acceleration = {0, 0};
    Vector2 force = {0, 0};
    Vector2 size = {0, 0};
    float mass = 1.f;
    Uint8 color = 255;

    void Update()
    {
        if (position.x < 0)
        {
            position.x = 0;
            velocity.x = -velocity.x;
        }
        if (position.x + size.x > windowSize.x)
        {
            position.x = windowSize.x - size.x;
            velocity.x = -velocity.x;
        }

        if (position.y < 0)
        {
            position.y = 0;
            velocity.y = -velocity.y;
        }

        if (position.y + size.y > windowSize.y)
        {
            position.y = windowSize.y - size.y;
            velocity.y = -velocity.y;
        }

        // i have to check if the particle has a force
        if (force.x != 0 || force.y != 0)
        {
            acceleration = force / mass;
            force = {0, 0};
        }
        else
        {
            acceleration = {0, 0};
        }
        position += velocity;
        velocity += acceleration;
    }

    bool checkCollision(Particle other)
    {
        return (position.x < other.position.x + other.size.x &&
                position.x + size.x > other.position.x &&
                position.y < other.position.y + other.size.y &&
                position.y + size.y > other.position.y);
    }
};

int max(int a, int b)
{
    return (a > b) ? a : b;
}

int min(int a, int b)
{
    return (a < b) ? a : b;
}

int main()
{
    srand(static_cast<unsigned>(time(0)));
    std::cout << "Hello SDL3" << std::endl;
    if (!SDL_Init(SDL_INIT_EVENTS))
    {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Hello SDL3", 640, 480, SDL_WINDOW_RESIZABLE);
    if (window == nullptr)
    {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (renderer == nullptr)
    {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
    std::vector<Particle> particles;
    particles.reserve(100);
    for (int i = 0; i < NUMBER_OF_PARTICLES; ++i)
    {
        float mass = float(rand() % 4) + 1;
        Particle particle = {
            {static_cast<float>(rand() % 640),
             static_cast<float>(rand() % 480)},
            {static_cast<float>(rand() % 3),
             static_cast<float>(rand() % 3)},
            {0, 0},
            {0, 0},
            {20 / mass, 20 / mass},
            mass};
        particles.push_back(particle);
    }
    bool applyForce = false;
    bool stopGame = false;
    Vector2 mouse = {0, 0};
    SDL_Event event;
    while (true)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT)
            {
                SDL_DestroyRenderer(renderer);
                SDL_DestroyWindow(window);
                SDL_Quit();
                return 0;
            }
            if (event.type == SDL_EVENT_WINDOW_RESIZED)
            {
                SDL_SetWindowSize(window, event.window.data1, event.window.data2);
                windowSize.x = (float)event.window.data1;
                windowSize.y = (float)event.window.data2;
                // i should set the viewport to the new size
                SDL_SetRenderViewport(renderer, NULL);
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
                SDL_RenderClear(renderer);
            }
            if (event.type == SDL_EVENT_MOUSE_MOTION)
            {
                mouse.x = event.motion.x;
                mouse.y = event.motion.y;
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    float mass = float(rand() % 4) + 1;
                    Particle particle = {
                        {static_cast<float>(event.button.x),
                         static_cast<float>(event.button.y)},
                        {static_cast<float>(rand() % 10 - 5),
                         static_cast<float>(rand() % 10 - 5)},
                        {0, 0},
                        {0, 0},
                        {14 / mass, 14 / mass},
                        mass};
                    SDL_Log("added particle at %.2f %.2f", event.button.x, event.button.y);
                    particles.push_back(particle);
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    applyForce = !applyForce;
                }
            }
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                {
                    stopGame = !stopGame;
                }
                if (event.key.key == SDLK_R)
                {
                    particles.clear();
                }
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 125);
        SDL_RenderClear(renderer);

        if (!stopGame)
        {
            if (applyForce)
            {
                // i will apply a force ta all the particles in the direction of the mouse
                for (Particle &particle : particles)
                {
                    Vector2 direction = {mouse.x - particle.position.x,
                                         mouse.y - particle.position.y};
                    float distance = direction.norma();
                    if (distance > 0)
                    {
                        direction = direction.normalize();
                        particle.force += direction * 0.8f;
                    }
                }
            }

            for (Particle &particle : particles)
            {
                for (Particle &other : particles)
                {
                    // Dentro del bucle de colisión (donde verificas if (particle.checkCollision(other)))
                    if (&particle != &other && particle.checkCollision(other))
                    {
                        particle.color = particle.color == 0 ? 0 : particle.color - 20;
                        other.color = other.color == 0 ? 0 : other.color - 20;
                        // 1. Calcular dirección de colisión y separar posiciones
                        Vector2 collisionDir = particle.position - other.position;
                        float overlapX = (particle.size.x + other.size.x) - std::abs(collisionDir.x);
                        float overlapY = (particle.size.y + other.size.y) - std::abs(collisionDir.y);

                        // Separar en el eje de menor penetración
                        if (overlapX < overlapY)
                        {
                            float sign = collisionDir.x > 0 ? 1.f : -1.f;
                            particle.position.x += sign * overlapX * 0.5f;
                            other.position.x -= sign * overlapX * 0.5f;
                        }
                        else
                        {
                            float sign = collisionDir.y > 0 ? 1.f : -1.f;
                            particle.position.y += sign * overlapY * 0.5f;
                            other.position.y -= sign * overlapY * 0.5f;
                        }

                        // 2. Calcular nuevas velocidades usando física de colisiones
                        Vector2 normal = collisionDir.normalize();
                        Vector2 relativeVel = particle.velocity - other.velocity;
                        float velAlongNormal = relativeVel * normal;

                        // Solo resolver si los objetos se están acercando
                        if (velAlongNormal > 0)
                            continue;

                        float cor = COEFFICIENT_OF_RESTITUTION;
                        float impulseScalar = -(1 + cor) * velAlongNormal;
                        impulseScalar /= (1 / particle.mass + 1 / other.mass);

                        Vector2 impulse = normal * impulseScalar;
                        particle.velocity += impulse / particle.mass;
                        other.velocity -= impulse / other.mass;
                        
                        if (particles.size() > 2000) continue;

                        float mass = float(rand() % 4) + 1;
                        particles.push_back({
                            {static_cast<float>(rand() % (int) windowSize.x),
                             static_cast<float>(rand() % (int) windowSize.y)},
                            {particle.velocity.x,
                            particle.velocity.y},
                            {0, 0},
                            {0, 0},
                            {20 / mass, 20 / mass},
                            mass
                        });
                    }
                }
            }

            for (Particle &particle : particles)
            {
                particle.Update();
            }
        }

        for (Particle &particle : particles)
        {
            Uint8 color = static_cast<Uint8>(255 / particle.mass);
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_ADD);
            SDL_SetRenderDrawColor(renderer, 255, particle.color, particle.color, color);
            SDL_FRect rect = {particle.position.x,
                              particle.position.y, particle.size.x, particle.size.y};
            SDL_RenderFillRect(renderer, &rect);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);

        if (particles.size() > 2000)
            particles.erase(particles.begin());
    }
    return 0;
}