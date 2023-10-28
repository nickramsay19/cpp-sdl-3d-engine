#include <iostream>
#include <string>
#include <memory>
#include <utility>
#include <numbers> // std::numbers::pi
#include <cmath>
#include <algorithm>
#include <optional>
#include <SDL.h>
#include "include/Triangle.hpp"
#include "include/CohenSutherlandClip.hpp"

constexpr int WIN_WIDTH = 1080;
constexpr int WIN_HEIGHT = 720;
constexpr double WIN_ASPECT_RATIO = static_cast<double>(WIN_WIDTH) / static_cast<double>(WIN_HEIGHT);
constexpr std::string WIN_TITLE ("Nick's Window");

//constexpr double PI = std::numbers::pi;
constexpr double CAM_FOV = PI/2.0;
constexpr double CAM_DIST = 1.0;
constexpr double CAM_GAP = 0.1;

void project_point(Point<double>& p, const Point<double>& camera) {
    // project x and y into orthographic perspective (?)
    const double f = 1.0 / tanl(CAM_FOV / 2.0);
    const double x_projected = f * p.x;
    const double y_projected = f * p.y;

    // normalize z
    const double zFar = camera.z + CAM_DIST;
    const double zNear = camera.z + CAM_GAP;
    const double z_normalized = p.z * (zFar/(zFar-zNear)) - (zFar*zNear/(zFar-zNear));
    
    //p.x = WIN_ASPECT_RATIO * x_projected / p.z;
    p.x = x_projected / p.z;
    p.y = y_projected / p.z;
    p.z = z_normalized;

}

Point<int> scale_point_to_win(const Point<double>& p) {
    return Point<int> (
        (WIN_WIDTH/2.0) * p.x + (WIN_WIDTH/2.0),
        WIN_HEIGHT - ((WIN_HEIGHT/2.0) * p.y + (WIN_HEIGHT/2.0)), // flip vertically
        p.z
    );
}

int main(int argc, char* argv[]) {
    // initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // create a window
    SDL_Window* window = SDL_CreateWindow(WIN_TITLE.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // create a renderer for the window
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    //Point<double> camera = Point<double>(0,0,0);
    Mesh<double> cube = MakeCube<double>(Point<double>(0.0,0.0,0.0), 0.2);

    // main loop
    bool quit = false;
    int tick = 0;

    double camera = 0.0;
    Point<double> player (0.0, 0.0, 0.0);

    while (!quit) {
        //player.Display();
        //std::cout << '\n';

        const double r0 = sqrtl(pow(player.x,2) + pow(player.z,2));

        // handle events on queue
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            // user requests quit
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_KEYDOWN) {
                // select surfaces based on key press
                switch (e.key.keysym.sym) {
                    case SDLK_q: 
                        quit = true;
                        break;
                    case SDLK_a:
                        player.x += 0.05 * cosl(camera + PI/2.0 + PI/2.0);
                        player.z += 0.05 * sinl(camera + PI/2.0 + PI/2.0);
                        break;
                    case SDLK_d:
                        player.x += 0.05 * cosl(camera + PI/2.0 - PI/2.0);
                        player.z += 0.05 * sinl(camera + PI/2.0 - PI/2.0);
                        break;
                    case SDLK_w:
                        player.x += 0.05 * cosl(camera + PI/2.0);
                        player.z += 0.05 * sinl(camera + PI/2.0);
                        break;
                    case SDLK_s:
                        player.x += 0.05 * cosl(camera + PI/2.0 + PI);
                        player.z += 0.05 * sinl(camera + PI/2.0 + PI);
                        break;
                    case SDLK_RIGHT: 
                        camera += 0.05;
                        break;
                    case SDLK_LEFT: 
                        camera -= 0.05;
                        break;
                    case SDLK_UP: 
                        break;
                    case SDLK_DOWN: 
                        break;
                    default:
                        break;
                }
            }
        }

        // clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (const Triangle<double>& t : cube.tris) {

            Triangle<double> tc = t;
            tc.a -= player;
            tc.b -= player;
            tc.c -= player;

            // check if in fov
            const double theta_a = atan2l(tc.a.z, tc.a.x) - PI/2.0;
            const bool in_fov_a = theta_a > camera - CAM_FOV/1.0 && theta_a < camera + CAM_FOV/1.0;

            const double theta_b = atan2l(tc.b.z, tc.b.x) - PI/2.0;
            const bool in_fov_b = theta_b > camera - CAM_FOV/1.0 && theta_b < camera + CAM_FOV/1.0;

            const double theta_c = atan2l(tc.c.z, tc.c.x) - PI/2.0;
            const bool in_fov_c = theta_c > camera - CAM_FOV/1.0 && theta_c < camera + CAM_FOV/1.0;

            std::cout << in_fov_a << "," << in_fov_b << "," << in_fov_c << '\n';

            // rotate around camera
            double r_xz = 0.0;
            double a0_xz = 0.0;
            r_xz = sqrtl(powl(tc.a.x, 2) + powl(tc.a.z, 2));
            a0_xz = atan2l(tc.a.z, tc.a.x);
            tc.a.x = r_xz * cosl(a0_xz + camera);
            tc.a.z = r_xz * sinl(a0_xz + camera);

            r_xz = sqrtl(powl(tc.b.x, 2) + powl(tc.b.z, 2));
            a0_xz = atan2l(tc.b.z, tc.b.x);
            tc.b.x = r_xz * cosl(a0_xz + camera);
            tc.b.z = r_xz * sinl(a0_xz + camera);

            r_xz = sqrtl(powl(tc.c.x, 2) + powl(tc.c.z, 2));
            a0_xz = atan2l(tc.c.z, tc.c.x);
            tc.c.x = r_xz * cosl(a0_xz + camera);
            tc.c.z = r_xz * sinl(a0_xz + camera);


            // orthographic projection
            project_point(tc.a, player);
            project_point(tc.b, player);
            project_point(tc.c, player);


            // scale to size of screen
            Triangle<int> tci (
                scale_point_to_win(tc.a),
                scale_point_to_win(tc.b),
                scale_point_to_win(tc.c)
            );

            // clipping/culling
            using OptionalLine = std::optional<std::tuple<int,int,int,int>>;

            const OptionalLine ab_clip = cohen_sutherland_clip(tci.a.x, tci.a.y, tci.b.x, tci.b.y, 0, 0, WIN_WIDTH, WIN_HEIGHT);
            if (ab_clip && in_fov_a && in_fov_b) {
                const auto &[x1, y1, x2, y2] = ab_clip.value();
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            } 
            const OptionalLine bc_clip = cohen_sutherland_clip(tci.b.x, tci.b.y, tci.c.x, tci.c.y, 0, 0, WIN_WIDTH, WIN_HEIGHT);
            if (bc_clip && in_fov_b && in_fov_c) {
                const auto &[x1, y1, x2, y2] = bc_clip.value();
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
            const OptionalLine ca_clip = cohen_sutherland_clip(tci.c.x, tci.c.y, tci.a.x, tci.a.y, 0, 0, WIN_WIDTH, WIN_HEIGHT);
            if (ca_clip && in_fov_c && in_fov_a) {
                const auto &[x1, y1, x2, y2] = ca_clip.value();
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
            }
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
        tick += 1;
    }

    // cleanup and exit
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
