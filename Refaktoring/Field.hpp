#pragma once
class IVisitor;
class Field
{

public:
    virtual void onStep(IVisitor &player){}
    virtual void onPass(IVisitor &player){}
    virtual ~Field() = default;

};