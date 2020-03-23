//
// Created by Moving on 2020/3/17 0017.
//

#ifndef BULLET3TEXT_MYBULLET_H
#define BULLET3TEXT_MYBULLET_H

#include <iostream>
#include <btBulletCollisionCommon.h>
#include <btBulletDynamicsCommon.h>
#include <map>

#endif //BULLET3TEXT_MYBULLET_H

class MBWorld {
public:
    ///World
    btAxisSweep3* BroadPhase;
    //btBroadphaseInterface* OverlappingPairCache;
    btDefaultCollisionConfiguration* CollisionConfiguration;
    btCollisionDispatcher* Dispatcher;
    btSequentialImpulseConstraintSolver* Solver;
    btDiscreteDynamicsWorld* DynamicsWorld;
    float CalcFreq;///计算频率
    MBWorld(btVector3 aabbMin, btVector3 aabbMax, int maxProxies);///创建世界
    ~MBWorld();
    void setGravity(float x, float y, float z);
    void setCalcFreq(float freq);
    void takeStep(int numb);

    ///Shape
    btAlignedObjectArray<btCollisionShape*> Shapes;
    std::map<int,btCollisionShape*> RecordedShapes;
    void useShape(btCollisionShape* shape);///使用形状创建下一个物体
    bool recordShape(btCollisionShape* shape,int numb);///将形状存储在一个数字的位置上(不能为0)
    bool shapeCheckRecorded(int numb);///检查数字上是否有形状

    ///Body
    btRigidBody* createRigidBody(float x, float y, float z, float mass, int shapeIndex = 0);
    void bodySetRotate(btRigidBody* body, float x, float y, float z);
    void bodySetRotateRelative(btRigidBody* body, float x, float y, float z);
    void bodySetPos(btRigidBody* body, float x, float y, float z);
    void bodySetPosRelative(btRigidBody* body, float x, float y, float z);
    ///
};

MBWorld::MBWorld(btVector3 aabbMin, btVector3 aabbMax, int maxProxies) {
    ///宽相
    BroadPhase = new btAxisSweep3(aabbMin,aabbMax,maxProxies);
    //OverlappingPairCache = new btDbvtBroadphase();
    ///碰撞属性 调度
    CollisionConfiguration = new btDefaultCollisionConfiguration();
    Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
    ///物理模拟器
    Solver = new btSequentialImpulseConstraintSolver;
    /// 世界
    DynamicsWorld = new btDiscreteDynamicsWorld(Dispatcher,BroadPhase,Solver,CollisionConfiguration);
    DynamicsWorld->setGravity(btVector3(0,-10,0));
    CalcFreq = 1.0f/60.0f;
}

MBWorld::~MBWorld() {
    for (int i=DynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
    {
        btCollisionObject* obj = DynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        DynamicsWorld->removeCollisionObject( obj );
        delete obj;
    }
    for (int i=0;i<Shapes.size();++i)
    {
        btCollisionShape* shape = Shapes[i];
        Shapes[i] = 0;
        delete shape;
    }
    delete DynamicsWorld;
    delete Solver;
    delete Dispatcher;
    delete CollisionConfiguration;
    delete BroadPhase;
}

void MBWorld::setGravity(float x, float y, float z) {
    DynamicsWorld->setGravity(btVector3(x,y,z));
}

void MBWorld::setCalcFreq(float freq) {
    CalcFreq = freq;
}

void MBWorld::takeStep(int numb) {
    DynamicsWorld->stepSimulation(CalcFreq,numb);
    return;
}

void MBWorld::useShape(btCollisionShape *shape) {
    Shapes.push_back(shape);
}

bool MBWorld::recordShape(btCollisionShape *shape, int numb) {
    if(RecordedShapes.find(numb)!=RecordedShapes.end()){
        delete(shape);
        return false;
    }
    Shapes.push_back(shape);
    RecordedShapes[numb] = shape;
    return true;
}

bool MBWorld::shapeCheckRecorded(int numb) {
    if(RecordedShapes.find(numb)==RecordedShapes.end()) return false;
    return true;
}

btRigidBody* MBWorld::createRigidBody(float x, float y, float z, float mass, int shapeIndex) {
    btCollisionShape *shape;
    if(shapeIndex==0){
        shape = Shapes[Shapes.size()-1];
    }
    else if (RecordedShapes.find(shapeIndex)!=RecordedShapes.end()){
        shape = RecordedShapes[shapeIndex];
    }
    else return nullptr;

    btTransform Transform;
    Transform.setIdentity();
    Transform.setOrigin(btVector3(x,y,z));

    btScalar Mass(mass);
    bool IsDynamic = (Mass != 0.f);
    btVector3 LocalInertia(0,0,0);
    if(IsDynamic) shape->calculateLocalInertia(Mass, LocalInertia);

    btDefaultMotionState* MyMotionState = new btDefaultMotionState(Transform);
    btRigidBody::btRigidBodyConstructionInfo RBInfo(Mass,MyMotionState,shape,LocalInertia);
    btRigidBody* Body = new btRigidBody(RBInfo);
    DynamicsWorld->addRigidBody(Body);
    return Body;
}

void MBWorld::bodySetRotate(btRigidBody *body, float x, float y, float z) {
    btTransform trans = body->getWorldTransform();
    trans.setRotation(btQuaternion(y,x,z));
    body->setWorldTransform(trans);
}

void MBWorld::bodySetRotateRelative(btRigidBody *body, float x, float y, float z) {
    btTransform trans = body->getWorldTransform();
    trans.setRotation(trans.getRotation()+btQuaternion(y,x,z));
    body->setWorldTransform(trans);
}

void MBWorld::bodySetPos(btRigidBody *body, float x, float y, float z) {
    btTransform trans = body->getWorldTransform();
    trans.setOrigin(btVector3(x,y,z));
    body->setWorldTransform(trans);
}

void MBWorld::bodySetPosRelative(btRigidBody *body, float x, float y, float z) {
    btTransform trans = body->getWorldTransform();
    trans.setOrigin(trans.getOrigin()+btVector3(x,y,z));
    body->setWorldTransform(trans);
}













