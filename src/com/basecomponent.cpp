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

int BaseComponent::release()
{
    if(owner == nullptr){
        auto val = ref.release();
        if(val <= 0){
            delete this;
        }
        return val;
    }else{
        return owner->release();
    }
}

HResult BaseComponent::QueryInterface(const std::string& id, void** ppv)
{
    *ppv = nullptr;

    if(owner == nullptr){
        if(id == IUnknown::iid){
            *ppv = (IUnknown*)this;
            return HResult();
        }
        return HResult(ErrorCode::NO_INTERFACE);
    }else{
        return owner->QueryInterface(id, ppv);
    }
}

}//namespace COM
