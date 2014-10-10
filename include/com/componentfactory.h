#pragma once

#include <com/basecomponent.h>
#include <com/ifactory.h>
#include <unordered_map>

namespace COM {

class ComponentFactory : public BaseComponent, public IFactory
{
public:
    ComponentFactory(){}
    virtual ~ComponentFactory() override{}

    //interface IUnknown
    virtual Error QueryInterface(const std::string&, void** ppv) override;

    //interface IFactory
    virtual bool registerComponent(const std::string& id, std::function<IUnknown*()> f) override;
    virtual bool unregisterComponent(const std::string& id) override;
    virtual IUnknown* create(const std::string& id) override;
    virtual std::vector<std::string> keys()const override;

private:
    typedef std::function<IUnknown*()> creator_type;
    std::unordered_map<std::string, creator_type> table;
};
}//namespace COM
