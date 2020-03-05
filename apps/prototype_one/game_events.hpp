#ifndef WIZARD_GAME_EVENTS_HPP
#define WIZARD_GAME_EVENTS_HPP

#include <event/event.hpp>

struct team_killed_event : public event::Event
{
public:
	explicit team_killed_event(unsigned int _team) : event::Event(event::EVENT_TYPE::TEAM_KILLED), team(_team)
	{}

	unsigned int team;
};

struct team_damage_event : public event::Event
{
	explicit team_damage_event(unsigned int _team, float _damage) : event::Event(event::EVENT_TYPE::TEAM_DAMAGE), team(_team), damage(_damage)
	{}

	unsigned int team;
	float damage;
};

struct team_energy_spent_event : public event::Event
{
	explicit team_energy_spent_event(unsigned int _team) : event::Event(event::EVENT_TYPE::TEAM_DAMAGE), team(_team)
	{}

	unsigned int team;
	float spent;
};

#endif
