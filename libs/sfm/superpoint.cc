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
#include "python3.6m/Python.h"
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <mutex>
SFM_NAMESPACE_BEGIN

SuperPoint::SuperPoint(Options const &options)
    : options(options)
{

    if (this->options.debug_output)
        this->options.verbose_output = true;
}

/* ---------------------------------------------------------------- */

void SuperPoint::process(void)
{
    util::ClockTimer timer, total_timer;
    if (this->options.verbose_output)
    {
        std::cout << "SUPERPPOINT:Computing" << std::endl;
    }
    this->descriptor_generation();
}

/* ---------------------------------------------------------------- */

void SuperPoint::set_oncepath(std::string path)
{
    this->oncepath = path;
}
/*----------------------------------------------------------------- */
void SuperPoint::set_image(mve::ByteImage::ConstPtr img)
{
    if (img->channels() != 1 && img->channels() != 3)
        throw std::invalid_argument("Gray or color image expected");

    this->orig = mve::image::byte_to_float_image(img);
    if (img->channels() == 3)
    {
        this->orig = mve::image::desaturate<float>(this->orig, mve::image::DESATURATE_AVERAGE);
    }
}

/* ---------------------------------------------------------------- */

void SuperPoint::set_float_image(mve::FloatImage::ConstPtr img)
{
    if (img->channels() != 1 && img->channels() != 3)
        throw std::invalid_argument("Gray or color image expected");

    if (img->channels() == 3)
    {
        this->orig = mve::image::desaturate<float>(img, mve::image::DESATURATE_AVERAGE);
    }
    else
    {
        this->orig = img->duplicate();
    }
}
void SuperPoint::descriptor_generation(void)
{

    // Py_Initialize();
    //将要处理的图像名存入superpointtemp.txt
    std::string image = this->oncepath + "/original.jpg";
    // 找到要执行的python脚本并执行。
    std::string pypath = this->exec("sudo find ~ -name SuperPoint.py");
    char buffer[8192] = {0};
    pypath = pypath.substr(0, pypath.length() - 1);
    pypath += ' ';
    pypath += image;
    sprintf(buffer, "sudo python3 %s", pypath.c_str());
    system(buffer);
    sleep(50);
    // std::string resultpts = this->exec("sudo find ~ -name pts1.txt");
    // resultpts = resultpts.substr(0, resultpts.length() - 1);
    std::ifstream ifs("pts1.txt");
    if (!ifs) //如果不存在这个文件报错
    {
        perror("Can not open file:");
        std::cout << "open failure" << std::endl;
    }
    std::string line;
    Keypoint tkp;
    int flagxy = 0, kpnum = 0;

    while (std::getline(ifs, line))
    {
        if(flagxy == 2)break;
        std::stringstream ss(line);
        std::string token;
        while (ss >> token)
        {
            if (flagxy == 0)
            {
                tkp.x = std::atof(token.c_str());
                keypoints.push_back(tkp);
            }
            else
            {
                keypoints[kpnum].y = std::atof(token.c_str());
                kpnum += 1;
            }
        }
        flagxy += 1;
    }
    ifs.close();
    // std::string resultdesc = this->exec("sudo find ~ -name desc.txt");
    // resultdesc = resultdesc.substr(0, resultdesc.length() - 1);
    std::ifstream ifsd("desc.txt");
    if (!ifsd) //如果不存在这个文件报错
    {
        perror("Can not open file:");
        std::cout << "open failure" << std::endl;
    }
    Descriptor tdesc;
    int  descnum = 0,descindex=0;

    while (std::getline(ifsd, line))
    {   
        descindex = 0;
        std::stringstream ss(line);
        std::string token;
        while (ss >> token)
        {
            tdesc.data[descindex] = std::atof(token.c_str());
            descindex += 1;
        }
        tdesc.x = keypoints[descnum].x;
        tdesc.y = keypoints[descnum].y;
        tdesc.scale = 1;
        descriptors.push_back(tdesc);
        descnum+=1;
    }
    ifsd.close();
    assert(keypoints.size() == descriptors.size());
if(std::remove("pts1.txt")==0)
     {
         std::cout<<"tempptsfiledelsuccess"<<std::endl;        
     }
     else
     {
         std::cout<<"tempptsfiledelfail"<<std::endl;        
     }
if(std::remove("desc.txt")==0)
     {
         std::cout<<"tempdescfiledelsuccess"<<std::endl;        
     }
     else
     {
         std::cout<<"tempdescfiledelfail"<<std::endl;        
     }

    // Py_Finalize();
}

/* ---------------------------------------------------------------- */
std::string SuperPoint::exec(char *cmd)
{
    FILE *pipe = popen(cmd, "r");
    if (!pipe)
        return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}
SFM_NAMESPACE_END
