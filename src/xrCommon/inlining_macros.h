#pragma once

// #define						definition															  ...

#define IC						inline				                                                  //--' Новые дефайны для сокращения функций
#define EI						extern int			                                                  //--' Новые дефайны для сокращения функций
#define ECI						extern const int			                                          //--' Новые дефайны для сокращения функций
#define EB						extern BOOL			                                                  //--' Новые дефайны для сокращения функций
#define EC						extern 				                                                  //--' Новые дефайны для сокращения функций
#define EF						extern float			                                              //--' Новые дефайны для сокращения функций
#define ECF						extern const float			                                          //--' Новые дефайны для сокращения функций
#define VBI						virtual bool		                                                  //--' Новые дефайны для сокращения функций
#define ft						float															      //--' Новые дефайны для сокращения функций
#define spectr					auto																  //--' Новые дефайны для сокращения функций
#define _el						else                                                                  //--' Новые дефайны для сокращения функций
#define LUA						luabind::functor<LPCSTR>											  //--' Новые дефайны для сокращения функций
#define _cd						__cdecl																  //--' Новые дефайны для сокращения функций
#define GetU32Int				extern u32															  //--' Новые дефайны для сокращения функций
#define xr_new					new																	  //--' Новые дефайны для сокращения функций
#define IGObj					IGameObject															  //--' Новые дефайны для сокращения функций

#define NO_INLINE				__declspec(noinline)												  //--' Системный дефайн
#define FORCE_INLINE			__forceinline														  //--' Системный дефайн
#define ALIGN(a)				__declspec(align(a))                                                  //--' Системный дефайн
#define DEBUG_BREAK				__debugbreak()														  //--' Системный дефайн
#define __thread				__declspec(thread)													  //--' Системный дефайн
#define ICF						FORCE_INLINE														  //--' Системный дефайн
#define ICN						NO_INLINE															  //--' Системный дефайн


