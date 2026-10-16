#include "project/TerrainGenerator.h"
#include <glm/glm.hpp>
#include <glm/gtc/noise.hpp>

namespace openglgp::project
{

float TerrainGenerator::height(float x, float z, const TerrainParams& params) const
{
    const float seedOffset = static_cast<float>(m_seed % 100000u) * 0.137f;
    float total = 0.0f, amplitude = 1.0f, frequency = params.baseFrequency, amplitudeSum = 0.0f;
    for (int i = 0; i < params.octaves; ++i)
    {
        const glm::vec3 p((x + seedOffset) * frequency, 0.0f, (z + seedOffset) * frequency);
        total += glm::perlin(p) * amplitude;
        amplitudeSum += amplitude;
        amplitude *= params.amplitudePersistence;
        frequency *= params.frequencyGain;
    }
    if (amplitudeSum > 0.0f) total /= amplitudeSum;
    return total * params.amplitude;
}

}
