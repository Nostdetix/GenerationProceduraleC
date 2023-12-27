#include <iostream>
#include <fstream>
#include <cmath>
#include <ctime>
#include <vector>
#include <numeric>
#include <random>
#include <algorithm>

// Classe pour générer un bruit Perlin
class PerlinNoise {
public:
    PerlinNoise(unsigned int seed);

    double noise(double x, double y, double z);

private:
    std::vector<int> p;

    double fade(double t);
    double lerp(double t, double a, double b);
    double grad(int hash, double x, double y, double z);
};

PerlinNoise::PerlinNoise(unsigned int seed) {
    // Initialisez le générateur de bruit Perlin avec la seed
    p.resize(512);
    std::iota(p.begin(), p.end(), 0);
    std::default_random_engine engine(seed);
    std::shuffle(p.begin(), p.end(), engine);
    p.insert(p.end(), p.begin(), p.end());
}

double PerlinNoise::fade(double t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

double PerlinNoise::lerp(double t, double a, double b) {
    return a + t * (b - a);
}

double PerlinNoise::grad(int hash, double x, double y, double z) {
    int h = hash & 15;
    double u = h < 8 ? x : y;
    double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double PerlinNoise::noise(double x, double y, double z) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    int Z = (int)floor(z) & 255;

    x -= floor(x);
    y -= floor(y);
    z -= floor(z);

    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
    int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

    return lerp(w, lerp(v, lerp(u, grad(p[AA], x, y, z), grad(p[BA], x - 1, y, z)), lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))),
               lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1), grad(p[BA + 1], x - 1, y, z - 1)), lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))));
}

// Fonction pour générer un bruit blanc
double whiteNoise() {
    return static_cast<double>(rand()) / RAND_MAX;
}

void getBiomeColor(double noiseValue, int& red, int& green, int& blue) {  
    if (noiseValue < 0.05) {
        // Jaune pour le désert
        red = 255;
        green = 255;
        blue = 0;
    } else if (noiseValue < 0.1) {
        // Vert foncé pour la forêt
        red = 10;
        green = 10;
        blue = 10;
    } else if (noiseValue < 0.2) {
        // Vert foncé pour la forêt
        red = 34;
        green = 139;
        blue = 34;
    } else if (noiseValue < 0.4) {
        // Vert plus clair pour les prairies
        red = 154;
        green = 205;
        blue = 50;
    } else if (noiseValue < 0.6) {
        // Brun pour les montagnes
        red = 139;
        green = 69;
        blue = 19;
    } else if (noiseValue < 0.8) {
        // Blanc pour les pics enneigés
        red = 255;
        green = 255;
        blue = 255;
    } else {
        // Bleu clair pour l'eau
        red = 135;
        green = 206;
        blue = 250;
    }
        // Ajustez la couleur en fonction de la hauteur représentée par le bruit blanc
    double heightFactor = 0.5;  // Ajustez ce facteur pour déterminer l'impact de la hauteur
    red *= 1.0 - heightFactor * whiteNoise();
    green *= 1.0 - heightFactor * whiteNoise();
    blue *= 1.0 - heightFactor * whiteNoise();
}


int main() {
    const int width = 1000;
    const int height = 1000;
    const unsigned int seed = static_cast<unsigned int>(std::time(0));

    PerlinNoise perlinNoise(seed);

    // Création du fichier image
    std::ofstream image("map.ppm");
    image << "P3\n" << width << " " << height << "\n255\n";

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double noiseValue = perlinNoise.noise(static_cast<double>(x) / 50.0, static_cast<double>(y) / 50.0, 0.0);
            int red, green, blue;
            getBiomeColor(noiseValue, red, green, blue);
            image << red << " " << green << " " << blue << "\n";
        }
    }
}
void addFractalNoise(std::vector<std::vector<double>>& heightMap, double fractalScale, int octaves) {
    PerlinNoise fractalNoise(time(0)); // Use time as seed for randomness

    int width = heightMap.size();
    int height = heightMap[0].size();

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double fractalValue = 0.0;

            for (int octave = 0; octave < octaves; ++octave) {
                double frequency = pow(2.0, octave);
                double amplitude = pow(fractalScale, -octave);

                fractalValue += fractalNoise.noise(x / frequency, y / frequency, 0.0) * amplitude;
            }

            heightMap[x][y] += fractalValue;
        }
    }
}