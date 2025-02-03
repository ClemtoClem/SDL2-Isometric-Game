#ifndef PERLIN_HEADER
#define PERLIN_HEADER

[[nodiscard]] double rawnoise(int n);

[[nodiscard]] double noise1d(int x, int octave, int seed);

[[nodiscard]] double noise2d(int x, int y, int octave, int seed);

[[nodiscard]] double noise3d(int x, int y, int z, int octave, int seed);

[[nodiscard]] double interpolate(double a, double b, double x);

[[nodiscard]] double smooth1d(double x, int octave, int seed);

[[nodiscard]] double smooth2d(double x, double y, int octave, int seed);

[[nodiscard]] double smooth3d(double x, double y, double z, int octave, int seed);

[[nodiscard]] double pnoise1d(double x, double persistence, int octaves, int seed);

[[nodiscard]] double pnoise2d(double x, double y, double persistence, int octaves, int seed);

[[nodiscard]] double pnoise3d(double x, double y, double z, double persistence, int octaves, int seed);

#endif
