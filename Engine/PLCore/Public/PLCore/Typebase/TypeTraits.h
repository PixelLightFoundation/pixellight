/*********************************************************\
 *  File: PLCore.h                                       *
 *
 *  Copyright (C) 2002-2013 The PixelLight Team (http://www.pixellight.org/)
 *
 *  This file is part of PixelLight.
 *
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 *  and associated documentation files (the "Software"), to deal in the Software without
 *  restriction, including without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
 *  Software is furnished to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included in all copies or
 *  substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 *  BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 *  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
\*********************************************************/

#include "CompileError.h"

#ifndef __PLCORE_TYPETRAITS2_H__
#define __PLCORE_TYPETRAITS2_H__
#pragma once


//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
namespace PLCore {


//[-------------------------------------------------------]
//[ Types                                                 ]
//[-------------------------------------------------------]
/**
*  @brief
*    Type that is not used
*/
class NullType {
};

/**
*  @brief
*    Type that is considered invalid (unknown type)
*/
template <typename T>
class InvalidType {
};


//[-------------------------------------------------------]
//[ Helper classes                                        ]
//[-------------------------------------------------------]
/**
*  @brief
*    Class representing 'true' in templates
*/
class TrueType {
};

/**
*  @brief
*    Class representing 'false' in templates
*/
class FalseType {
};

/**
*  @brief
*    Conversion helper class
*/
struct ConversionHelper
{
	template <typename T> ConversionHelper(const volatile T&);
	template <typename T> ConversionHelper(T&);
};


//[-------------------------------------------------------]
//[ Generic templates                                     ]
//[-------------------------------------------------------]
/**
*  @brief
*    Template to specialize another template only if two types are equal
*
*  @param[in] T1
*    First compared type
*  @param[in] T2
*    Second compared type
*  @param[in] TMPL
*    Class template
*  @param[in] TYPE
*    Template argument for class template
*/
// Implementation for T1 != T2
template <typename T1, typename T2, template <typename T> class TMPL, typename TYPE>
class SpecializeIfEqual {
	public:
		typedef FalseType Type;
};

// Implementation for T1 == T2
template <typename T1, template <typename T> class TMPL, typename TYPE>
class SpecializeIfEqual<T1, T1, TMPL, TYPE> {
	public:
		typedef TMPL<TYPE> Type;
};

/**
*  @brief
*    Choose type based on condition
*
*  @param[in] CHECK
*    Condition
*  @param[in] TYPE
*    Type to return if condition is true
*  @param[in] REST
*    ChooseType that contains the rest of the decision if condition is false
*/
// Generic
template <bool CHECK, typename TYPE, typename REST>
struct ChooseType {
};

// Implementation for CHECK == true
template <typename TYPE, typename REST>
struct ChooseType<true, TYPE, REST> {
	typedef TYPE Type;
};

// Implementation for CHECK == false
template <typename TYPE, typename REST>
struct ChooseType<false, TYPE, REST> {
	typedef typename REST::Type Type;
};

/**
*  @brief
*    Boolean operation: OR
*
*  @param[in] T1
*    First class
*  @param[in] T2
*    Second class
*
*  @remarks
*    The value of Value will contain the result of (T1::Value || T2::Value)
*/
template <typename T1, typename T2>
struct BooleanOr {
	static const bool Value = (T1::Value || T2::Value);
};

/**
*  @brief
*    Boolean operation: AND
*
*  @param[in] T1
*    First class
*  @param[in] T2
*    Second class
*
*  @remarks
*    The value of Value will contain the result of (T1::Value && T2::Value)
*/
template <typename T1, typename T2>
struct BooleanAnd {
	static const bool Value = (T1::Value && T2::Value);
};

/**
*  @brief
*    Boolean operation: NOT
*
*  @param[in] T
*    Class
*
*  @remarks
*    The value of Value will contain the result of (!T::Value)
*/
template <typename T>
struct BooleanNot {
	static const bool Value = (!T::Value);
};


//[-------------------------------------------------------]
//[ Type traits                                           ]
//[-------------------------------------------------------]
/**
*  @brief
*    Class that recognizes integral types
*
*  @remarks
*    This class can be used to check if a given data type is an integer-type.
*    Examples:
*    - IntegralType<long> ::IsIntegral => 1
*    - IntegralType<long> ::Type       => long
*    - IntegralType<float>::IsIntegral => 0
*    - IntegralType<float>::Type       => NullType
*/
template <typename T>
class IntegralType {
	public:
		enum { IsIntegral = 0 };
		typedef NullType Type;
};

template <>
class IntegralType<uint8> {
	public:
		enum { IsIntegral = 1 };
		typedef uint8 Type;
};

template <>
class IntegralType<uint16> {
	public:
		enum { IsIntegral = 1 };
		typedef uint16 Type;
};

template <>
class IntegralType<uint32> {
	public:
		enum { IsIntegral = 1 };
		typedef uint32 Type;
};

template <>
class IntegralType<int8> {
	public:
		enum { IsIntegral = 1 };
		typedef int8 Type;
};

template <>
class IntegralType<int16> {
	public:
		enum { IsIntegral = 1 };
		typedef int16 Type;
};

template <>
class IntegralType<int32> {
	public:
		enum { IsIntegral = 1 };
		typedef int32 Type;
};

/**
*  @brief
*    Check if two types are equal
*
*  @param[in] T1
*    First type
*  @param[in] T2
*    Second type
*
*  @remarks
*    If (From) and (To) are equal, the value of Value is 'true', else 'false'
*/
// Implementation for T1 != T2
template<typename T1, typename T2>
struct IsEqual
{
	static const bool Value = false;
};

// Implementation for T1 == T2
template<typename T>
struct IsEqual<T, T>
{
	static const bool Value = true;
};


/**
*  @brief
*    Check if two types are convertible
*
*  @param[in] From
*    First type
*  @param[in] To
*    Second type
*
*  @remarks
*    If (From) can be converted to (To), the value of Value is 'true', else 'false'
*/
template<typename From, typename To>
struct IsConvertible
{
	private:
		typedef char                          one_byte;
		typedef struct { char two_chars[2]; } two_bytes;

