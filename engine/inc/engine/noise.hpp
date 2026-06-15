#pragma once

// K.jpg's OpenSimplex 2 (Smooth, "SuperSimplex")
// VoxSim C++ Port

class Noise
{
private:
    static const long PRIME_X = 0x5205402B9270C86FL;
    static const long PRIME_Y = 0x598CD327003817B5L;
    static const long PRIME_Z = 0x5BCC226E9FA0BACBL;
    static const long PRIME_W = 0x56CC5227E58F554BL;
    static const long HASH_MULTIPLIER = 0x53A3F72DEEC546F5L;

    static constexpr double ROOT2OVER2 = 0.7071067811865476;
    static constexpr double SKEW_2D = 0.366025403784439;
    static constexpr double UNSKEW_2D = -0.21132486540518713;

    static constexpr double ROOT3OVER3 = 0.577350269189626;
    static constexpr double FALLBACK_ROTATE3 = 2.0 / 3.0;
    static constexpr double ROTATE3_ORTHOGONALIZER = UNSKEW_2D;

    static const int N_GRADIENTS_2D_EXPONENT = 7;
    static const int N_GRADIENTS_2D = 1 << N_GRADIENTS_2D_EXPONENT;

    static constexpr double NORMALIZER_2D = 0.05481866495625118;

    static constexpr float RSQUARED_2D = 2.0f / 3.0f;
    static constexpr float RSQUARED_3D = 3.0f / 4.0f;
    static constexpr float RSQUARED_4D = 4.0f / 5.0f;

    inline static float GRADIENTS_2D[N_GRADIENTS_2D * 2] = {0};

public:
    Noise();

    static float get2D(long seed, double x, double y);

    static float get2D_UnskewedBase(long seed, double xs, double ys);

private:
    inline static float gradient(long seed, long xsvp, long ysvp, float dx, float dy);

    inline static int fastFloor(double x);
};