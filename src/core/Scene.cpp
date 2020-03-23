//
// Created by Krisu on 2020-02-05.
//


#include "Scene.hpp"
#include "GlobalTransform.hpp"
#include "LightInformation.hpp"
#include "Engine.hpp"
#include "GameObject.hpp"
#include "Skybox.hpp"
#include "Light.hpp"
#include "Material.hpp"
#include "Renderer.hpp"
#include "Mesh.hpp"
#include "DirectionalShadow.hpp"
#include "Transform.hpp"
//Bullet 增加了include
#include "MyBullet.hpp"


void Scene::Build() {
    IBL const& ibl = up_skybox->GetIBL();
    for (auto& up_game_object : up_game_objects) {
        /* Check if it's renderable */
        try {
            auto& material = up_game_object->GetComponent<Material>();
            material.setIBLTextures(ibl);
            material.updateShaderUniform();
        } catch (NoComponent&) {
            // TODO: better mechanism
            continue;
        }

        /* Call BeforeRenderLoop() for all components */
        for (auto &up_component : up_game_object->components_map) {
            auto & component = up_component.second;
            component->BeforeRenderLoop();
        }
    }
}

GameObject &Scene::CreateGameObject() {
    up_game_objects.emplace_back(std::make_unique<GameObject>());
    return *up_game_objects.back();
}

void Scene::Update() {
    Engine& engine = Engine::GetInstance();
    auto& renderer = engine.GetRenderer();

    /* 1 - Updating Shared GPU memory */
    this->UpdateUniformBlocks();

    /* 2 - Scene update */
    renderer.ResetViewport();

    //Bullet让步增加了一步
    MBWorld::getWorld()->takeStep(10);

    for (auto& up_game_obj : up_game_objects) {
        // BEFORE
        for (auto &it : up_game_obj->components_map) {
            auto & component = it.second;
            component->BeforeRenderPass();
        }
        //Bullet 增加了对所有obj的transform修改
        auto& transform = up_game_obj->GetComponent<Transform>();
        auto& physics = up_game_obj->GetComponent<Physics>();
        btTransform trans; physics.Body->getMotionState()->getWorldTransform(trans);
        btQuaternion q = trans.getRotation();
        btVector3 pos = trans.getOrigin(), axis = q.getAxis(); btScalar angle = q.getAngle();
        transform.SetPosition(pos.x, pos.y, pos.z); transform.SetRotation(axis.x, axis.y, axis.z, angle);

        // RENDERING
        try {
            auto& mesh = up_game_obj->GetComponent<Mesh>();
            auto& material = up_game_obj->GetComponent<Material>();
            auto& shader = material.GetShader();
            shader.UseShaderProgram();
            mesh.DrawCall();
        } catch (NoComponent&) {
            return ;
        }
        up_skybox->Draw();

        // AFTER
        for (auto &it : up_game_obj->components_map) {
            auto & component = it.second;
            component->AfterRenderPass();
        }
    }
}


void Scene::UpdateUniformBlocks() {
    Engine& engine = Engine::GetInstance();
    /* GLobalTransform Uniform Block */
    auto& globalTransform = engine.GetUniformBuffer<GlobalTransform>();
    globalTransform.UpdateView(GetCurrentCamera().GetViewMatrix());
    globalTransform.UpdateProjeciton(GetCurrentCamera().GetProjectionMatrix());

    /* LightInformation Uniform Block */
    auto& lightInfo = engine.GetUniformBuffer<LightInformation>();
    lightInfo.UpdateLightSize(LightManager::GetLightsCount());
    lightInfo.UpdateCameraPosition(GetCurrentCamera().Position());
}




