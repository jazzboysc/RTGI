#version 120

#define MAX_WAVES 32

struct CWave
{
	float StartTime, Speed, MaxY, FrequencyMPIM2;
	vec2 Position;
};

uniform float Time;
uniform CWave Waves[MAX_WAVES];

varying vec2 Position;

void main()
{
	float y = 0.0;

	for(int i = 0; i < MAX_WAVES; i++)
	{
		float d = distance(Waves[i].Position, Position);
		float t = Time - Waves[i].StartTime - d / Waves[i].Speed;

		if(t > 0.0)
		{
			float maxy = Waves[i].MaxY - Waves[i].MaxY * t;

			if(maxy > 0.0)
			{
				y -= sin(t * Waves[i].FrequencyMPIM2) * maxy / (1.0 + d);
			}
		}
	}

	gl_FragColor = vec4(0.0, 1.0, 0.0, y);
}
