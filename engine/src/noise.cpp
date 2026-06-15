#include <engine/noise.hpp>

Noise::Noise()
{
    float gradient2[] = {
        0.38268343236509f,
        0.923879532511287f,
        0.923879532511287f,
        0.38268343236509f,
        0.923879532511287f,
        -0.38268343236509f,
        0.38268343236509f,
        -0.923879532511287f,
        -0.38268343236509f,
        -0.923879532511287f,
        -0.923879532511287f,
        -0.38268343236509f,
        -0.923879532511287f,
        0.38268343236509f,
        -0.38268343236509f,
        0.923879532511287f,
        //-------------------------------------//
        0.130526192220052f,
        0.99144486137381f,
        0.608761429008721f,
        0.793353340291235f,
        0.793353340291235f,
        0.608761429008721f,
        0.99144486137381f,
        0.130526192220051f,
        0.99144486137381f,
        -0.130526192220051f,
        0.793353340291235f,
        -0.60876142900872f,
        0.608761429008721f,
        -0.793353340291235f,
        0.130526192220052f,
        -0.99144486137381f,
        -0.130526192220052f,
        -0.99144486137381f,
        -0.608761429008721f,
        -0.793353340291235f,
        -0.793353340291235f,
        -0.608761429008721f,
        -0.99144486137381f,
        -0.130526192220052f,
        -0.99144486137381f,
        0.130526192220051f,
        -0.793353340291235f,
        0.608761429008721f,
        -0.608761429008721f,
        0.793353340291235f,
        -0.130526192220052f,
        0.99144486137381f,
    };

    for (int i = 0; i < sizeof(gradient2) / sizeof(float); i++)
    {
        gradient2[i] = static_cast<float>(gradient2[i] / NORMALIZER_2D);
    }

    for (int i = 0, j = 0; i < sizeof(GRADIENTS_2D) / sizeof(float); i++, j++)
    {
        if (j == sizeof(gradient2) / sizeof(float)) j = 0;

        GRADIENTS_2D[i] = gradient2[j];
    }
}

float Noise::get2D(long seed, double x, double y)
{
    double s = SKEW_2D * (x + y);
    double xs = x + s, ys = y + s;

    return get2D_UnskewedBase(seed, xs, ys);
}

