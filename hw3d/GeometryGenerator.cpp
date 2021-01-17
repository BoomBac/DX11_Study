#include "GeometryGenerator.h"
#include <corecrt_math.h>



namespace GTool
{
	enum class EColor
	{
		yellow,
		blue,
		green_dark,
		green_light,
		white,
		brown,
	};
	Vcolor GetColor(EColor co)
	{
		switch (co)
		{
		case GTool::EColor::yellow:
			return Vcolor{ 1.0f, 0.96f, 0.62f, 1.0f };
		case GTool::EColor::blue:
			return Vcolor{ 0.0f, 0.0f, 1.0f, 1.0f };
		case GTool::EColor::green_dark:
			return Vcolor{ 0.1f, 0.48f, 0.19f, 1.0f };
		case GTool::EColor::green_light:
			return Vcolor{ 0.48f, 0.77f, 0.46f, 1.0f };
		case GTool::EColor::white:
			return Vcolor{ 1.0f, 1.0f, 1.0f, 1.0f };
		case GTool::EColor::brown:
			return Vcolor{ 0.45f, 0.39f, 0.34f, 1.0f };
		default:
			return Vcolor{ 1.0f, 1.0f, 1.0f, 1.0f };
		}
	}
	Vpostion Cross(const Vpostion& i, const Vpostion& j)
	{
		return Vpostion{ i.Y * j.Z - i.Z * j.Y,
			i.Z * j.X - i.X * j.Z,
			i.X * j.Y - i.Y * j.X };
	}
	Vpostion SimpleVSub(const Vpostion& s, const Vpostion& d)
	{
		return Vpostion{ d.X - s.X,d.Y -s.Y,d.Z - s.Z };
	}
	template <typename type>
	float Length(const type& v)
	{
		return sqrt(pow(v.X, 2) + pow(v.Y, 2) + pow(v.Z, 2));
	}
	template <typename type>
	Nomal division(const type& v, float num)
	{
		return Nomal{ v.X / num,v.Y / num,v.Z / num };
	}
	template<typename type>
	type Nomalize(const type& v)
	{
		return division(v, Length(v));
	}
}

const int TriangleNomCount = 80000;


GeometryGenerator::GeometryGenerator()
{

}

bool GeometryGenerator::GenerateHill(float width, float depth, int count_x, int count_z, SimpleVertex* VertexBuffer, unsigned short* IndoxBuffer)
{
	float gap_x = width / (count_x - 1);
	float gap_z = depth / (count_z - 1);
	int TriangleCount = (count_x - 1) * (count_z - 1) * 2;
	int Vcount = VertexUsed;
	int Icount = IndoxUsed;
	for (int countz = 0; countz < count_z; countz++)
	{
		for (int countx = 0; countx < count_x; countx++)
		{
			VertexBuffer[Vcount].pos.X = (-width) / 2.f + gap_x * countx;
			VertexBuffer[Vcount].pos.Z= depth / 2.f - gap_z * countz;
			VertexBuffer[Vcount].pos = {
				VertexBuffer[Vcount].pos.X,
				GetHeight(VertexBuffer[Vcount].pos.X, VertexBuffer[Vcount].pos.Z),
				VertexBuffer[Vcount].pos.Z };
			VertexBuffer[Vcount].Tex.u = (float)countx / ((float)count_x - 1.f);
			VertexBuffer[Vcount].Tex.v = (float)countz / ((float)count_z - 1.f);
			Vcount++;
		}
	}
	for (int count = 0; count < count_x * count_z; count++)
	{
		if ((count - count_x + 1) >= 0 && (count - count_x + 1) % (count_x) == 0)
			continue;
		if ((Icount-IndoxUsed) < TriangleCount * 3)
		{
			IndoxBuffer[Icount] = VertexUsed+count;
			IndoxBuffer[Icount + 1] = VertexUsed + count + 1;
			IndoxBuffer[Icount + 2] = VertexUsed + count + count_x + 1;
			IndoxBuffer[Icount + 3] = VertexUsed + count;
			IndoxBuffer[Icount + 4] = VertexUsed + count + count_x + 1;
			IndoxBuffer[Icount + 5] = VertexUsed + count + count_x;
			Icount += 6;
		}
	}
	Nomal TriNom[TriangleNomCount];
	for (int i = 0; i < count_x * count_z; i++)
	{
		ComputeNomal(VertexBuffer, IndoxBuffer, TriNom, VertexBuffer[i],i,GET_POINTAMOUNT(count_x,count_z),GET_INDOX_AMOUNT(count_x,count_z));
	}
	VertexUsed += GET_POINTAMOUNT(count_x, count_z);
	IndoxUsed += GET_INDOX_AMOUNT(count_x, count_z);

	return true;
}

