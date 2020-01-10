/*
 * Copyright (C) 2015, Simon Fuhrmann
 * TU Darmstadt - Graphics, Capture and Massively Parallel Computing
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-Clause license. See the LICENSE.txt file for details.
 */

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "util/timer.h"
#include "math/functions.h"
#include "math/matrix.h"
#include "math/matrix_tools.h"
#include "mve/image_io.h"
#include "mve/image_tools.h"
#include "sfm/superpoint.h"

SFM_NAMESPACE_BEGIN

SuperPoint::SuperPoint (Options const& options)
    : options(options)
{


    if (this->options.debug_output)
        this->options.verbose_output = true;
}

/* ---------------------------------------------------------------- */

void
SuperPoint::process (void)
{
    
}

/* ---------------------------------------------------------------- */

void
SuperPoint::set_image (mve::ByteImage::ConstPtr img)
{
    if (img->channels() != 1 && img->channels() != 3)
        throw std::invalid_argument("Gray or color image expected");

    this->orig = mve::image::byte_to_float_image(img);
    if (img->channels() == 3)
    {
        this->orig = mve::image::desaturate<float>
            (this->orig, mve::image::DESATURATE_AVERAGE);
    }
}

/* ---------------------------------------------------------------- */

void
SuperPoint::set_float_image (mve::FloatImage::ConstPtr img)
{
    if (img->channels() != 1 && img->channels() != 3)
        throw std::invalid_argument("Gray or color image expected");

    if (img->channels() == 3)
    {
        this->orig = mve::image::desaturate<float>
            (img, mve::image::DESATURATE_AVERAGE);
    }
    else
    {
        this->orig = img->duplicate();
    }
}





SFM_NAMESPACE_END