	private:
		static one_byte  test(const To&, int);
		static two_bytes test(ConversionHelper, ...);

	public:
		static const bool Value = sizeof(test(*(static_cast<From*>(nullptr)), 0)) == 1;


};

/**
*  @brief
*    Check if one type is a base type of another
*
*  @param[in] Class
*    First type
*  @param[in] Base
*    Second type
*
*  @remarks
*    If Base is a base class of Class (and Base and Class are NOT equal), the value of Value is 'true', else 'false'
*/
template<typename Class, typename Base>
struct IsBaseClass
{
	static const bool Value = BooleanAnd< IsConvertible<Class, Base>, BooleanNot< IsEqual<Class, Base> > >::Value;
};

/**
*  @brief
*    Check if a type is an enum
*
*  @param[in] T
*    Type
*
*  @remarks
*    If T is an enum type, the value of Value is 'true', else 'false'
*/
template<typename T>
struct IsEnum
{
	static const bool Value = IsConvertible<T, int>::Value;
};

template<typename T>
struct IsEnum< InvalidType<T> >
{
	static const bool Value = false;
};

/**
*  @brief
*    Classify a type (check if it is a class, an enum, etc.)
*
*  @param[in] T
*    Type
*/
template <typename T>
struct TypeClassification {
	static const bool Value = IsEnum<T>::Value;	/**< 'true' if T is an enum */
};

/**
*  @brief
*    Get representative type for non-default types
*
*  @param[in] T
*    Type
*
*  @remarks
*    Type will be defined as:
*    - int             (if T is an enum)
*    - InvalidType<T>  (otherwise)
*/
//template <typename T>
//struct CheckType {
//	typedef typename ChooseType< TypeClassification<T>::Value, PLCore::EnumTypePlain<T>,
//						ChooseType< true, InvalidType<T>, InvalidType<T> >
//					 >::Type Type;
//};

/**
*  @brief
*    Check if a class is derived from another class and issue an error if not
*
*  @param[in] Class
*    Class
*  @param[in] Base
*    Base class
*
*  @remarks
*    Will issue a compiler error if Class is not derived from Base
*/
// Implementation for 'true' (no error)
template <bool Condition, typename Class, typename Base>
struct CheckBaseClassBool {
	// No error
	static void Error() {
	}
};

// Implementation for 'false' (no error)
template <typename Class, typename Base>
struct CheckBaseClassBool<false, Class, Base> {
	// Create error
	static void Error() {
		PLCORE_ERROR(Not_A_Base_Class)
	}
};

// Generic implementation
template <typename Class, typename Base>
struct CheckBaseClass {
	typedef CheckBaseClassBool< IsBaseClass<Class, Base>::Value, Class, Base > Type;
};

/**
*  @brief
*    Member function type decomposer
*/
template <typename T>
struct MemberFunctionDecomp;

template <class TClass, typename TRet, typename... TArgs>
struct MemberFunctionDecomp<TRet(TClass::*)(TArgs...)> {

	//[-------------------------------------------------------]
	//[ Public types                                          ]
	//[-------------------------------------------------------]
	typedef TClass _Class;
	typedef TRet _ReturnType;
};

/**
*  @brief
*    Remove reference from a type
*/
template <typename T>
struct RemoveReference {

	typedef T Type;
};

template <typename T>
struct RemoveReference<T&> {

	typedef T Type;
};

template <typename T>
struct RemoveReference<T&&> {

	typedef T Type;
};

/**
*  @brief
*    Remove const from a type
*/
template <typename T>
struct RemoveConst {

	typedef T Type;
};

template <typename T>
struct RemoveConst<const T> {

	typedef T Type;
};

/**
*  @brief
*    This template will remove all type qualifiers (const, pointers, references)
*/
template <typename T>
struct RawType
{
	// [TODO] use the above Remove* instead of specialization
	typedef T Type;
};

template <typename T>
struct RawType<const T>
{
	typedef typename RawType<T>::Type Type;
};

template <typename T>
struct RawType<T&>
{
	typedef typename RawType<T>::Type Type;
};

template <typename T>
struct RawType<T*>
{
	typedef typename RawType<T>::Type Type;
};

//[-------------------------------------------------------]
//[ Namespace                                             ]
//[-------------------------------------------------------]
} // PLCore


#endif // __PLCORE_TYPETRAITS2_H__
