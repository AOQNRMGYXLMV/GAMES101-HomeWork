//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    Vector3f hitColor;
    Intersection intersection = intersect(ray);
    Vector3f p = intersection.coords; // shading point
    Vector3f wo = -ray.direction;     // observation direction
    Vector3f N = normalize(intersection.normal);
    Material* mt = intersection.m;

    if (intersection.emit.norm() > 0) { // hit light
        return Vector3f(1);
    }
    if (!intersection.happened) { // no intersection
        return hitColor;
    }

    // direct illumination
    Vector3f L_dir(0);
    float pdf;
    Intersection sampled;
    sampleLight(sampled, pdf);
    Vector3f  x = sampled.coords; // sampled light position
    Vector3f ws = normalize(x - p);  // shading point to sample point
    Vector3f NN = normalize(sampled.normal); // normal of sampled position
    Intersection interLight = intersect(Ray(p, ws));
    float dist = (sampled.coords - interLight.coords).norm();
    if (dist < 0.01) {
        L_dir = sampled.emit * mt->eval(ws, wo, N) * dotProduct(ws, N) * dotProduct(-ws, NN) / dotProduct(x-p, x-p) / pdf;
    }

    // indirect illumination
    Vector3f L_indir(0);
    float p_RR = get_random_float();
    if (p_RR < RussianRoulette) {
        Vector3f wi = mt->sample(wo, N);
        Ray traceRay(p, wi);
        pdf = mt->pdf(wi, wo, N);
        L_indir = castRay(traceRay, depth) * mt->eval(wi, wo, N) * dotProduct(wi, N) / pdf / RussianRoulette;
    }

    hitColor = L_dir + L_indir;

    return hitColor;
}
