#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <memory>
#include <cstdlib>

#include "pf.h"

#include "model/colormap.h"
#include "model/image.h"
#include "model/enums.h"
#include "model/fractfunc.h"
#include "model/vectors.h"
#include "model/fractgeometry.h"

#define MAX_ITERATIONS 100

constexpr double pos_params[N_PARAMS] {
    0.0, 0.0, 0.0, 0.0, // X Y Z W
    4.0, // Size or zoom
    0.0, 0.0, 0.0, 0.0, 0.0, 0.0 // XY XZ XW YZ YW ZW planes (4D stuff)
};

int result[2];

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
void generate_fractal()
{
    pf_obj *pf_handle = pf_new();

    // formula params: [0, 4.0, 0.0, 1.0, 4.0, 0.0, 1.0]
    int param_len = 7;
    auto params{std::make_unique<s_param []>(param_len)};
    params[0].t = INT;
    params[0].intval = 0;
    params[1].t = FLOAT;
    params[1].doubleval = 4.0;
    params[2].t = FLOAT;
    params[2].doubleval = 0.0;
    params[3].t = FLOAT;
    params[3].doubleval = 1.0;
    params[4].t = FLOAT;
    params[4].doubleval = 4.0;
    params[5].t = FLOAT;
    params[5].doubleval = 0.0;
    params[6].t = FLOAT;
    params[6].doubleval = 1.0;

    // initialize the point function with the params
    pf_handle->vtbl->init(pf_handle, const_cast<double *>(pos_params), params.get(), param_len);

    // create the colormap with 3 colors
    std::unique_ptr<ListColorMap> cmap{new (std::nothrow) ListColorMap{}};
    cmap->init(3);
    cmap->set(0, 0.0, 0, 0, 0, 255);
    cmap->set(1, 0.004, 255, 255, 255, 255);
    cmap->set(2, 1.0, 255, 255, 255, 255);

    // create the image (logic representation)
    auto im{std::make_unique<image>()};
    im->set_resolution(640, 480, -1, -1);

    const auto w = im->Xres();
    const auto h = im->Yres();

    // calculate the 4D vectors: topleft and deltas (x, y)
    fract_geometry geometry { const_cast<double *>(pos_params), false, w, h, 0, 0 };

    // we put these variables out of the loop scope to use its previous value
    int iters_taken = 0;
    int min_period_iters = 0;
    // now we calculate every pixel (for a 2D image projection in a single tile)
    for (auto x = 0; x < w; x++) {
        for (auto y = 0; y < h; y++) {
            // calculate the position in 4D (x, y, z, w)
            dvec4 pos = geometry.vec_for_point_2d(x, y);
            // run the formula
            double dist = 0.0;
            int fate = 0;
            int solid = 0;
            int direct_color = 0;
            double colors[4] = {0.0};
            int inside = 0;
            if (iters_taken == -1) { // we got inside the last time so we'll probably do it again
                min_period_iters = 0;
            } else {
                min_period_iters = std::min(min_period_iters + 10, MAX_ITERATIONS);
            }
            pf_handle->vtbl->calc(
                pf_handle,
                pos.n,
                MAX_ITERATIONS,
                -1, // wrap param
                min_period_iters,
                1.0E-9, // period tolerance
                x, y, 0, // x, y and aa: these values are not needed in the formula but required as arguments for debugging purposes
                &iters_taken, &fate, &dist, &solid,
                &direct_color, &colors[0]);
            // process the formula output and get the value from colormap
            rgba_t color{};
            if (fate & FATE_INSIDE) {
                iters_taken = -1;
                inside = 1;
            }
            if (direct_color) {
                color = cmap->lookup_with_dca(solid, inside, colors);
                fate |= FATE_DIRECT;
            } else {
                color = cmap->lookup_with_transfer(dist, solid, inside);
            }
            if (solid)
            {
                fate |= FATE_SOLID;
            }
            // this is only needed for optimization and zooming
            im->setIter(x, y, iters_taken);
            im->setFate(x, y, 0, fate);
            im->setIndex(x, y, 0, dist);
            // put the pixel color into the image buffer position
            im->put(x, y, color);
        }
    }

    // cleaning
    pf_handle->vtbl->kill(pf_handle);

    // return the result pointer
    uint8_t* image_buffer = static_cast<uint8_t *>(malloc(im->bytes()));
    memcpy(image_buffer, im->getBuffer(), im->bytes());

    result[0] = reinterpret_cast<int>(image_buffer);
    result[1] = im->bytes();
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
void destroy_fractal(uint8_t* result) {
    free(result);
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
int get_result_pointer() {
  return result[0];
}

#ifdef __EMSCRIPTEN__
EMSCRIPTEN_KEEPALIVE
#endif
int get_result_size() {
  return result[1];
}

#ifdef __cplusplus
}
#endif