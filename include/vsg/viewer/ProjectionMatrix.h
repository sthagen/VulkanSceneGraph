#pragma once

/* <editor-fold desc="MIT License">

Copyright(c) 2018 Robert Osfield

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

</editor-fold> */

#include <vsg/viewer/EllipsoidModel.h>
#include <vsg/viewer/ViewMatrix.h>

namespace vsg
{

    // Base class for projection matrices
    class ProjectionMatrix : public Inherit<Object, ProjectionMatrix>
    {
    public:
        virtual void get(mat4& matrix) const = 0;
        virtual void get(dmat4& matrix) const = 0;

        virtual void get_inverse(mat4& matrix) const
        {
            get(matrix);
            matrix = inverse(matrix);
        }
        virtual void get_inverse(dmat4& matrix) const
        {
            get(matrix);
            matrix = inverse(matrix);
        }
    };
    VSG_type_name(vsg::ProjectionMatrix);

    class Perspective : public Inherit<ProjectionMatrix, Perspective>
    {
    public:
        Perspective() :
            fieldOfViewY(60.0),
            aspectRatio(1.0),
            nearDistance(1.0),
            farDistance(10000.0)
        {
        }

        Perspective(double fov, double ar, double nd, double fd) :
            fieldOfViewY(fov),
            aspectRatio(ar),
            nearDistance(nd),
            farDistance(fd)
        {
        }

        void get(mat4& matrix) const override { matrix = perspective(radians(fieldOfViewY), aspectRatio, nearDistance, farDistance); }
        void get(dmat4& matrix) const override { matrix = perspective(radians(fieldOfViewY), aspectRatio, nearDistance, farDistance); }

        double fieldOfViewY;
        double aspectRatio;
        double nearDistance;
        double farDistance;
    };
    VSG_type_name(vsg::Perspective);

    class Orthographic : public Inherit<ProjectionMatrix, Orthographic>
    {
    public:
        Orthographic() :
            left(-1.0),
            right(1.0),
            bottom(-1.0),
            top(1.0),
            nearDistance(1.0),
            farDistance(10000.0)
        {
        }

        Orthographic(double l, double r, double b, double t, double nd, double fd) :
            left(l),
            right(r),
            bottom(b),
            top(t),
            nearDistance(nd),
            farDistance(fd)
        {
        }

        void get(mat4& matrix) const override { matrix = orthographic(left, right, bottom, top, nearDistance, farDistance); }
        void get(dmat4& matrix) const override { matrix = orthographic(left, right, bottom, top, nearDistance, farDistance); }

        double left;
        double right;
        double bottom;
        double top;
        double nearDistance;
        double farDistance;
    };
    VSG_type_name(vsg::Orthographic);

    class RelativeProjection : public Inherit<ProjectionMatrix, RelativeProjection>
    {
    public:
        RelativeProjection(ref_ptr<ProjectionMatrix> pm, const dmat4& m) :
            projectionMatrix(pm),
            matrix(m)
        {
        }

        void get(mat4& in_matrix) const override
        {
            projectionMatrix->get(in_matrix);
            in_matrix = mat4(matrix) * in_matrix;
        }

        void get(dmat4& in_matrix) const override
        {
            projectionMatrix->get(in_matrix);
            in_matrix = matrix * in_matrix;
        }

        ref_ptr<ProjectionMatrix> projectionMatrix;
        dmat4 matrix;
    };
    VSG_type_name(vsg::RelativeProjection);

    class EllipsoidPerspective : public Inherit<ProjectionMatrix, EllipsoidPerspective>
    {
    public:
        EllipsoidPerspective(ref_ptr<LookAt> la, ref_ptr<EllipsoidModel> em) :
            lookAt(la),
            ellipsoidModel(em),
            fieldOfViewY(60.0),
            aspectRatio(1.0),
            nearFarRatio(0.0001),
            horizonMountainHeight(1000.0)
        {
        }

        EllipsoidPerspective(ref_ptr<LookAt> la, ref_ptr<EllipsoidModel> em, double fov, double ar, double nfr, double hmh) :
            lookAt(la),
            ellipsoidModel(em),
            fieldOfViewY(fov),
            aspectRatio(ar),
            nearFarRatio(nfr),
            horizonMountainHeight(hmh)
        {
        }

        void get(mat4& matrix) const override
        {
            dmat4 dm;
            get(dm);
            matrix = dm;
        }
        void get(dmat4& matrix) const override
        {
            // std::cout<<"camera eye : "<<lookAt->eye<<", "<<ellipsoidModel->convertECEVToLatLongHeight(lookAt->eye)<<std::endl;
            vsg::dvec3 v = lookAt->eye;
            vsg::dvec3 lv = vsg::normalize(lookAt->center - lookAt->eye);
            double R = ellipsoidModel->radiusEquator();
            double H = ellipsoidModel->convertECEFToLatLongHeight(v).z;
            double D = R + H;

            double alpha = (D > R) ? std::acos(R / D) : 0.0;

            double beta_ratio = R / (R + horizonMountainHeight);
            double beta = beta_ratio < 1.0 ? std::acos(beta_ratio) : 0.0;

            double theta_ratio = -vsg::dot(lv, v) / (vsg::length(lv) * vsg::length(v));
            double theta = theta_ratio < 1.0 ? std::acos(theta_ratio) : 0.0;

            double l = R * (std::tan(alpha) + std::tan(beta));

            double farDistance = std::cos(theta + alpha - vsg::PI * 0.5) * l;
            double nearDistance = farDistance * nearFarRatio;
            //std::cout<<"H = "<<H<<", l = "<<l<<", theta = "<<vsg::degrees(theta)<<", fd = "<<farDistance<<std::endl;

            matrix = perspective(radians(fieldOfViewY), aspectRatio, nearDistance, farDistance);
        }

        ref_ptr<LookAt> lookAt;
        ref_ptr<EllipsoidModel> ellipsoidModel;

        double fieldOfViewY;
        double aspectRatio;
        double nearFarRatio;
        double horizonMountainHeight;
    };
    VSG_type_name(vsg::EllipsoidPerspective);

} // namespace vsg
