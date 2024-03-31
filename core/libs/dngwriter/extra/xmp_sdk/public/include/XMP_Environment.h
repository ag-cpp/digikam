#ifndef __XMP_Environment_h__
#define __XMP_Environment_h__ 1

// =================================================================================================
// XMP_Environment.h - Build environment flags for the XMP toolkit.
// ================================================================
//
// This header is just C preprocessor macro definitions to set up the XMP toolkit build environment.
// It must be the first #include in any chain since it might affect things in other #includes.
//
// =================================================================================================

// =================================================================================================
// Copyright 2002 Adobe
// All Rights Reserved.
//
// NOTICE:  Adobe permits you to use, modify, and distribute this file in accordance with the terms
// of the Adobe license agreement accompanying it. If you have received this file from a source other 
// than Adobe, then your use, modification, or distribution of it requires the prior written permission
// of Adobe.
// =================================================================================================

#if AdobePrivate
// =================================================================================================
// Change history
// ==============
//
// Writers:
//  AWL Alan Lillich
//  JKR Jens Krueger
//  IJS Inder Jeet Singh
//  HK  Honey Kansal
//  ADC	Amandeep Chawla
//
// mm/dd/yy who Description of changes, most recent on top.
//
// 02-24-14 ADC 5.6-c001 XMPCommon Framework and XMPCore new APIs ported to Mac Environment.
//
// 01-03-14 HK  5.6-f087 [3688857] Fixing data alignment issues on ARM processor.
//
// 12-19-12 IJS 5.6-f005 Making the importToXMP and exportFromXMP as public Plug-in methods
//
// 05-18-12	JKR	5.4-c005 Add iOS targets, separate iOS specific code behind XMP_iOSBuild preprocessor define.
//
// 05-19-09 AWL 5.0-c031-f039 First part of threading improvements. Force full rebuilds.
//
// 01-23-09 AWL Add a comment just to force full client and library rebuilds for bug 2261961 changes.
//
// 08-24-07 AWL 4.2-c019 Remove support for ancient CXMP_* init/term routines and non-public (AXE) expat.
//
// 04-14-06 AWL 4.0-c003 Undo the previous edits. There seems to be no way on the UNIX platforms to
//				determine the endianness safely. There are no standard macros, we can't force it on
//				the client's build scripts.
// 04-14-06 AWL 4.0-c002 Add XMP_NativeBigEndian macro to XMP_Environment.h, verify the setting at
//				init time in both the client and DLL. Verify that this matches the expat_config.h
//				settings. Change UnicodeConversions to use the macro.
// 03-24-06 AWL 4.0 Adapt for move to ham-perforce, integrate XMPFiles, bump version to 4.
//
// 01-28-05 AWL Remove BIB.
// 01-17-04 AWL Move into new Perforce depot, cosmetic cleanup.
// 09-11-02 AWL Started first draft.
//
// =================================================================================================
#endif // AdobePrivate

// =================================================================================================
// Determine the Platform
// ======================

// One of MAC_ENV, WIN_ENV, UNIX_ENV, WEB_ENV or IOS_ENV must be defined by the client. Since some other code
// requires these to be defined without values, they are only used here to define XMP-specific
// macros with 0 or 1 values.

// ! Tempting though it might be to have a standard macro for big or little endian, there seems to
// ! be no decent way to do that on our own in UNIX. Forcing it on the client isn't acceptable.

#if defined ( MAC_ENV ) && !defined (IOS_ENV)

	#if 0	// ! maybe someday - ! MAC_ENV
		#error "MAC_ENV must be defined so that \"#if MAC_ENV\" is true"
	#endif
	
    #if defined ( WIN_ENV ) || defined ( UNIX_ENV ) || defined ( ANDROID_ENV ) || defined ( WEB_ENV )
        #error "XMP environment error - must define only one of MAC_ENV, WIN_ENV, UNIX_ENV, ANDROID_ENV or WEB_ENV"
    #endif

    #define XMP_MacBuild  1
    #define XMP_WinBuild  0
    #define XMP_UNIXBuild 0
	#define XMP_iOSBuild  0
	#define XMP_UWP	      0
	#define XMP_AndroidBuild  0

