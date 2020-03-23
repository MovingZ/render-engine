

#include "MyBullet.hpp"

MBWorld* MBWorld::TheWorld;

Physics::Physics(int type, float x, float y, float z, float mass) {
    if (type == PHYSICS_BOX) {
        btCollisionShape* Shape = new btBoxShape(btVector3(btScalar(x / 2.0), btScalar(y / 2.0), btScalar(z / 2.0)));
        MBWorld::getWorld()->useShape(Shape);
        ///根据形状来创建物体，如果是一次性形状在useShape后不需要传递形状参数（封装）
        Body = MBWorld::getWorld()->createRigidBody(0, 0, 0, mass);
        Type = PHYSICS_BOX;
    }
    else {
        btCollisionShape* Shape = new btSphereShape(btScalar(x));
        MBWorld::getWorld()->useShape(Shape);
        ///根据形状来创建物体，如果是一次性形状在useShape后不需要传递形状参数（封装）
        Body = MBWorld::getWorld()->createRigidBody(0, 0, 0, mass);
        Type = PHYSICS_SPHERE;
    }
}

Physics::Physics(int type, float r, float mass) {
    if (type == PHYSICS_SPHERE) {
        btCollisionShape* Shape = new btSphereShape(btScalar(r));
        MBWorld::getWorld()->useShape(Shape);
        ///根据形状来创建物体，如果是一次性形状在useShape后不需要传递形状参数（封装）
        Body = MBWorld::getWorld()->createRigidBody(0, 0, 0, mass);
        Type = PHYSICS_SPHERE;
    }
    else {
        btCollisionShape* Shape = new btBoxShape(btVector3(btScalar(r / 2.0), btScalar(r / 2.0), btScalar(r / 2.0)));
        MBWorld::getWorld()->useShape(Shape);
        ///根据形状来创建物体，如果是一次性形状在useShape后不需要传递形状参数（封装）
        Body = MBWorld::getWorld()->createRigidBody(0, 0, 0, mass);
        Type = PHYSICS_BOX;
    }
}


MBWorld::MBWorld(btVector3 aabbMin, btVector3 aabbMax, int maxProxies) {
    ///宽相
    BroadPhase = new btAxisSweep3(aabbMin, aabbMax, maxProxies);
    ///碰撞属性 调度
    CollisionConfiguration = new btDefaultCollisionConfiguration();
    Dispatcher = new btCollisionDispatcher(CollisionConfiguration);
    ///物理模拟器
    Solver = new btSequentialImpulseConstraintSolver;
    /// 世界
    DynamicsWorld = new btDiscreteDynamicsWorld(Dispatcher, BroadPhase, Solver, CollisionConfiguration);
    DynamicsWorld->setGravity(btVector3(0, -10, 0));
    CalcFreq = 1.0f / 60.0f;
}

MBWorld::~MBWorld() {
    for (int i = DynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = DynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        DynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }
    for (int i = 0; i < Shapes.size(); ++i)
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
    DynamicsWorld->setGravity(btVector3(x, y, z));
}

void MBWorld::setCalcFreq(float freq) {
    CalcFreq = freq;
}

void MBWorld::takeStep(int numb) {
    DynamicsWorld->stepSimulation(CalcFreq, numb);
}

void MBWorld::useShape(btCollisionShape* shape) {
    Shapes.push_back(shape);
}

bool MBWorld::recordShape(btCollisionShape* shape, int numb) {
    if (RecordedShapes.find(numb) != RecordedShapes.end()) {
        delete(shape);
        return false;
    }
    Shapes.push_back(shape);
    RecordedShapes[numb] = shape;
    return true;
}

bool MBWorld::shapeCheckRecorded(int numb) {
    return !(RecordedShapes.find(numb) == RecordedShapes.end());
}

btRigidBody* MBWorld::createRigidBody(float x, float y, float z, float mass, int shapeIndex) {
    btCollisionShape* shape;
    if (shapeIndex == 0) {
        shape = Shapes[Shapes.size() - 1];
    }
    else if (RecordedShapes.find(shapeIndex) != RecordedShapes.end()) {
        shape = RecordedShapes[shapeIndex];
    }
    else return nullptr;

    btTransform Transform;
    Transform.setIdentity();
    Transform.setOrigin(btVector3(x, y, z));

    btScalar Mass(mass);
    bool IsDynamic = (Mass != 0.f);
    btVector3 LocalInertia(0, 0, 0);
    if (IsDynamic) shape->calculateLocalInertia(Mass, LocalInertia);

    auto* MyMotionState = new btDefaultMotionState(Transform);
    btRigidBody::btRigidBodyConstructionInfo RBInfo(Mass, MyMotionState, shape, LocalInertia);
    auto* Body = new btRigidBody(RBInfo);
    DynamicsWorld->addRigidBody(Body);
    return Body;
}

void MBWorld::bodySetRotate(btRigidBody* body, float x, float y, float z) {
    btTransform trans = body->getWorldTransform();
    trans.setRotation(btQuaternion(y, x, z));
    body->setWorldTransform(trans);
}

void MBWorld::bodySetRotate(btRigidBody* body, float axix_x, float axix_y, float axix_z, float angle)
{
    btTransform trans = body->getWorldTransform();
    trans.setRotation(btQuaternion(btVector3(axix_x,axix_y,axix_z),angle));
    body->setWorldTransform(trans);
}



void MBWorld::bodySetPos(btRigidBody* body, float x, float y, float z) {
    btTransform trans = body->getWorldTransform();
    trans.setOrigin(btVector3(x, y, z));
    body->setWorldTransform(trans);
}

void MBWorld::bodySetPosRelative(btRigidBody* body, float x, float y, float z) {
    btTransform trans = body->getWorldTransform();
    trans.setOrigin(trans.getOrigin() + btVector3(x, y, z));
    body->setWorldTransform(trans);
}

MBWorld* MBWorld::createWorld(btVector3 aabbMin, btVector3 aabbMax, int maxProxies) {
    if (TheWorld != nullptr) return TheWorld;
    TheWorld = new MBWorld(aabbMin, aabbMax, maxProxies);
    return TheWorld;
}

MBWorld* MBWorld::getWorld() {
    if (TheWorld != nullptr) return TheWorld;
    return nullptr;
}