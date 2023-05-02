//======================================================================================================================
// CONFIDENTIAL AND PROPRIETARY INFORMATION / NOT FOR DISCLOSURE WITHOUT WRITTEN PERMISSION
// Copyright (C) 2022 James Steele. All Rights Reserved.
//======================================================================================================================


#include "mathcc/Math3d.h"

namespace math {

	const Vec3 Vec3::ZERO(0, 0, 0);
	const Vec3 Vec3::UNIT_X(1, 0, 0);
	const Vec3 Vec3::UNIT_Y(0, 1, 0);
	const Vec3 Vec3::UNIT_Z(0, 0, 1);

	const Vec4 Vec4::ZERO(0, 0, 0, 0);
	const Vec4 Vec4::UNIT_X(1, 0, 0, 0);
	const Vec4 Vec4::UNIT_Y(0, 1, 0, 0);
	const Vec4 Vec4::UNIT_Z(0, 0, 1, 0);
	const Vec4 Vec4::UNIT_W(0, 0, 0, 1);

	const Quat Quat::IDENTITY(0, 0, 0, 1);

	const Mat3 Mat3::IDENTITY(1, 1, 1);
	const Mat4 Mat4::IDENTITY(1, 1, 1, 1);
}
