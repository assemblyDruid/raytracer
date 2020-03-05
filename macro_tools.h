#ifndef __RT_MACRO_TOOLS_H___
#define __RT_MACRO_TOOLS_H___

#ifndef __RT_ON__
#define __RT_ON__ 1
#endif // ___RT_ON___

#ifndef __RT_OFF__
#define __RT_OFF__ 0
#endif // ___RT_OFF___

#ifndef _mut_
#define _mut_ /* NOTICE: MUTABLE */
#endif // _mut_

// Decorators
#define __RT_call__ /* STDCALL */

// Passifiers
#ifdef _WIN32
#define restrict __restrict
#endif // WIN32

#endif // __RT_MACRO_TOOLS_H___
