//=======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2021 - 2024 James Steele. All Rights Reserved.
//=======================================================================================================================

#ifndef RttiType_h
#define RttiType_h

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <typeinfo>
#include "mathcc/Math3d.h"

enum RttiPropType {
    RttiPropType_Nil = 0,
    RttiPropType_Int,
    RttiPropType_Float,
    RttiPropType_Vec3,
    RttiPropType_Vec4,
    RttiPropType_Quat,
    RttiPropType_String,
    RttiPropType_Object,
    RttiPropType_Count,
};

struct RttiProp {
    RttiPropType        type;
    const char *        name;
    uintptr_t           offset;
    uint64_t            isVector : 1;
    
    typedef std::vector<RttiProp> vector;
    typedef std::map<uint64_t, uint32_t> map;
};

class RttiObject;
typedef RttiObject * (*RttiClassFactory)();

class RttiType {
public:
    RttiType( const char * name_, RttiType * super_, RttiClassFactory factory_, const RttiProp * initProps_ );
    
    ~RttiType();
    
    RttiObject * Create() { return factory(); }
    
    const char * GetName() const { return name; }
    
    RttiType * GetSuper() { return super; }
    
    bool IsOfType( const RttiType * theType ) const;
    
    const RttiType * GetSuper() const { return super; }
    
    const RttiProp * FindProp( const char * name ) const;
    
    static void Initialise();
    
    static void Finalise();
    
    static RttiType * Find( const char * name );
    
    
protected:
    
    static void PublishProps( RttiType * type );
    
protected:
    typedef std::vector<RttiType* > vector;
    typedef std::map<uint64_t, uint32_t> map;
    
    RttiType *          super = nullptr;
    const char *        name = nullptr;
    const RttiProp *    initProps = nullptr;
    RttiProp::vector    props;
    RttiProp::map       propsMap;
    RttiType *          next = nullptr;
    RttiClassFactory    factory = nullptr;
    
    static RttiType *   initListHead;
    static RttiType *   initListTail;
    static vector       typeArray;
    static map          typeMap;

};

template<typename T> inline RttiPropType RttiProp_GetType() {}
template<typename T> inline bool RttiProp_IsVector() {}

#define RTTI_PROP_FUNCS( type, e )\
template<> inline RttiPropType RttiProp_GetType<type>() { return e; }\
template<> inline bool RttiProp_IsVector<type>() { return false; }\
template<> inline RttiPropType RttiProp_GetType<std::vector<type>>() { return e; }\
template<> inline bool RttiProp_IsVector<std::vector<type>>() { return true; }

RTTI_PROP_FUNCS( int32_t, RttiPropType_Int )
RTTI_PROP_FUNCS( float, RttiPropType_Float )
RTTI_PROP_FUNCS( math::Vec3, RttiPropType_Vec3 )
RTTI_PROP_FUNCS( math::Vec4, RttiPropType_Vec4 )
RTTI_PROP_FUNCS( math::Quat, RttiPropType_Quat )
RTTI_PROP_FUNCS( std::string, RttiPropType_String )
RTTI_PROP_FUNCS( RttiObject * , RttiPropType_Object )

template<typename type>
inline constexpr RttiProp RttiMakeProp( const char * name, uintptr_t offset ) {
    RttiProp p;
    p.type = RttiProp_GetType<type>();
    p.name = name;
    p.offset = offset;
    p.isVector = (RttiProp_IsVector<type>() == true ) ? 1 : 0;
    return p;
}

#define RTTI_PROP_OBJECT( T ) RTTI_PROP_FUNCS( T * , RttiPropType_Object )

#define RTTI_OFFSETOF( M, S) (uintptr_t) (&( ( M* )( 0x0 ) )->S ))

#define RTTI_PROP( M ) RttiMakeProp< decltype( M ) >( &M, RTTI_OFFSETOF( ThisClassType, M ),

#define RTTI_CLASS( CLASS, SUPER )\
    public: typedef SUPER Super;\
    static RttiObject * Factory() { return new CLASS; }\
    static RttiType TYPE;\
    static RttiType * GetClassType() { return &TYPE; }\
    virtual RttiType * GetType() { return &TYPE; }\
    static const RttiProp INIT_PROPS[];

#define RTTI_CLASS_BEGIN_NAMED( CLASS, NAME )\
    typedef CLASS ThisClass;\
    static const char * ThisClassName = NAME;\
    const RttiProp CLASS::INIT_PROPS[] = {

#define RTTI_CLASS_BEGIN( CLASS) RTTI_CLASS_BEGIN_NAMED( CLASS, #CLASS )

#define RTTI_CLASS_END(...)\
    { RttiPropType_Nil } };\
    RttiType ThisClass::TYPE( ThisClassName, ThisClass::Super::GetClassType(), ThisClass::Factory, ThisClass::INIT_PROPS );


#endif