float Noise::get2D_UnskewedBase(long seed, double xs, double ys)
{
    int xsb = fastFloor(xs), ysb = fastFloor(ys);
    float xi = static_cast<float>(xs - xsb), yi = static_cast<float>(ys - ysb);

    long xsbp = xsb * PRIME_X, ysbp = ysb * PRIME_Y;

    float t = (xi + yi) * static_cast<float>(UNSKEW_2D);
    float dx0 = xi + t, dy0 = yi + t;

    float a0 = RSQUARED_2D - dx0 * dx0 - dy0 * dy0;
    float value = (a0 * a0) * (a0 * a0) * gradient(seed, xsbp, ysbp, dx0, dy0);

    float a1 = static_cast<float>(2 * (1 + 2 * UNSKEW_2D) * (1 / UNSKEW_2D + 2)) * t + (static_cast<float>(-2 * (1 + 2 * UNSKEW_2D) * (1 + 2 * UNSKEW_2D)) + a0);
    float dx1 = dx0 - static_cast<float>(1 + 2 * UNSKEW_2D);
    float dy1 = dy0 - static_cast<float>(1 + 2 * UNSKEW_2D);
    value += (a1 * a1) * (a1 * a1) * gradient(seed, xsbp + PRIME_X, ysbp + PRIME_Y, dx1, dy1);

    float xmyi = xi - yi;
    if (t < UNSKEW_2D)
    {
        if (xi + xmyi > 1)
        {
            float dx2 = dx0 - static_cast<float>(3 * UNSKEW_2D + 2);
            float dy2 = dy0 - static_cast<float>(3 * UNSKEW_2D + 1);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;

            if (a2 > 0)
            {
                value += (a2 * a2) * (a2 * a2) * gradient(seed, xsbp + (PRIME_X << 1), ysbp + PRIME_Y, dx2, dy2);
            }
        }
        else
        {
            float dx2 = dx0 - static_cast<float>(UNSKEW_2D);
            float dy2 = dy0 - static_cast<float>(UNSKEW_2D + 1);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;

            if (a2 > 0)
            {
                value += (a2 * a2) * (a2 * a2) * gradient(seed, xsbp, ysbp + PRIME_Y, dx2, dy2);
            }
        }

        if (yi - xmyi > 1)
        {
            float dx3 = dx0 - static_cast<float>(3 * UNSKEW_2D + 1);
            float dy3 = dy0 - static_cast<float>(3 * UNSKEW_2D + 2);
            float a3 = RSQUARED_2D - dx3 * dx3 - dy3 * dy3;

            if (a3 > 0)
            {
                value += (a3 * a3) * (a3 * a3) * gradient(seed, xsbp + PRIME_X, ysbp + (PRIME_Y << 1), dx3, dy3);
            }
        }
        else
        {
            float dx3 = dx0 - static_cast<float>(UNSKEW_2D + 1);
            float dy3 = dy0 - static_cast<float>(UNSKEW_2D);
            float a3 = RSQUARED_2D - dx3 * dx3 - dy3 * dy3;

            if (a3 > 0)
            {
                value += (a3 * a3) * (a3 * a3) * gradient(seed, xsbp + PRIME_X, ysbp, dx3, dy3);
            }
        }
    }
    else
    {
        if (xi + xmyi < 0)
        {
            float dx2 = dx0 + static_cast<float>(1 + UNSKEW_2D);
            float dy2 = dy0 + static_cast<float>(UNSKEW_2D);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;

            if (a2 > 0)
            {
                value += (a2 * a2) * (a2 * a2) * gradient(seed, xsbp - PRIME_X, ysbp, dx2, dy2);
            }
        }
        else
        {
            float dx2 = dx0 - static_cast<float>(UNSKEW_2D + 1);
            float dy2 = dy0 - static_cast<float>(UNSKEW_2D);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;

            if (a2 > 0)
            {
                value += (a2 * a2) * (a2 * a2) * gradient(seed, xsbp + PRIME_X, ysbp, dx2, dy2);
            }
        }

        if (yi < xmyi)
        {
            float dx2 = dx0 + static_cast<float>(UNSKEW_2D);
            float dy2 = dy0 + static_cast<float>(UNSKEW_2D + 1);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;

            if (a2 > 0)
            {
                value += (a2 * a2) * (a2 * a2) * gradient(seed, xsbp, ysbp - PRIME_Y, dx2, dy2);
            }
        }
        else
        {
            float dx2 = dx0 - static_cast<float>(UNSKEW_2D);
            float dy2 = dy0 - static_cast<float>(UNSKEW_2D + 1);
            float a2 = RSQUARED_2D - dx2 * dx2 - dy2 * dy2;

            if (a2 > 0)
            {
                value += (a2 * a2) * (a2 * a2) * gradient(seed, xsbp, ysbp + PRIME_Y, dx2, dy2);
            }
        }
    }

    return value;
}

inline float Noise::gradient(long seed, long xsvp, long ysvp, float dx, float dy)
{
    long hash = seed ^ xsvp ^ ysvp;
    hash *= HASH_MULTIPLIER;
    hash ^= hash >> (64 - N_GRADIENTS_2D_EXPONENT + 1);

    int gi = (int)hash & ((N_GRADIENTS_2D - 1) << 1);

    return GRADIENTS_2D[gi | 0] * dx + GRADIENTS_2D[gi | 1] * dy;
}

inline int Noise::fastFloor(double x)
{
    int xi = static_cast<int>(x);

    return x < xi ? xi - 1 : xi;
}