# System Bit Ranges and component bits. 


## Transform 

Bit shift range: **0-0**

```c++
	enum class component_bits : component_shift
	{
		transform = 0
	};
```

## Rendering
Bit shift range: **1-16**

```c++
	enum class component_bits : component_shift
	{
		camera                 = 1,
		light_directional      = 2,
		light_point            = 3,
		renderable_mesh_static = 4
	};
```

## Audio
Bit shift range: **17-24**

```c++
    enum class component_bits : component_shift
    {
        audio_emitter = 17,
        audio_listener = 18
    };
```

## Game
Bit shift range: **32-48**

```c++
    enum class component_bits : component_shift
    {
        game_piece = 32
    };
```

