#include <com/componentfactory.h>

namespace COM {

//interface IUnknown
HResult ComponentFactory::QueryInterface(const std::string& id, void** ppv){
    if(id == IFactory::iid){
        *ppv = (IFactory*)this;
        return HResult();
    }

    return BaseComponent::QueryInterface(id, ppv);
}

//interface IFactory
HResult ComponentFactory::registerComponent(const std::string& id, std::function<IUnknown*()> f){
    auto it = table.find(id);
    if(it != table.end())
        return HResult(0);
    table.emplace(id, f);
    return HResult();
}

HResult ComponentFactory::unregisterComponent(const std::string& id){
    return table.erase(id) == 0 ? HResult(0) : HResult();
}

IUnknown* ComponentFactory::create(const std::string& id){
    auto it = table.find(id);
    if(it == table.end())
        return nullptr;
    return (*it).second();
}

std::vector<std::string> ComponentFactory::keys()const{
    std::vector<std::string> keys;
    keys.reserve(table.size());
    for(auto i : table){
        keys.emplace_back(i.first);
    }

    return keys;
}
}//namespace COM
