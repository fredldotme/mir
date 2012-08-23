/*
 * Copyright © 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by:
 *   Christopher James Halse Rogers <christopher.halse.rogers@canonical.com>
 */

#ifndef MIR_GRAPHICS_GBM_GBM_BUFFER_H_
#define MIR_GRAPHICS_GBM_GBM_BUFFER_H_

#include "mir/compositor/buffer.h"
#include "mir/graphics/graphic_alloc_adaptor.h"

#include <stdexcept>
#include <memory>

namespace mir
{
namespace graphics
{
namespace gbm
{

class GBMBuffer: public compositor::Buffer
{
public:
    GBMBuffer(const std::shared_ptr<GraphicAllocAdaptor>& device, geometry::Width w, geometry::Height h, compositor::PixelFormat pf);
    virtual ~GBMBuffer();

    virtual geometry::Width width() const;

    virtual geometry::Height height() const;

    virtual geometry::Stride stride() const;

    virtual compositor::PixelFormat pixel_format() const;

    virtual void lock();

    virtual void unlock();

    virtual Texture* bind_to_texture();

private:
    const geometry::Width  buffer_width;
    const geometry::Height buffer_height;
    const compositor::PixelFormat buffer_format;
    geometry::Stride buffer_stride;

    const std::shared_ptr<GraphicAllocAdaptor> alloc_device;

    std::shared_ptr<BufferHandle> gbm_handle;
};

}
}
}


#endif // MIR_GRAPHICS_GBM_GBM_BUFFER_H_
