/*
 * Copyright (C) 2015, Simon Fuhrmann
 * TU Darmstadt - Graphics, Capture and Massively Parallel Computing
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD 3-Clause license. See the LICENSE.txt file for details.
 */

#ifndef SFM_FEATURE_SET_HEADER
#define SFM_FEATURE_SET_HEADER

#include <vector>

#include "math/vector.h"
#include "util/aligned_memory.h"
#include "sfm/sift.h"
#include "sfm/surf.h"
#include "sfm/superpoint.h"
#include "sfm/defines.h"

SFM_NAMESPACE_BEGIN

/**
 * The FeatureSet holds per-feature information for a single view, and
 * allows to transparently compute and match multiple feature types.
 * FeatureSet保存单个view中每个特征的信息
 */
class FeatureSet
{
public:
    /** Bitmask with feature types. */
    enum FeatureTypes
    {
        FEATURE_SIFT = 1 << 0,
        FEATURE_SURF = 1 << 1,
        FEATURE_SUPERPOINT = 1 << 2,
        FEATURE_ALL = 0xFF
    };

    /** Options for feature detection and matching. */
    struct Options
    {
        Options (void);

        FeatureTypes feature_types;
        // SuperPoint::Options superpoint_opts;
        Sift::Options sift_opts;
        Surf::Options surf_opts;
        SuperPoint::Options superpoint_opts;
    };

public:
    FeatureSet (void);
    explicit FeatureSet (Options const& options);
    void set_options (Options const& options);

    /** Computes the features specified in the options. */
    void compute_features (mve::ByteImage::Ptr image,std::string);

    /** Normalizes the features positions w.r.t. the image dimensions. */
    void normalize_feature_positions (float px, float py);

    /** Clear descriptor data. */
    void clear_descriptors (void);

public:
    /** Image dimension used for feature computation. */
    int width, height;
    /** Per-feature image position. */
    std::vector<math::Vec2f> positions;
    /** Per-feature image color. */
    std::vector<math::Vec3uc> colors;
    /** The SIFT descriptors. */
    Sift::Descriptors sift_descriptors;
    /** The SURF descriptors. */
    Surf::Descriptors surf_descriptors;
    /** The SUPERPOINT descriptors */
    SuperPoint::Descriptors superpoint_descriptors;
private:
    void compute_sift (mve::ByteImage::ConstPtr image);
    void compute_surf (mve::ByteImage::ConstPtr image);
    void compute_superpoint(mve::ByteImage::ConstPtr image,std::string path);

private:
    Options opts;
};

/* ------------------------ Implementation ------------------------ */

inline
FeatureSet::Options::Options (void)
    : feature_types(FEATURE_SIFT)
{
}

inline
FeatureSet::FeatureSet (void)
{
}

inline
FeatureSet::FeatureSet (Options const& options)
    : opts(options)
{
}

inline void
FeatureSet::set_options (Options const& options)
{
    this->opts = options;
}

SFM_NAMESPACE_END

#endif /* SFM_FEATURE_SET_HEADER */
