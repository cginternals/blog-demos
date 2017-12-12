#version 420 core

layout(binding = 0, offset = 0) uniform atomic_uint ac;

out float out_color;

void main()
{
	uint aci = atomicCounterIncrement(ac);
	out_color = float(aci);
}
