#ifndef OPENGLGP_PROJECT_TERRAINGENERATOR_H
#define OPENGLGP_PROJECT_TERRAINGENERATOR_H

#include <cstdint>

namespace openglgp::project
{

    struct TerrainParams
    {
        float amplitude = 6.0f;
        float baseFrequency = 0.08f;
        float frequencyGain = 2.0f;
        float amplitudePersistence = 0.5f;
        int   octaves = 4;
        int   gridSize = 64;
        float worldSize = 100.0f;
    };

    class TerrainGenerator
    {
    public:
        explicit TerrainGenerator(std::uint32_t seed) : m_seed(seed) {}
        std::uint32_t seed() const { return m_seed; }
        float height(float x, float z, const TerrainParams& p) const;
    private:
        std::uint32_t m_seed;
    };

}
#endif