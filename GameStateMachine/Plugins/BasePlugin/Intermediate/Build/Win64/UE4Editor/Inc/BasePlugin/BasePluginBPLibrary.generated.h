// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef BASEPLUGIN_BasePluginBPLibrary_generated_h
#error "BasePluginBPLibrary.generated.h already included, missing '#pragma once' in BasePluginBPLibrary.h"
#endif
#define BASEPLUGIN_BasePluginBPLibrary_generated_h

#define GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_RPC_WRAPPERS \
 \
	DECLARE_FUNCTION(execBasePluginSampleFunction) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Param); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UBasePluginBPLibrary::BasePluginSampleFunction(Z_Param_Param); \
		P_NATIVE_END; \
	}


#define GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_RPC_WRAPPERS_NO_PURE_DECLS \
 \
	DECLARE_FUNCTION(execBasePluginSampleFunction) \
	{ \
		P_GET_PROPERTY(UFloatProperty,Z_Param_Param); \
		P_FINISH; \
		P_NATIVE_BEGIN; \
		*(float*)Z_Param__Result=UBasePluginBPLibrary::BasePluginSampleFunction(Z_Param_Param); \
		P_NATIVE_END; \
	}


#define GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUBasePluginBPLibrary(); \
	friend struct Z_Construct_UClass_UBasePluginBPLibrary_Statics; \
public: \
	DECLARE_CLASS(UBasePluginBPLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/BasePlugin"), NO_API) \
	DECLARE_SERIALIZER(UBasePluginBPLibrary)


#define GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_INCLASS \
private: \
	static void StaticRegisterNativesUBasePluginBPLibrary(); \
	friend struct Z_Construct_UClass_UBasePluginBPLibrary_Statics; \
public: \
	DECLARE_CLASS(UBasePluginBPLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/BasePlugin"), NO_API) \
	DECLARE_SERIALIZER(UBasePluginBPLibrary)


#define GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_STANDARD_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UBasePluginBPLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UBasePluginBPLibrary) \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UBasePluginBPLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UBasePluginBPLibrary); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UBasePluginBPLibrary(UBasePluginBPLibrary&&); \
	NO_API UBasePluginBPLibrary(const UBasePluginBPLibrary&); \
public:


#define GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UBasePluginBPLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) : Super(ObjectInitializer) { }; \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	NO_API UBasePluginBPLibrary(UBasePluginBPLibrary&&); \
	NO_API UBasePluginBPLibrary(const UBasePluginBPLibrary&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UBasePluginBPLibrary); \
DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UBasePluginBPLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UBasePluginBPLibrary)


#define GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_PRIVATE_PROPERTY_OFFSET
#define GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_25_PROLOG
#define GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_GENERATED_BODY_LEGACY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_PRIVATE_PROPERTY_OFFSET \
	GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_RPC_WRAPPERS \
	GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_INCLASS \
	GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_STANDARD_CONSTRUCTORS \
public: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


#define GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_PRIVATE_PROPERTY_OFFSET \
	GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_RPC_WRAPPERS_NO_PURE_DECLS \
	GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_INCLASS_NO_PURE_DECLS \
	GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h_28_ENHANCED_CONSTRUCTORS \
static_assert(false, "Unknown access specifier for GENERATED_BODY() macro in class BasePluginBPLibrary."); \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> BASEPLUGIN_API UClass* StaticClass<class UBasePluginBPLibrary>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID GameStateMachine_Plugins_BasePlugin_Source_BasePlugin_Public_BasePluginBPLibrary_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
