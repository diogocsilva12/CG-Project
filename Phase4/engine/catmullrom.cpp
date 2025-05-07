    #include "engine.h"
    #include <vector>
    #include <cmath>

    //AUX: multiply a 4x4 matrix by a 4x1 vector
    void multMatrixVector(const float* m, const float* v, float* res) {
        for (int j = 0; j < 4; j++) {
            res[j] = 0;
            for (int k = 0; k < 4; k++) {
                res[j] += m[j * 4 + k] * v[k];
            }
        }
    }

    //Catmull-Rom matrix
    static const float m[16] = {
        -0.5f,  1.5f, -1.5f,  0.5f,
        1.0f, -2.5f,  2.0f, -0.5f,
        -0.5f,  0.0f,  0.5f,  0.0f,
        0.0f,  1.0f,  0.0f,  0.0f
    };

    //Calculate a point and derivative on a Catmull-Rom segment
    void getCatmullRomPoint(float t, const Point& p0, const Point& p1, const Point& p2, const Point& p3, float* pos, float* deriv) {
        float T[4] = { t * t * t, t * t, t, 1 };
        float dT[4] = { 3 * t * t, 2 * t, 1, 0 };

        float px[4] = { p0.x, p1.x, p2.x, p3.x };
        float py[4] = { p0.y, p1.y, p2.y, p3.y };
        float pz[4] = { p0.z, p1.z, p2.z, p3.z };

        float ax[4], ay[4], az[4];
        multMatrixVector(m, px, ax);
        multMatrixVector(m, py, ay);
        multMatrixVector(m, pz, az);

        pos[0] = ax[0] * T[0] + ax[1] * T[1] + ax[2] * T[2] + ax[3] * T[3];
        pos[1] = ay[0] * T[0] + ay[1] * T[1] + ay[2] * T[2] + ay[3] * T[3];
        pos[2] = az[0] * T[0] + az[1] * T[1] + az[2] * T[2] + az[3] * T[3];

        deriv[0] = ax[0] * dT[0] + ax[1] * dT[1] + ax[2] * dT[2] + ax[3] * dT[3];
        deriv[1] = ay[0] * dT[0] + ay[1] * dT[1] + ay[2] * dT[2] + ay[3] * dT[3];
        deriv[2] = az[0] * dT[0] + az[1] * dT[1] + az[2] * dT[2] + az[3] * dT[3];
    }

    //Get global Catmull-Rom point
    void getGlobalCatmullRomPoint(float gt, const std::vector<Point>& points, float* pos, float* deriv) {
        int numPoints = points.size();
        float t = gt * numPoints;
        int index = (int)floor(t);
        t = t - index;

        int indices[4];
        indices[0] = (index + numPoints - 1) % numPoints;
        indices[1] = (index) % numPoints;
        indices[2] = (index + 1) % numPoints;
        indices[3] = (index + 2) % numPoints;

        getCatmullRomPoint(t, points[indices[0]], points[indices[1]], points[indices[2]], points[indices[3]], pos, deriv);
    }

    //Vector cross product
    void cross(const float* a, const float* b, float* res) {
        res[0] = a[1] * b[2] - a[2] * b[1];
        res[1] = a[2] * b[0] - a[0] * b[2];
        res[2] = a[0] * b[1] - a[1] * b[0];
    }

    //Vector normalization
    void normalize(float* v) {
        float len = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        if (len > 0.00001f) {
            v[0] /= len;
            v[1] /= len;
            v[2] /= len;
        }
    }

    //Build rotation matrix from 3 axes
    void buildRotMatrix(const float* x, const float* y, const float* z, float* m) {
        m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0.0f;
        m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0.0f;
        m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0.0f;
        m[12] = 0.0f; m[13] = 0.0f; m[14] = 0.0f; m[15] = 1.0f;
    }