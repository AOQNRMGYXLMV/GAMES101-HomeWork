//
// Created by goksu on 2/25/20.
//

#include <fstream>
#include <future>
#include <atomic>
#include <mutex>
#include "Scene.hpp"
#include "Renderer.hpp"

int calcPixels = 0;
std::mutex g_mutex;


inline float deg2rad(const float& deg) { return deg * M_PI / 180.0; }

const float EPSILON = 0.00001;

// calculate [st, ed) pixels color
std::vector<Vector3f> RenderRange(uint32_t st, uint32_t ed, int spp, const Scene& scene) {
    int total = scene.width * scene.height;
    std::vector<Vector3f> buf(ed - st);
    float scale = tan(deg2rad(scene.fov * 0.5));
    float imageAspectRatio = scene.width / (float)scene.height;
    Vector3f eye_pos(278, 273, -800);
    int m = 0;
    for (uint32_t id = st; id < ed; id++) {
        int j = id / scene.width;
        int i = id % scene.width;
        float x = (2 * (i + 0.5) / (float)scene.width - 1) *
                  imageAspectRatio * scale;
        float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

        Vector3f dir = normalize(Vector3f(-x, y, 1));
        for (int k = 0; k < spp; k++){
            buf[m] += scene.castRay(Ray(eye_pos, dir), 0) / spp;  
        }
        m++;
        if (m % 500 == 0) { // update progress
            std::lock_guard<std::mutex> lk(g_mutex);
            calcPixels += 500;
            UpdateProgress(1.f * calcPixels / total);
        }
    }

    int add = total % 500;
    {
        std::lock_guard<std::mutex> lk(g_mutex);
        calcPixels += add;
        UpdateProgress(1.f * calcPixels / total);
    }
    
    return buf;
}

// The main render function. This where we iterate over all pixels in the image,
// generate primary rays and cast these rays into the scene. The content of the
// framebuffer is saved to a file.
void Renderer::Render(const Scene& scene)
{
    int total = scene.width * scene.height;
    std::vector<Vector3f> framebuffer(total);

    const int concurrency = std::thread::hardware_concurrency();
    int spp = 2500;
    std::cout << "concurrency: " << concurrency << "\n";
    std::cout << "SPP: " << spp << "\n";
    int step = (total) / concurrency;
    uint32_t st = 0;
    std::vector<std::future<std::vector<Vector3f>>> tasks;
    for (int i = 0; i < concurrency; i++) {
        uint32_t ed = st + step;
        if (i == concurrency - 1) ed = total;
        std::cout << "start a thread to calc range [" << st << ", " << ed << ")\n";
        tasks.push_back(std::async(RenderRange, st, ed, spp, std::cref(scene)));
        st += step;
    }

    st = 0;
    for (int i = 0; i < concurrency; i++) {
        int ed = st + step;
        if (i == concurrency - 1) ed = total;
        std::cout << "waiting for task " << i+1 << " complete.\n";
        const std::vector<Vector3f>& buf = tasks[i].get();
        std::cout << "task " << i+1 << " completed.\n";
        for (int id = st; id < ed; id++) {
            framebuffer[id] = buf[id - st];
        }
        st += step;
    }

    // float scale = tan(deg2rad(scene.fov * 0.5));
    // float imageAspectRatio = scene.width / (float)scene.height;
    // Vector3f eye_pos(278, 273, -800);
    // int m = 0;

    // // change the spp value to change sample ammount
    // int spp = 128;
    // std::cout << "SPP: " << spp << "\n";
    // for (uint32_t j = 0; j < scene.height; ++j) {
    //     for (uint32_t i = 0; i < scene.width; ++i) {
    //         // generate primary ray direction
    //         float x = (2 * (i + 0.5) / (float)scene.width - 1) *
    //                   imageAspectRatio * scale;
    //         float y = (1 - 2 * (j + 0.5) / (float)scene.height) * scale;

    //         Vector3f dir = normalize(Vector3f(-x, y, 1));
    //         for (int k = 0; k < spp; k++){
    //             framebuffer[m] += scene.castRay(Ray(eye_pos, dir), 0) / spp;  
    //         }
    //         m++;
    //     }
    //     UpdateProgress(j / (float)scene.height);
    // }
    // UpdateProgress(1.f);

    // save framebuffer to file
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
    for (auto i = 0; i < scene.height * scene.width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].x), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].y), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(clamp(0, 1, framebuffer[i].z), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);    
}
