#include <componentfactory.h>


#ifdef __cplusplus
#  define EXTERN_C extern "C"
#else
#  define EXTERN_C extern
#endif


Q_EXTERN_C Q_DECL_EXPORT  Q_STANDARD_CALL qt_plugin_instance() \
Q_PLUGIN_INSTANCE(PLUGINCLASS)

