#include <iostream>
#include <cmath>
#include "MyBullet.h"
using namespace std;

int main() {
    ///创建世界之前需要设置世界的三个参数
    int maxProxies = 1024;///最大刚体数量
    btVector3 worldAabbMin(-10000,-10000,-10000);///最大边界
    btVector3 worldAabbMax(10000,10000,10000);///最小边界
    MBWorld MyWorld(worldAabbMin,worldAabbMax,maxProxies);///创建世界（封装）


    ///想要创建一个平面形状
    ///先设置一个形状
    btCollisionShape* GroundShape = new btBoxShape(btVector3(btScalar(1000.),btScalar(5.),btScalar(1000.)));
    ///这个形状只使用一次所以我们用useShape（封装）
    MyWorld.useShape(GroundShape);
    ///根据形状来创建物体，如果是一次性形状在useShape后不需要传递形状参数（封装）
    btRigidBody* Ground =MyWorld.createRigidBody(0,0,0,0);

    ///全部结束时会自动释放形状内存

    ///创建一个球
    ///这个球我们想复用，先想好我们想把形状绑定在数字1上，那我们先查看数字1是否已经有形状了
    if(!MyWorld.shapeCheckRecorded(1)){///数字1上没有形状那我们创一个绑上去（封装）
        btCollisionShape* RoundShape = new btSphereShape(btScalar (6.));
        ///用recordShape将形状绑定到数字（封装）
        MyWorld.recordShape(RoundShape,1);
    }
    ///使用绑定数字的现状时，最后添加一个数字作为传参（封装）
    btRigidBody* Round =  MyWorld.createRigidBody(0,20,0,1.0,1);




    ///更多的参数考虑，我们希望地面倾斜，球有粗糙度,有摩擦力，有空气阻力，修改速度等
    MyWorld.bodySetRotate(Ground,0,0,M_PI/8);///设置转角（弧度）
    MyWorld.bodySetPos(Round,1,100,0);///设置位置

    MyWorld.bodySetRotateRelative(Ground,0,0,0);///设置转角
    MyWorld.bodySetPosRelative(Round,0,0,0);///设置位置

    Round->setRollingFriction(1.0);///这个没有封装因为感觉没必要，修改滚动粗糙度
    Round->setFriction(0.5);///修改粗糙度,两种粗糙度我测试了几次应该是取在（0-1）而且貌似默认值不为0
    Round->setDamping(0.2,0.5);///空气阻力是对单个物体修改的



    Round->setLinearVelocity(btVector3(0,10,0));///可以修改速度，当然Round->可以看到更多可以修改的内容



    MyWorld.setGravity(0,-9.8,0);///重力默认就是（0，-10，0），你也可以用这个函数来修改它到合适的大小
    MyWorld.setCalcFreq(1.0/60.0);///修改时间步长，不修改默认就是1/60

    MyWorld.takeStep(10);///时间前进一步，10代表了精度，一般就取10就好了

    ///现在我想获取Body的位置和转角
    btVector3 Pos = Round->getWorldTransform().getOrigin();
    btVector3 Euler;///欧拉角xyz
    Round->getWorldTransform().getRotation().getEulerZYX(Euler[2],Euler[1],Euler[0]);

    btQuaternion bqTmp = Round->getWorldTransform().getRotation();
    btVector4 Four = btVector4(bqTmp.x(),bqTmp.y(),bqTmp.z(),bqTmp.w());///四元数xyzw

    ///结束，啥都没输出哦

    for(int i=0; i<=300; ++i){
        MyWorld.takeStep(10);
        btTransform trans;
        Round->getMotionState()->getWorldTransform(trans);
        float th = trans.getOrigin().getY();
        std::cout<<th<<std::endl;
        //last2 = last-th;
        //last = th;
    }


    return 0;
}
