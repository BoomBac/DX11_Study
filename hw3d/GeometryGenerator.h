#pragma once
struct Vpostion
{
	float X;
	float Y;
	float Z;
};
struct Vcolor
{
	float r;
	float g;
	float b;
	float a;
};
struct Nomal
{
	float X;
	float Y;
	float Z;
	Nomal operator +=(const Nomal &input)
	{
		this->X += input.X;
		this->Y += input.Y;
		this->Z +=input.Z;
		return *this;
	}
};

struct SimpleVertex
{
	Vpostion pos;
	Nomal nomal;
};

#define GET_POINTAMOUNT(x,y) x*y
#define GET_INDOX_AMOUNT(x,y) (x-1)*(y-1)*6

class GeometryGenerator
{
public:
	GeometryGenerator();
	static bool GenerateHill(float width,float depth,int count_x,int count_z, SimpleVertex* VertexBuffer, unsigned short* IndoxBuffer);
	static bool GeneratePlane(float width,float depth,int count_x,int count_z, SimpleVertex* VertexBuffer, unsigned short* IndoxBuffer);
	static bool GenerateBox(float width, float height, float depth, SimpleVertex* VertexBuffer, unsigned short* IndoxBuffer, int Voffset, int Ioffset,Vpostion origin);
	static float GetHeight(float &x, float &z);
	//顶点缓冲区，索引缓冲区，三角形法线缓冲区，要求法线的点，当前点位于顶点缓冲的位置
	static void ComputeNomal(SimpleVertex* vArr, unsigned short* IArr, Nomal* TriNomal, SimpleVertex& Point, int SimpleVertex_indox);
};

