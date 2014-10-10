#include <com/basecomponent.h>

namespace COM{

BaseComponent::BaseComponent(IUnknown *own)
    :owner(own)
{
}

int BaseComponent::addRef()
{
    if(owner == nullptr)
        return ref.add_ref();
    else
        return owner->addRef();
}

int BaseComponent::Release()
{
    if(owner == nullptr){
        auto val = ref.release();
        if(val <= 0){
            delete this;
        }
        return val;
    }else{
        return owner->Release();
    }
}

Error BaseComponent::QueryInterface(const std::string& id, void** ppv)
{
    *ppv = nullptr;

    if(owner == nullptr){
        if(id == IUnknown::iid){
            *ppv = (IUnknown*)this;
            return OK;
        }
        return NO_INTERFACE;
    }else{
        return owner->QueryInterface(id, ppv);
    }
}

}//namespace COM
