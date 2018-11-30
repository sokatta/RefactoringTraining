#pragma once
class IVisitor;
class IField
{

public:
    virtual void onStep(IVisitor &player){}
    virtual void onPass(IVisitor &player){}
    virtual ~IField() = default;

};