#ifndef __VENDOR_H__
#define __VENDOR_H__

typedef int (*pfn_vendor_func0_int)();
typedef void (*pfn_vendor_func0_void)();
typedef int (*pfn_vendor_func1_int_int)(int);

#define CALL_AND_RETURN_INT(x,y) {return gpfn_##x(y);}
#define CALL_AND_RETURN_VOID(x,y) {gpfn_##x(y); return 0;}

#define VENDOR_FUNC(x,y,z,func_params, call_params) \
		static y gpfn_##x = 0; \
		int vendor_##x(func_params) { \
			if (make_sure_library_ok()) \
				return -1; \
			if (make_sure_functions_ok()) \
				return -1; \
			CALL_AND_RETURN_##z(x,call_params); \
		}

#define VENDOR_FUNC0_INT(x) VENDOR_FUNC(x, pfn_vendor_func0_int, INT, void,)

#define VENDOR_FUNC0_VOID(x) VENDOR_FUNC(x, pfn_vendor_func0_void, VOID, void,)

#define VENDOR_FUNC1_INT_INT(x) VENDOR_FUNC(x, pfn_vendor_func1_int_int, INT, int vendor_param_int1,vendor_param_int1)

#define VENDOR_FUNC_SYM_ENTRY(x,y) \
		if (!gpfn_##x) {\
			dlerror();\
			gpfn_##x = (y) dlsym(\
					(void *) g_hardware_legacy_handler, #x);\
			if (!gpfn_##x) {\
				LOGE("unable to find function:%s,%s\n", #x, dlerror());\
				return -2;\
			}\
		}
#define VENDOR_FUNC0_INT_SYM_ENTRY(x) VENDOR_FUNC_SYM_ENTRY(x,pfn_vendor_func0_int)
#define VENDOR_FUNC0_VOID_SYM_ENTRY(x) VENDOR_FUNC_SYM_ENTRY(x,pfn_vendor_func0_void)
#define VENDOR_FUNC1_INT_INT_SYM_ENTRY(x) VENDOR_FUNC_SYM_ENTRY(x,pfn_vendor_func1_int_int)

extern void * g_hardware_legacy_handler;
int make_sure_library_ok();

#endif
