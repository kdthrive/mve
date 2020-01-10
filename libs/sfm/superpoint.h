/*
 * Copyright (C) 2015, Simon Fuhrmann, Ronny Klowsky
 * TU Darmstadt - Graphics, Capture and Massively Parallel Computing
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-Clause license. See the LICENSE.txt file for details.
 *
 * Notes:
 * - The implementation allows a minmum octave of -1 only.
 * - The descriptor extration supports 128 dimensions only.
 * - Coordinates in the keypoint are relative to the octave.
 *   Absolute coordinates are obtained by (TODO why? explain):
 *   (x + 0.5, y + 0.5) * 2^octave - (0.5, 0.5).
 * - Memory consumption is quite high, especially with large images.
 *   TODO: Find a more efficent code path to create octaves.
 */
#ifndef SFM_SUPERPOINT_HEADER
#define SFM_SUPERPOINT_HEADER

#include <string>
#include <vector>

#include "math/vector.h"
#include "mve/image.h"
#include "sfm/defines.h"

SFM_NAMESPACE_BEGIN

class SuperPoint
{
public:
    /**
     * SuperPoint options.
     */
    struct Options
    {
        Options (void);
        /**
         * Produce status messages on the console.
         */
        bool verbose_output;

        /**
         * Produce even more messages on the console.
         */
        bool debug_output;
    };

    /**
     * Representation of a SIFT keypoint.
     * The keypoint locations are relative to the resampled size in
     * the image pyramid. To get the size relative to the input image,
     * each of (ix,iy,x,y) need to be multiplied with 2^o, where o
     * is the octave index of the keypoint. The octave index is -1 for the
     * upsampled image, 0 for the input image and >0 for subsampled images.
     * Note that the scale of the KP is already relative to the input image.
     */
    struct Keypoint
    {
        /** Octave index of the keypoint. Can be negative. */
        int octave;
        /** Sample index. Initally integer in {0 ... S-1}, later in [-1,S]. */
        float sample;
        /** Keypoint x-coordinate. Initially integer, later sub-pixel. */
        float x;
        /** Keypoint y-coordinate. Initially integer, later sub-pixel. */
        float y;
    };

    /**
     * Representation of the SIFT descriptor.
     * The descriptor is created in a rotation invariant way. The resulting
     * vector is unsigned and normalized, and has 128 dimensions.
     */
    struct Descriptor
    {
        /** The sub-pixel x-coordinate of the image keypoint. */
        float x;
        /** The sub-pixel y-coordinate of the image keypoint. */
        float y;

        float scale=1;
        /** The descriptor data, elements are unsigned in [0.0, 1.0]. */
        math::Vector<float, 256> data;
    };

public:
    typedef std::vector<Keypoint> Keypoints;
    typedef std::vector<Descriptor> Descriptors;

public:
    explicit SuperPoint (Options const& options);

    /** Sets the input image. */
    void set_image (mve::ByteImage::ConstPtr img);
    /** Sets the input image. */
    void set_float_image (mve::FloatImage::ConstPtr img);

    /** Starts the SUPERPOINT keypoint detection and descriptor extraction. */
    void process (void);

    /** Returns the list of keypoints. */
    Keypoints const& get_keypoints (void) const;

    /** Returns the list of descriptors. */
    Descriptors const& get_descriptors (void) const;

protected:

    bool descriptor_assignment (Keypoint const& kp, Descriptor& desc);
    float keypoint_relative_scale (Keypoint const& kp);
    float keypoint_absolute_scale (Keypoint const& kp);
    void descriptor_generation (void);

    //#############################TTTTTTTTTTOOOOOOOOOOOOOOOOODOOOOOOOOOOOOOOOOOOOOOOOOddddd

private:
    Options options;
    mve::FloatImage::ConstPtr orig; // Original input image
    Keypoints keypoints; // Detected keypoints
    Descriptors descriptors; // Final SIFT descriptors
};

/* ---------------------------------------------------------------- */

inline
SuperPoint::Options::Options (void)
    : verbose_output(false)
    , debug_output(false)
{
}

inline SuperPoint::Keypoints const&
SuperPoint::get_keypoints (void) const
{
    return this->keypoints;
}

inline SuperPoint::Descriptors const&
SuperPoint::get_descriptors (void) const
{
    return this->descriptors;
}

SFM_NAMESPACE_END

#endif /* SFM_SIFT_HEADER */
