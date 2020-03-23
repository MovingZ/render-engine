//
// Created by Moving on 2020/3/17 0017.
//

#ifndef BULLET3TEXT_MYBULLET_H
#define BULLET3TEXT_MYBULLET_H

#include <iostream>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <map>

#define PHYSICS_BOX 0
#define PHYSICS_SPHERE 1

class MBWorld {
private:
    ///World
    static MBWorld* TheWorld;
    btAxisSweep3* BroadPhase;
    btDefaultCollisionConfiguration* CollisionConfiguration;
    btCollisionDispatcher* Dispatcher;
    btSequentialImpulseConstraintSolver* Solver;
    btDiscreteDynamicsWorld* DynamicsWorld;
    float CalcFreq;///计算频率
    MBWorld(btVector3 aabbMin, btVector3 aabbMax, int maxProxies);
public:
    static MBWorld* createWorld(btVector3 aabbMin, btVector3 aabbMax, int maxProxies);///创建世界
    static MBWorld* getWorld();///创建世界
    ~MBWorld();
    void setGravity(float x, float y, float z);
    void setCalcFreq(float freq);
    void takeStep(int numb);

    ///Shape
    btAlignedObjectArray<btCollisionShape*> Shapes;
    std::map<int, btCollisionShape*> RecordedShapes;
    void useShape(btCollisionShape* shape);///使用形状创建下一个物体
    bool recordShape(btCollisionShape* shape, int numb);///将形状存储在一个数字的位置上(不能为0)
    bool shapeCheckRecorded(int numb);///检查数字上是否有形状

    ///Body
    btRigidBody* createRigidBody(float x, float y, float z, float mass, int shapeIndex = 0);
    static void bodySetRotate(btRigidBody* body, float x, float y, float z);
    static void bodySetRotate(btRigidBody* body, float axix_x, float axix_y, float axix_z, float angle);
    static void bodySetPos(btRigidBody* body, float x, float y, float z);
    static void bodySetPosRelative(btRigidBody* body, float x, float y, float z);
    ///
};



class Physics {
public:
    Physics(int type, float x, float y, float z, float mass = 0);
    Physics(int type, float r, float mass = 0);
    btRigidBody* Body;
    int Type;
};

#endif //BULLET3TEXT_MYBULLET_H



