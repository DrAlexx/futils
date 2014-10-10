#pragma once

#include <com/iunknown.h>
#include <functional>
#include <vector>

namespace COM{

class IFactory : public IUnknown
{
public:
    virtual HResult registerComponent(const std::string& id, std::function<IUnknown*()> f) =0;
    virtual HResult unregisterComponent(const std::string& id) =0;
    virtual IUnknown* create(const std::string& id) =0;
    virtual std::vector<std::string> keys()const =0;

    static std::string iid;
};
}//namespace COM

