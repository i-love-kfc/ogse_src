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

	//определяем двигал ли прицелом актер
	if(!d.similar(m_vOldCameraDir, m_fCameraMoveEpsilon))
		camera_moved = true;


	if (m_dwTimePassed == 0) // происходит ровно один, первый, раз, зачем это стоит здесь?
	{
		m_vLastPoint.set(m_vCurrentPoint);
		CalcNextPoint();
	}
	else
	{
		// это просто отмотка времени назад на m_dwDeltaTime по достижении m_dwDeltaTime
		while (m_dwTimePassed > m_dwDeltaTime)
		{
			m_dwTimePassed -= m_dwDeltaTime;
			// имитирует деятельность за пропушенное время
			m_vLastPoint.set(m_vTargetPoint); // сохраняем текущую целевую точку
			CalcNextPoint(); // устанавливаем новую целевую точку 
		};
	}
	// меняем текущую точку прицеливания как линейную интерполяцию между последней и целевой по времени
	m_vCurrentPoint.lerp(m_vLastPoint, m_vTargetPoint, float(m_dwTimePassed)/m_dwDeltaTime);

	m_vOldCameraDir = d;	// запоминаем предыдущее положение

	if(!camera_moved) // если игрок не крутился
	{
		//Log("change direction");
		d.add(m_vCurrentPoint); // то собственно применяем эффект
	}

	m_dwTimePassed += Device.dwTimeDelta; // увеличиваем счётчик времени

	return TRUE;
}