#elif defined ( WIN_ENV )

	#if 0	// ! maybe someday - ! WIN_ENV
		#error "WIN_ENV must be defined so that \"#if WIN_ENV\" is true"
	#endif
	
    #if defined ( MAC_ENV ) || defined ( UNIX_ENV ) || defined ( IOS_ENV ) || defined ( ANDROID_ENV ) || defined ( WEB_ENV )
		#error "XMP environment error - must define only one of MAC_ENV (or IOS_ENV), WIN_ENV, UNIX_ENV, ANDROID_ENV or WEB_ENV"
    #endif

    #define XMP_MacBuild  0
    #define XMP_WinBuild  1
    #define XMP_UNIXBuild 0
	#define XMP_iOSBuild  0
	#if defined ( WIN_UNIVERSAL_ENV)
		#define XMP_UWP	  1
	#else
		#define XMP_UWP	  0
	#endif
	#define XMP_AndroidBuild  0

#elif defined ( UNIX_ENV ) && !defined (ANDROID_ENV) && !defined (WEB_ENV)

	#if 0	// ! maybe someday - ! UNIX_ENV
		#error "UNIX_ENV must be defined so that \"#if UNIX_ENV\" is true"
	#endif
	
	#if defined ( MAC_ENV ) || defined ( WIN_ENV ) || defined ( IOS_ENV ) || defined ( ANDROID_ENV ) || defined ( WEB_ENV )
		#error "XMP environment error - must define only one of MAC_ENV (or IOS_ENV), WIN_ENV, UNIX_ENV, ANDROID_ENV or WEB_ENV"
	#endif

    #define XMP_MacBuild  0
    #define XMP_WinBuild  0
    #define XMP_UNIXBuild 1
	#define XMP_iOSBuild  0

	#define XMP_UWP		  0
	#define XMP_AndroidBuild  0

#elif defined ( IOS_ENV )

	#if 0	// ! maybe someday - ! IOS_ENV
		#error "IOS_ENV must be defined so that \"#if IOS_ENV\" is true"
	#endif

	#if defined ( WIN_ENV ) || defined ( UNIX_ENV ) || defined ( ANDROID_ENV ) || defined ( WEB_ENV )
		#error "XMP environment error - must define only one of IOS_ENV, WIN_ENV, UNIX_ENV, ANDROID_ENV or WEB_ENV"
	#endif

	#define XMP_MacBuild  0
	#define XMP_WinBuild  0
	#define XMP_UNIXBuild 0
	#define XMP_iOSBuild  1

	#define XMP_UWP		  0

	#define XMP_AndroidBuild  0

#elif defined ( ANDROID_ENV )

	#if 0	// ! maybe someday - ! UNIX_ENV
		#error "ANDROID_ENV must be defined so that \"#if ANDROID_ENV\" is true"
	#endif
	
	#if defined ( MAC_ENV ) || defined ( WIN_ENV ) || defined ( IOS_ENV ) || defined ( WEB_ENV )
		#error "XMP environment error - must define only one of MAC_ENV (or IOS_ENV), WIN_ENV, UNIX_ENV, ANDROID_ENV or WEB_ENV"
	#endif

    #define XMP_MacBuild  0
    #define XMP_WinBuild  0
    #define XMP_UNIXBuild 0
	#define XMP_iOSBuild  0
	#define XMP_AndroidBuild  1

#elif defined ( WEB_ENV )

    #if 0    // ! maybe someday - ! WEB_ENV
        #error "WEB_ENV must be defined so that \"#if WEB_ENV\" is true"
    #endif
    
	#if defined ( MAC_ENV ) || defined ( WIN_ENV ) || defined ( IOS_ENV ) || defined ( ANDROID_ENV )
    	#error "XMP environment error - must define only one of MAC_ENV (or IOS_ENV), WIN_ENV, UNIX_ENV, ANDROID_ENV or WEB_ENV"
	#endif

    #define XMP_MacBuild  0
    #define XMP_WinBuild  0
    #define XMP_UNIXBuild 1
    #define XMP_iOSBuild  0
    #define XMP_UWP          0
    #define XMP_AndroidBuild  0

