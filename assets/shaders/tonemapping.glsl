vec3 toneMapFilmic(vec3 color) {
    color = max(vec3(0.0), color - vec3(0.004)); // Adjust black level
    return (color * (6.2 * color + 0.5)) / (color * (6.2 * color + 1.7) + 0.06);
}
