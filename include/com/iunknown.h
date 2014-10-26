#pragma once

#include <com/hresult.h>
#include <string>

namespace COM{

class IUnknown{
public:
    virtual ~IUnknown(){}

    virtual int addRef()  = 0;
    virtual int release() = 0;
    virtual HResult QueryInterface(const std::string&, void** ppv) = 0;

    static std::string iid;
};
}//namespace COM

namespace boost {

inline void intrusive_ptr_add_ref(COM::IUnknown* i){
    i->addRef();
}

inline void intrusive_ptr_release(COM::IUnknown* i){
    i->release();
}
}

#include <boost/intrusive_ptr.hpp>

namespace COM{
    typedef boost::intrusive_ptr<IUnknown> IUnknownPtr;
}
