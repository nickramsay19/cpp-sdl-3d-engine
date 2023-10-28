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

void project_point(Point<double>& p, const Point<double>& player, double camera) {

    // normalize x
    const double xFar = player.x + CAM_DIST;
    const double xNear = player.x + CAM_GAP;
    const double x_normalized = p.x * cosl(camera)*(xFar/(xFar-xNear)) - (xFar*xNear/(xFar-xNear));

    // normalize z
    const double zFar = player.z + CAM_DIST;
    const double zNear = player.z + CAM_GAP;
    const double z_normalized = p.z * (zFar/(zFar-zNear)) - (zFar*zNear/(zFar-zNear));

    // project x and y into orthographic perspective (?)
    const double f = 1.0 / tanl(CAM_FOV / 2.0);
    const double x_projected = f * p.x;
    const double y_projected = f * p.y;
    
    //p.x = WIN_ASPECT_RATIO * x_projected / p.z;
    p.x = x_projected / p.z;
    p.y = y_projected / p.z;
    p.z = p.z;

}

Point<int> scale_point_to_win(const Point<double>& p) {
    return Point<int> (
        (WIN_WIDTH/2.0) * p.x + (WIN_WIDTH/2.0),
        WIN_HEIGHT - ((WIN_HEIGHT/2.0) * p.y + (WIN_HEIGHT/2.0)), // flip vertically
        p.z
    );
}

double fix_angle(double angle) {
    if (angle < 0) {
        return angle = 2*PI + angle;
    } else if (angle > 2*PI) {
        angle = fmodl(angle, 2.0*PI);
    }
    return angle;
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
    Mesh<double> cube = MakeCube<double>(Point<double>(0.5,0.0,0.0), 0.2); // east
    cube.red = 255; 


    Mesh<double> cube2 = MakeCube<double>(Point<double>(0.5,0.0,0.0), 0.05);
    cube2.green = 255;

    std::vector<Mesh<double>> meshes {cube, cube2};

    // main loop
    bool quit = false;
    int tick = 0;

    double camera = 0.0; // facing east
    Point<double> player (0.0, 0.0, 0.0);

    while (!quit) {
        //player.Display();
        //std::cout << '\t';
        //std::cout << "" << camera << " (" << (camera/PI) * 180 << ")";
        //std::cout << "\t";
        //cube.origin.Display();
        //std::cout << '\n';

        // player distance from world origin
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
                        player.x += 0.05 * cosl(fix_angle(camera - PI/2.0));
                        player.z -= 0.05 * sinl(fix_angle(camera - PI/2.0));
                        break;
                    case SDLK_d:
                        player.x += 0.05 * cosl(fix_angle(camera + PI/2.0));
                        player.z -= 0.05 * sinl(fix_angle(camera + PI/2.0));
                        break;
                    case SDLK_w:
                        player.x += 0.05 * cosl(fix_angle(camera));
                        player.z -= 0.05 * sinl(fix_angle(camera));
                        break;
                    case SDLK_s:
                        player.x += 0.05 * cosl(fix_angle(camera + PI));
                        player.z -= 0.05 * sinl(fix_angle(camera + PI));
                        break;
                    case SDLK_RIGHT: 
                        camera = fix_angle(camera + 0.05);
                        break;
                    case SDLK_LEFT: 
                        camera = fix_angle(camera - 0.05);
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

        // move pointing cube
        const Point<double> player_pointing (
            player.x + 0.3*cosl(camera),
            player.y,
            player.z - 0.3*sinl(camera)
        );
        meshes[1].set_origin(player_pointing);

        // clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        for (const Mesh<double>& m : meshes) {
            for (const Triangle<double>& t : m.tris) {

                Triangle<double> tc = t;
                tc.a -= player;
                tc.b -= player;
                tc.c -= player;

                // rotate around camera
                double r_xz = 0.0;
                double a0_xz = 0.0;
                r_xz = sqrtl(powl(tc.a.x, 2) + powl(tc.a.z, 2));
                a0_xz = fix_angle(camera - PI/2.0 + atan2l(tc.a.z, tc.a.x));
                tc.a.x = r_xz * cosl(a0_xz);
                tc.a.z = r_xz * sinl(a0_xz);

                r_xz = sqrtl(powl(tc.b.x, 2) + powl(tc.b.z, 2));
                a0_xz = fix_angle(camera - PI/2.0 + atan2l(tc.b.z, tc.b.x));
                tc.b.x = r_xz * cosl(a0_xz);
                tc.b.z = r_xz * sinl(a0_xz);

                r_xz = sqrtl(powl(tc.c.x, 2) + powl(tc.c.z, 2));
                a0_xz = fix_angle(camera - PI/2.0 + atan2l(tc.c.z, tc.c.x));
                tc.c.x = r_xz * cosl(a0_xz);
                tc.c.z = r_xz * sinl(a0_xz);

                // orthographic projection
                project_point(tc.a, player, camera);
                project_point(tc.b, player, camera);
                project_point(tc.c, player, camera);

                // scale to size of screen
                Triangle<int> tci (
                    scale_point_to_win(tc.a),
                    scale_point_to_win(tc.b),
                    scale_point_to_win(tc.c)
                );

                // clipping/culling
                using OptionalLine = std::optional<std::tuple<int,int,int,int>>;

                SDL_SetRenderDrawColor(renderer, m.red, m.green, m.blue, 255);

                const OptionalLine ab_clip = cohen_sutherland_clip(tci.a.x, tci.a.y, tci.b.x, tci.b.y, 0, 0, WIN_WIDTH, WIN_HEIGHT);
                if (ab_clip) {
                    const auto &[x1, y1, x2, y2] = ab_clip.value();
                    //SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
                    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                } 
                const OptionalLine bc_clip = cohen_sutherland_clip(tci.b.x, tci.b.y, tci.c.x, tci.c.y, 0, 0, WIN_WIDTH, WIN_HEIGHT);
                if (bc_clip) {
                    const auto &[x1, y1, x2, y2] = bc_clip.value();
                    //SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                }
                const OptionalLine ca_clip = cohen_sutherland_clip(tci.c.x, tci.c.y, tci.a.x, tci.a.y, 0, 0, WIN_WIDTH, WIN_HEIGHT);
                if (ca_clip) {
                    const auto &[x1, y1, x2, y2] = ca_clip.value();
                    //SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
                }
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
