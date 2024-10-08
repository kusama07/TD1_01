#include "Easing.h"

Vector2 Lerp(Vector2 start, Vector2 end, float t) {
    return {
        start.x + (end.x - start.x) * t,
        start.y + (end.y - start.y) * t
    };
}