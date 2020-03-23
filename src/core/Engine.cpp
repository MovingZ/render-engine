//
// Created by Krisu on 2020-02-05.
//

#if defined(WIN32) || defined(_WIN32)
#include <direct.h>
    #define chdir _chdir
#else
#include <unistd.h>
#endif

#include "Engine.hpp"
#include "Scene.hpp"
#include "UniformBlock.hpp"
#include "Skybox.hpp"
//Bullet �˴����include
#include "MyBullet.hpp"

Scene& Engine::CreateScene() {
    scenes.emplace_back(std::make_unique<Scene>());
    return *scenes.back();
}

Renderer& Engine::GetRenderer() {
    return renderer;
}

Engine::Engine() {
    /* Setting asset directory */
    chdir("..");
}


Engine& Engine::GetInstance() {
    static Engine engine;
    return engine;
}


Scene &Engine::GetScene(std::vector<Scene>::size_type index) {
    return *scenes[index];
}


Scene &Engine::GetCurrentScene() {
    assert(currentScene != nullptr);
    return *currentScene;
}


void Engine::MakeCurrentScene(Scene &scene) {
    currentScene = &scene;
}

template<typename... Args>
Shader &Engine::CreateShader(Args &&... args) {
    auto up_shader = std::make_unique<Shader>(std::forward<Args>(args)...);
    Shader& ret = *up_shader;
    shaders.push_back(std::move(up_shader));
    /* binding all enabled uniform blocks to newly created shader */
    for (auto& it : uniform_blocks) {
        auto& uniform_block = it.second;
        if (uniform_block->IsEnabled()) {
            uniform_block->BindShader(ret);
        }
    }
    return ret;
}


Shader &Engine::GetDefaultShader() {
    static Shader& default_ = CreateShader("shader/standard.vert",
                                           "shader/standard.frag");
    return default_;
}


Shader &Engine::GetTestShader() {
    static Shader& test = CreateShader("shader/test.vert", "shader/test.frag");
    return test;
}

//Bullet �˴���ʼ������
void Engine::EnablePhysics() {
    int maxProxies = 1024;///����������
    btVector3 worldAabbMin(-10000, -10000, -10000);///��С�߽�
    btVector3 worldAabbMax(10000, 10000, 10000);///���߽�
    MBWorld::createWorld(worldAabbMin, worldAabbMax, maxProxies);///�������磨��װ��
}