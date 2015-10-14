#ifndef MAPBOX_UTIL_GEOJSONVT
#define MAPBOX_UTIL_GEOJSONVT

#include "geojsonvt_types.hpp"
#include "geojsonvt_tile.hpp"

#include <map>
#include <mutex>
#include <string>
#include <vector>

namespace mapbox {
namespace util {
namespace geojsonvt {

class GeoJSONVT {
public:
    static std::vector<ProjectedFeature> convertFeatures(const std::string& data,
                                                         uint8_t maxZoom = 14,
                                                         double tolerance = 3,
                                                         bool debug = false);

    GeoJSONVT(std::vector<ProjectedFeature> features_,
              uint8_t maxZoom = 14,
              uint8_t indexMaxZoom = 5,
              uint32_t indexMaxPoints = 100000,
              bool solidChildren = false,
              double tolerance = 3,
              bool debug = false);

    const Tile& getTile(uint8_t z, uint32_t x, uint32_t y);

    const std::map<uint64_t, Tile>& getAllTiles() const;

    // returns the total number of tiles generated until now.
    uint64_t getTotal() const;

    static const Tile emptyTile;

private:
    void splitTile(std::vector<ProjectedFeature> features,
                   uint8_t z,
                   uint32_t x,
                   uint32_t y,
                   uint8_t cz = 0,
                   uint32_t cx = 0,
                   uint32_t cy = 0);

    static TilePoint
    transformPoint(const ProjectedPoint& p, uint16_t extent, uint32_t z2, uint32_t tx, uint32_t ty);
    static const Tile& transformTile(Tile&, uint16_t extent);

    static uint64_t toID(uint8_t z, uint32_t x, uint32_t y);

    static ProjectedPoint intersectX(const ProjectedPoint& a, const ProjectedPoint& b, double x);

    static ProjectedPoint intersectY(const ProjectedPoint& a, const ProjectedPoint& b, double y);

    static bool isClippedSquare(Tile& tile, uint16_t extent, uint8_t buffer);

    struct FeatureStackItem {
        std::vector<ProjectedFeature> features;
        uint8_t z;
        uint32_t x;
        uint32_t y;

        FeatureStackItem(std::vector<ProjectedFeature> features_,
                         uint8_t z_,
                         uint32_t x_,
                         uint32_t y_)
            : features(features_), z(z_), x(x_), y(y_) {
        }
    };

private:
    std::mutex mtx;
    const uint8_t maxZoom;         // max zoom to preserve detail on
    const uint8_t indexMaxZoom;    // max zoom in the tile index
    const uint32_t indexMaxPoints; // max number of points per tile in the tile index
    const bool solidChildren;      // whether to tile solid square tiles further
    const double tolerance;        // simplification tolerance (higher means simpler)
    const bool debug;
    const uint16_t extent = 4096;  // tile extent
    const uint8_t buffer = 64;     // tile buffer on each side
    std::map<uint64_t, Tile> tiles;
    std::map<uint8_t, uint16_t> stats;
    uint64_t total = 0;
};

} // namespace geojsonvt
} // namespace util
} // namespace mapbox

#endif // MAPBOX_UTIL_GEOJSONVT
