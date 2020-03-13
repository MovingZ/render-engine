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
#include "Shadow.hpp"


void Scene::Build() {
    IBL const& ibl = up_skybox->GetIBL();
    for (auto& up_game_object : up_game_objects) {
        /* Check if it's renderable */
        try {
            auto& material = up_game_object->GetComponent<Material>();
            auto& shader = material.GetShader();

            material.setIBLTextures(ibl);
            material.updateShaderUniform();

            shader.UseShaderProgram();
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

void Scene::CreateLight(const Light &light) {
    lights.push_back(light);
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

    /* 2 - Rendering Shadow map */
    Shadow shadow(1024, 1024);

    glViewport(0, 0, 1024, 1024);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow.depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);
    static Shader shadowMapGen { "shader/shadow-map-gen.vert",
                                 "shader/shadow-map-gen.frag"};
    // only for one light...
    assert(lights.size() == 1);
    for (auto const& light : lights) {
        // Treat it as directional light for now...
        glm::mat4 lightProjection = glm::ortho<float>(
                -10, 10, -10, 10,
                1.0, 100
                );
        glm::mat4 lightView = glm::lookAt(
                light.position,
                glm::vec3(0, 1, 0),
                glm::vec3(0, 1, 0));
        glm::mat4 lightSpaceTransform = lightProjection * lightView;
        shadowMapGen.Set("lightSpaceTransform", lightSpaceTransform);
        for (auto& up_game_obj : up_game_objects) {
            auto& mesh = up_game_obj->GetComponent<Mesh>();
            mesh.DrawCall();
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    /* 3 - Scene update */
    auto [w, h] = renderer.GetWindowSize();
#if __APPLE__
    w *= 2; h *= 2;
#endif
    glViewport(0, 0, w, h);
    for (auto& up_game_obj : up_game_objects) {
        // BEFORE
        for (auto &it : up_game_obj->components_map) {
            auto & component = it.second;
            component->BeforeRenderPass();
        }
        // RENDERING
        try {
            auto& mesh = up_game_obj->GetComponent<Mesh>();
            auto& material = up_game_obj->GetComponent<Material>();
            material.GetShader().UseShaderProgram();
            mesh.DrawCall();
        } catch(NoComponent&) {
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
    globalTransform.UpdateView(camera.GetViewMatrix());
    globalTransform.UpdateProjeciton(camera.GetProjectionMatrix());

    /* LightInformation Uniform Block */
    auto& lightInfo = engine.GetUniformBuffer<LightInformation>();
    for (int i = 0; i < lights.size(); i++) {
        lightInfo.UpdateLight(i, lights[i]);
    }
    lightInfo.UpdateLightSize(lights.size());
    lightInfo.UpdateCameraPosition(camera.Position());
}


