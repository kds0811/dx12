#include "Quat.h"
#include <algorithm>
#include "Rotator.h"
#include "Vector.h"
#include "EulerAngles.h"


bool Quat::operator==(const Quat& other) const noexcept
{
    return QuatsIsEqual(ToSIMD(), other.ToSIMD());
}

bool Quat::operator!=(const Quat& other) const noexcept
{
    return DirectX::XMQuaternionNotEqual(ToSIMD(), other.ToSIMD());
}

bool Quat::IsNearEqual(const Quat& other, float epsilon) const noexcept
{
    return QuatsIsNearEqual(ToSIMD(), other.ToSIMD(), epsilon);
}

Rotator Quat::ToRotator() const noexcept
{
    return Rotator();
}

DirectX::XMVECTOR Quat::GetNormalizeSIMD() const noexcept
{
    return DirectX::XMQuaternionNormalize(ToSIMD());
}

Quat Quat::GetNormalize() const noexcept
{
    return Quat(GetNormalizeSIMD());
}

DirectX::XMVECTOR Quat::GetNormalizeEstSIMD() const noexcept
{
    return DirectX::XMQuaternionNormalizeEst(ToSIMD());
}

Quat Quat::GetNormalizeEst() const noexcept
{
    return Quat(GetNormalizeEstSIMD());
}

DirectX::XMVECTOR Quat::MutiplySIMD(DirectX::FXMVECTOR otherQuat) const noexcept
{
    return MultiplyTwoQuatsSIMD(ToSIMD(), otherQuat);
}

Quat Quat::Mutiply(const Quat& otherQuat) const noexcept
{
    return Quat(MultiplyTwoQuatsSIMD(ToSIMD(), otherQuat.ToSIMD()));
}

DirectX::XMVECTOR Quat::InverseSIMD() const noexcept
{
    return InverseQuatSIMD(ToSIMD());
}

Quat Quat::Inverse() const noexcept
{
    return Quat(InverseQuatSIMD(ToSIMD()));
}


DirectX::XMVECTOR Quat::MultiplyTwoQuatsSIMD(DirectX::FXMVECTOR quat1, DirectX::FXMVECTOR quat2) noexcept
{
    return DirectX::XMQuaternionMultiply(quat1, quat2);
}

Quat Quat::MultiplyTwoQuats(const Quat& quat1, const Quat& quat2) noexcept
{
    return Quat(MultiplyTwoQuatsSIMD(quat1.ToSIMD(), quat2.ToSIMD()));
}

DirectX::XMVECTOR Quat::InverseQuatSIMD(DirectX::FXMVECTOR quat) noexcept
{
    return DirectX::XMQuaternionInverse(quat);
}

Quat Quat::InverseQuat(const Quat& quat1) noexcept
{
    return Quat(InverseQuatSIMD(quat1.ToSIMD()));
}

DirectX::XMVECTOR Quat::SlerpQuatsSIMD(DirectX::FXMVECTOR quat1, DirectX::FXMVECTOR quat2, float t) noexcept
{
    return DirectX::XMQuaternionSlerp(quat1, quat2, t);
}

Quat Quat::SlerpQuats(const Quat& quat1, const Quat& quat2, float t) noexcept
{
    return Quat(SlerpQuatsSIMD(quat1.ToSIMD(), quat2.ToSIMD(), t));
}

Rotator Quat::QuatToRotator(DirectX::FXMVECTOR quaternion) noexcept
{
    auto NormQuat = DirectX::XMQuaternionNormalize(quaternion);
    Quat1 qt;
    qt.w = DirectX::XMVectorGetW(quaternion);
    qt.x = DirectX::XMVectorGetX(quaternion);
    qt.y = DirectX::XMVectorGetY(quaternion);
    qt.z = DirectX::XMVectorGetZ(quaternion);

    const float RAD_TO_DEG = 180.0f / DirectX::XM_PI;

    auto eul = Eul_FromQuat1(qt, EulOrdYXZr);

    return Rotator(eul.y * RAD_TO_DEG, eul.x * RAD_TO_DEG, eul.z * RAD_TO_DEG);
}


Rotator Quat::QuatToRotator(const Quat& quat1) noexcept
{
    return Rotator(QuatToRotator(quat1.ToSIMD()));
}

DirectX::FXMVECTOR Quat::RotatorToQuatSIMD(const Rotator& rot) noexcept
{
    return DirectX::XMQuaternionRotationRollPitchYaw(
        DirectX::XMConvertToRadians(rot.GetPitch()), DirectX::XMConvertToRadians(rot.GetYaw()), DirectX::XMConvertToRadians(rot.GetRoll()));
}


Quat Quat::RotatorToQuat(const Rotator& rot) noexcept
{
    return Quat(RotatorToQuatSIMD(rot));
}

DirectX::XMVECTOR Quat::MatrixToQuatSIMD(DirectX::FXMMATRIX matrix) noexcept
{
    return DirectX::XMQuaternionRotationMatrix(matrix);
}

Quat Quat::MatrixToQuat(DirectX::FXMMATRIX matrix) noexcept
{
    return Quat(DirectX::XMQuaternionRotationMatrix(matrix));
}

DirectX::XMMATRIX Quat::QuatToMatrix(DirectX::FXMVECTOR quat) noexcept
{
    return DirectX::XMMatrixRotationQuaternion(quat);
}

DirectX::XMMATRIX Quat::QuatToMatrix(const Quat& quat) noexcept
{
    return QuatToMatrix(quat.ToSIMD());
}

bool Quat::QuatsIsEqual(DirectX::FXMVECTOR quat1, DirectX::FXMVECTOR quat2) noexcept
{
    return DirectX::XMQuaternionEqual(quat1, quat2);
}

bool Quat::QuatsIsEqual(const Quat& quat1, const Quat& quat2) noexcept
{
    return QuatsIsEqual(quat1.ToSIMD(), quat2.ToSIMD());
}

bool Quat::QuatsIsNearEqual(DirectX::FXMVECTOR quat1, DirectX::FXMVECTOR quat2, float epsilon) noexcept
{
    DirectX::XMVECTOR normQuat1 = DirectX::XMQuaternionNormalize(quat1);
    DirectX::XMVECTOR normQuat2 = DirectX::XMQuaternionNormalize(quat2);

    float dot = DirectX::XMVectorGetX(DirectX::XMQuaternionDot(normQuat1, normQuat2));

    return (std::abs(dot) >= 1.0f - epsilon);
}

bool Quat::QuatsIsNearEqual(const Quat& quat1, const Quat& quat2, float epsilon) noexcept
{
    return QuatsIsNearEqual(quat1.ToSIMD(), quat2.ToSIMD(), epsilon);
}

Vector Quat::RotateVector(const Vector& Vec, DirectX::FXMVECTOR quat) noexcept
{
    return Vector(DirectX::XMVector3Rotate(Vec.ToSIMD(), quat));
}

Vector Quat::RotateVector(const Vector& Vec, const Quat& quat) noexcept
{
    return Vector(RotateVector(Vec, quat.ToSIMD()));
}

