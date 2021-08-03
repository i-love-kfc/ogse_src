#include "EffectorCam.h"
//#include "xr_3da.h"
#include "RenderDevice.h"

void CEffectorZoomInertion::CalcNextPoint()
{
	float half_disp_radius = m_fDispRadius/2.f;
	m_vTargetPoint.x = m_Random.randF(-half_disp_radius,half_disp_radius);
	m_vTargetPoint.y = m_Random.randF(-half_disp_radius,half_disp_radius);

	m_vTargetVel.sub(m_vTargetPoint, m_vLastPoint);
};
BOOL CEffectorZoomInertion::Process(_vector3<float> &p, _vector3<float> &d, _vector3<float> &n, float& fFov, float& fFar, float& fAspect)
{
	bool camera_moved = false;

	//���������� ������ �� �������� �����
	if(!d.similar(m_vOldCameraDir, m_fCameraMoveEpsilon))
		camera_moved = true;


	if (m_dwTimePassed == 0) // ���������� ����� ����, ������, ���, ����� ��� ����� �����?
	{
		m_vLastPoint.set(m_vCurrentPoint);
		CalcNextPoint();
	}
	else
	{
		// ��� ������ ������� ������� ����� �� m_dwDeltaTime �� ���������� m_dwDeltaTime
		while (m_dwTimePassed > m_dwDeltaTime)
		{
			m_dwTimePassed -= m_dwDeltaTime;
			// ��������� ������������ �� ����������� �����
			m_vLastPoint.set(m_vTargetPoint); // ��������� ������� ������� �����
			CalcNextPoint(); // ������������� ����� ������� ����� 
		};
	}
	// ������ ������� ����� ������������ ��� �������� ������������ ����� ��������� � ������� �� �������
	m_vCurrentPoint.lerp(m_vLastPoint, m_vTargetPoint, float(m_dwTimePassed)/m_dwDeltaTime);

	m_vOldCameraDir = d;	// ���������� ���������� ���������

	if(!camera_moved) // ���� ����� �� ��������
	{
		//Log("change direction");
		d.add(m_vCurrentPoint); // �� ���������� ��������� ������
	}

	m_dwTimePassed += Device.dwTimeDelta; // ����������� ������� �������

	return TRUE;
}
