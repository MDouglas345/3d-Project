#pragma once
#include <glm/glm.hpp>
#include <vector>


struct Mesh{
	std::vector<glm::vec3> Data;
	std::vector<glm::vec3> Normals;
	std::vector<glm::vec3> UV;

	int m_width, m_height;

	float *HeightMap;


	int ID;

	Mesh(int width, int height) {
		this->m_width = width;
		this->m_height = height;
	}

	void genMesh(float seed) {
		int scale = 1;
		int yscale = 100;
		HeightMap = new float[m_width * m_height];
		float* Seed = new float[m_width * m_height];
		for (int i = 0; i < (m_width * m_height); i++) Seed[i] = (float)rand() / (float)RAND_MAX;

		PerlinNoise2D(m_width, m_height, Seed, 4, 0.9, HeightMap);

		for (int z = 0; z < this->m_width; z++) {
			for (int x = 0; x < this->m_height; x++) {


				/*this->Data.push_back(glm::vec3(x * scale, TestNoise2D(x * scale, z * scale), z * scale));
				this->Data.push_back(glm::vec3(x * scale, TestNoise2D(x * scale, (z+1) * scale), (z+1) * scale));
				this->Data.push_back(glm::vec3((x + 1) * scale, TestNoise2D((x+1) * scale, (z+1) * scale), (z+1) * scale));

				this->Data.push_back(glm::vec3(x * scale, TestNoise2D(x * scale, z * scale), z * scale));
				this->Data.push_back(glm::vec3((x+1) * scale, TestNoise2D((x+1) * scale, (z+1) * scale), (z + 1) * scale));
				this->Data.push_back(glm::vec3((x + 1) * scale, TestNoise2D((x+1) * scale, z * scale), z * scale));*/

				/*this->Data.push_back(glm::vec3(x * scale, HeightMap[z + x * m_height] * yscale , z * scale));
				this->Data.push_back(glm::vec3(x * scale, HeightMap[(z+1) + x * m_height] * yscale, (z + 1) * scale));
				this->Data.push_back(glm::vec3((x + 1) * scale, HeightMap[(z+1) + (x+1) * m_height] * yscale, (z + 1) * scale));

				this->Data.push_back(glm::vec3(x * scale, HeightMap[z + x * m_height] * yscale, z * scale));
				this->Data.push_back(glm::vec3((x + 1) * scale, HeightMap[(z+1) + (x+1) * m_height] * yscale, (z + 1) * scale));
				this->Data.push_back(glm::vec3((x + 1) * scale, HeightMap[z + (x+1) * m_height] * yscale, z * scale));*/

				this->Data.push_back(glm::vec3(x * scale, HeightMap[z + x * m_height] * yscale, z  * scale));
				this->Data.push_back(glm::vec3(x  * scale, HeightMap[(z+1) + x * m_height] * yscale, (z+1) * scale));
				
				//this->Data.push_back(glm::vec3(x * scale, 1, z * scale));
				//this->Data.push_back(glm::vec3(x * scale, 1, (z + 1) * scale));
				//this->Data.push_back(glm::vec3((x + 1) * scale, 1, (z + 1) * scale));

				//this->Data.push_back(glm::vec3(x * scale, TestNoise2D(x * scale, z * scale), z * scale));
				//this->Data.push_back(glm::vec3(x * scale, TestNoise2D(x * scale, (z+1) * scale), (z + 1) * scale));
			}
		}

	}

	float TestNoise2D(int x, int y) {
			int n;

			n = x + y * 57;
			n = (n << 13) ^ n;
			return (1.0 - ((n * ((n * n * 15731) + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
	}

	void PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput)
	{
		// Used 1D Perlin Noise
		for (int x = 0; x < nWidth; x++)
			for (int y = 0; y < nHeight; y++)
			{
				float fNoise = 0.0f;
				float fScaleAcc = 0.0f;
				float fScale = 1.0f;

				for (int o = 0; o < nOctaves; o++)
				{
					int nPitch = nWidth >> o;
					int nSampleX1 = (x / nPitch) * nPitch;
					int nSampleY1 = (y / nPitch) * nPitch;

					int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
					int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

					float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
					float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

					float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
					float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

					fScaleAcc += fScale;
					fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
					fScale = fScale / fBias;
				}

				// Scale to seed range
				fOutput[y * nWidth + x] = fNoise / fScaleAcc;
			}

	}

	float dotGridGradient(int ix, int iy, float x, float y) {

	}

};

class TerrainGenerator {
public:

	std::vector<Mesh> MapData;

	TerrainGenerator(int Width, int height, int seed) {
		MapData.push_back(Mesh(Width, height));
		MapData[0].genMesh(seed);
	}
};
