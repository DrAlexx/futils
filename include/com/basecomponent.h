#pragma once

#include <com/iunknown.h>
#include <util/refobject.h>

namespace COM{

class BaseComponent : public IUnknown
{
public:
    BaseComponent(IUnknown *owner=nullptr);
    virtual ~BaseComponent() override{}

    virtual int addRef() override;
    virtual int Release() override;
    virtual Error QueryInterface(const std::string& id, void** ppv) override;

private:
    RefObject   ref;
    IUnknown    *owner;
};

}//namespace COM
