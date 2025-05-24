#include <cmath>
#include <cstdlib>
#include <ctime>
#include <array>
#include <raylib.h>

class Vec {
public:
    float x, y, z;

    Vec() : x{0}, y{0}, z{0} {}
    Vec(float xx, float yy, float zz) : x{xx}, y{yy}, z{zz} {}

    friend Vec operator+(const Vec& v1, const Vec& v2) {
        return Vec(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
    }

    friend Vec operator*(const Vec& v, float q) {
        return Vec(v.x * q, v.y * q, v.z * q);
    }
};

float clamp(float val, float min, float max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

int main() {
    const int width = 640;
    const int height = 640;

    InitWindow(width, height, "Diffusion");
    SetTargetFPS(60);
    srand(static_cast<unsigned int>(time(0))); // Seed random

    std::array<std::array<Vec, height>, width> current;
    std::array<std::array<Vec, height>, width> next;

    // Initialize with noise
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            float z = static_cast<float>(rand()) / RAND_MAX * 255.0f;
            current[i][j] = Vec(i, j, z);
        }
    }

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        // Compute next frame
        for (int i = 1; i < width - 1; i++) {
            for (int j = 1; j < height - 1; j++) {
                Vec avg = (
                    current[i-1][j-1] + current[i][j-1] + current[i+1][j-1] +
                    current[i-1][j  ] + current[i][j  ] + current[i+1][j  ] +
                    current[i-1][j+1] + current[i][j+1] + current[i+1][j+1]
                ) * (1.0f / 9.0f);

                float newZ = clamp(avg.z, 0.0f, 255.0f);
                next[i][j] = Vec(i, j, newZ);
            }
        }

        // Swap buffers
        current.swap(next);

        // Draw to screen
        for (int i = 0; i < width; i++) {
            for (int j = 0; j < height; j++) {
                unsigned char brightness = static_cast<unsigned char>(clamp(current[i][j].z, 0.0f, 255.0f));
                DrawPixel(i, j, Color{brightness, brightness, brightness, 255});
            }
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
