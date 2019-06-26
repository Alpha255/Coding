#pragma once

#define _META_KEYWORD "reflect"
#define _META_EXCLUDE_KEYWORD "reflect-exclude"

#ifdef REFLECT
// non-intrusive meta data

#define Meta(...) __attribute__((annotate(_META_KEYWORD "," #__VA_ARGS__)))
#define MetaExclude()  __attribute__((annotate(_META_EXCLUDE_KEYWORD)))
#ifdef MetaInjector
#define MetaInject(...) MetaInjector(__VA_ARGS__)
#else
#define MetaInject(...)
#endif
#else
#define Meta(...)
#define MetaExclude(...)
#define MetaInject(...)
#endif