struct ImageRegion {
    int32_t x = 0;
    int32_t y = 0;
    int32_t z = 0;
    int32_t width = 0;
    int32_t height = 0;
    int32_t depth = 1;

    ImageRegion() = default;
    ImageRegion(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height, int32_t depth = 1)
        : x(x), y(y), z(z), width(width), height(height), depth(depth) {}
};