#else

    #error "XMP environment error - must define one of MAC_ENV, WIN_ENV, UNIX_ENV , ANDROID_ENV, WEB_ENV or IOS_ENV"

#endif

// =================================================================================================
// Common Macros
// =============

#if defined ( DEBUG )
    #if defined ( NDEBUG )
        #error "XMP environment error - both DEBUG and NDEBUG are defined"
    #endif
    #define XMP_DebugBuild 1
#endif

#if defined ( NDEBUG )
    #define XMP_DebugBuild 0
#endif

#ifndef XMP_DebugBuild
    #define XMP_DebugBuild 0
#endif

#if XMP_DebugBuild
    #include <stdio.h>  // The assert macro needs printf.
#endif

#ifndef AdobePrivate
	#define AdobePrivate 1
#endif

#if AdobePrivate
    #ifdef XMP_PRESERVE_BIB_CLIENT
        #error "Use of XMP_PRESERVE_BIB_CLIENT is no longer supported"
    #endif
    #ifdef UsePublicExpat
		#error "Use of a non-public Expat (like AXE) is no longer supported"
    #endif
#endif


#ifndef DISABLE_SERIALIZED_IMPORT_EXPORT 
	#define DISABLE_SERIALIZED_IMPORT_EXPORT 0
#endif

#ifndef XMP_64
	#if _WIN64 || defined(_LP64)
		#define XMP_64 1
	#else
		#define XMP_64 0
	#endif
#endif

// =================================================================================================
// Macintosh Specific Settings
// ===========================
#if (XMP_MacBuild)
	#define XMP_HELPER_DLL_IMPORT __attribute__((visibility("default")))
	#define XMP_HELPER_DLL_EXPORT __attribute__((visibility("default")))
	#define XMP_HELPER_DLL_PRIVATE __attribute__((visibility("hidden")))
	#define APICALL 
#endif

// =================================================================================================
// Windows Specific Settings
// =========================
#if (XMP_WinBuild)
	#define XMP_HELPER_DLL_IMPORT
	#define XMP_HELPER_DLL_EXPORT
	#define XMP_HELPER_DLL_PRIVATE
	#define APICALL __stdcall
#endif

// =================================================================================================
// UNIX Specific Settings
// ======================
#if (XMP_UNIXBuild) | (XMP_AndroidBuild)
	#define XMP_HELPER_DLL_IMPORT
	#define XMP_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
	#define XMP_HELPER_DLL_PRIVATE __attribute__ ((visibility ("hidden")))
	#define APICALL 
#endif

// =================================================================================================
// IOS Specific Settings
// ===========================
#if (XMP_iOSBuild)
	#include <TargetConditionals.h>
	#if (TARGET_CPU_ARM)
		#define XMP_IOS_ARM 1
	#else
		#define XMP_IOS_ARM 0
	#endif
	#define XMP_HELPER_DLL_IMPORT __attribute__((visibility("default")))
	#define XMP_HELPER_DLL_EXPORT __attribute__((visibility("default")))
	#define XMP_HELPER_DLL_PRIVATE __attribute__((visibility("hidden")))
	#define APICALL 
#endif

// =================================================================================================

#if (XMP_DynamicBuild)
	#define XMP_PUBLIC XMP_HELPER_DLL_EXPORT
	#define XMP_PRIVATE XMP_HELPER_DLL_PRIVATE
#elif (XMP_StaticBuild)
	#define XMP_PUBLIC
	#define XMP_PRIVATE
#else
	#define XMP_PUBLIC XMP_HELPER_DLL_IMPORT
	#define XMP_PRIVATE XMP_HELPER_DLL_PRIVATE
#endif

#endif  // __XMP_Environment_h__