bool GeometryGenerator::GeneratePlane(float width, float depth, int count_x, int count_z, SimpleVertex* VertexBuffer, unsigned short* IndoxBuffer,
	Nomal normal, Vpostion origin)
{
	float gap_x = width / (count_x - 1);
	float gap_z = depth / (count_z - 1);
	int TriangleCount = (count_x - 1) * (count_z - 1) * 2;
	int Vcount = VertexUsed;
	int Icount = IndoxUsed;
	for (int countz = 0; countz < count_z; countz++)
	{
		for (int countx = 0; countx < count_x; countx++)
		{
			if (normal.X==1)
			{
				VertexBuffer[Vcount].pos.Y = (-width) / 2.f + gap_x * countx +  origin.Y;
				VertexBuffer[Vcount].pos.Z = depth / 2.f - gap_z * countz + origin.Z;
				VertexBuffer[Vcount].pos.X = 0.f +- origin.X;
			}
			if (normal.Y == 1)
			{
				VertexBuffer[Vcount].pos.X = (-width) / 2.f + gap_x * countx +  origin.X;
				VertexBuffer[Vcount].pos.Z = depth / 2.f - gap_z * countz +  origin.Z;
				VertexBuffer[Vcount].pos.Y = 0.f +  origin.Y;
			}
			if (normal.Z==1)
			{
				VertexBuffer[Vcount].pos.X = (-width) / 2.f + gap_x * countx + origin.X;
				VertexBuffer[Vcount].pos.Y = depth / 2.f - gap_z * countz +  origin.Y;
				VertexBuffer[Vcount].pos.Z = 0.f +  origin.Z;
			}
			VertexBuffer[Vcount].Tex.u = (float)countx / ((float)count_x-1.f);
			VertexBuffer[Vcount].Tex.v = (float)countz / ((float)count_z-1.f);
			Vcount++;
		}
	}

	for (int count = 0; count < count_x * count_z; count++)
	{
		if ((count - count_x + 1) >= 0 && (count - count_x + 1) % (count_x) == 0)
			continue;
		if ((Icount - IndoxUsed )< TriangleCount * 3)
		{
			IndoxBuffer[Icount] = VertexUsed + count;
			IndoxBuffer[Icount + 1] = VertexUsed + count + 1;
			IndoxBuffer[Icount + 2] = VertexUsed + count + count_x + 1;
			IndoxBuffer[Icount + 3] = VertexUsed + count;
			IndoxBuffer[Icount + 4] = VertexUsed + count + count_x + 1;
			IndoxBuffer[Icount + 5] = VertexUsed + count + count_x;
			Icount += 6;
		}
	}
	Nomal TriNom[TriangleNomCount];
	for (int i = 0; i < count_x * count_z; i++)
	{
		ComputeNomal(VertexBuffer, IndoxBuffer, TriNom, VertexBuffer[i], i, GET_POINTAMOUNT(count_x, count_z), GET_INDOX_AMOUNT(count_x, count_z));
	}
	VertexUsed += GET_POINTAMOUNT(count_x, count_z);
	IndoxUsed += GET_INDOX_AMOUNT(count_x, count_z);
	return true;
}

bool GeometryGenerator::GenerateBox(float width, float height, float depth, SimpleVertex* VertexBuffer, unsigned short* IndoxBuffer,int Voffset,int Ioffset, Vpostion origin)
{
	SimpleVertex vbuffer[8] = { 0 };

	for (auto i : vbuffer)
	{
		i.nomal = { 0.0f,0.0f,0.0f };
		i.Tex.u = 0.5;
		i.Tex.v = 0.5;
	}
	vbuffer[0].pos = { origin.X + width / 2.f,origin.Y - height / 2.f,origin.Z + depth / 2.f };
	vbuffer[1].pos = { origin.X + width / 2.f,origin.Y - height / 2.f,origin.Z - depth / 2.f };
	vbuffer[2].pos = { origin.X - width / 2.f,origin.Y - height / 2.f,origin.Z - depth / 2.f };
	vbuffer[3].pos = { origin.X - width / 2.f,origin.Y - height / 2.f,origin.Z + depth / 2.f };
	vbuffer[4].pos = { origin.X + width / 2.f,origin.Y + height / 2.f,origin.Z + depth / 2.f };
	vbuffer[5].pos = { origin.X + width / 2.f,origin.Y + height / 2.f,origin.Z - depth / 2.f };
	vbuffer[6].pos = { origin.X - width / 2.f,origin.Y + height / 2.f,origin.Z - depth / 2.f };
	vbuffer[7].pos = { origin.X - width / 2.f,origin.Y + height / 2.f,origin.Z + depth / 2.f };

	for (int i = 0; i < sizeof(vbuffer) / sizeof(SimpleVertex); i++)
	{
		VertexBuffer[VertexUsed + i] = vbuffer[i];
	}
	unsigned short ibuffer[] =
	{
		5,1,2,	//front
		2,6,5,
		7,3,0,//back
		0,4,7,
		7,4,5,//top
		5,6,7,
		0,3,2,//bot
		2,1,0,
		5,4,0,//r
		0,1,5,
		7,6,2,//l
		2,3,7
	};
	for (int i = 0; i < sizeof(ibuffer) / sizeof(unsigned short); i++)
	{
		ibuffer[i] += VertexUsed;
		IndoxBuffer[IndoxUsed + i] = ibuffer[i];
	}
	VertexUsed += 8;
	IndoxUsed += 36;
	return true;
}

float GeometryGenerator::GetHeight(float &x, float &z)
{
	return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

void GeometryGenerator::ComputeNomal(SimpleVertex* vArr, unsigned short* IArr, Nomal* TriNomal, SimpleVertex& Point, int SimpleVertex_indox,int vNum,int iNum)
{
	Nomal temNomal;
	for (int i = 0 ; i < iNum; i += 3)
	{
		auto v1 = GTool::SimpleVSub(vArr[IArr[i]].pos, vArr[IArr[i + 1]].pos);
		auto v2 = GTool::SimpleVSub(vArr[IArr[i+1]].pos, vArr[IArr[i + 2]].pos);
		auto v3 = GTool::Cross(v1, v2);
		temNomal.X = v3.X;
		temNomal.Y = v3.Y;
		temNomal.Z = v3.Z;
		TriNomal[i / 3] = temNomal;
	}
	Nomal PointNomal = { 0 };
		for (int j = 0; j < iNum; j++)
		{
			if (SimpleVertex_indox == IArr[j])
				{
					PointNomal += TriNomal[j / 3];
				}
		}
		Point.nomal = GTool::Nomalize(PointNomal);
}

int GeometryGenerator::VertexUsed = 0;
int GeometryGenerator::IndoxUsed = 0;
