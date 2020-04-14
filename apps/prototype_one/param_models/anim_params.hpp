#pragma once

struct anim_params
{
	bool m_walked_recently = false;
	float m_time_last_walked = 0.f;
	int m_current_move_sprite = 0;
	bool m_is_placing_unit = false;
	float m_time_started_placing_unit = 0.f;
	bool m_is_vacuuming_energy = false;
};