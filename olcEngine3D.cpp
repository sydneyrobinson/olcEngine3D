
#include "olcConsoleGameEngine.h"
using namespace std; //good practice not to include this in header files

struct vec3d
{
    float x, y, z;
};

struct triangle
{
    vec3d p[3];

    wchar_t sym; // for colour and char combiationa
    short col; 
};

struct mesh
{
    vector<triangle> tris; // "std::vector" is a dynamic array (can resize self). contians triangle elements
};

struct mat4x4
{
    float m[4][4] = { 0 };
};

class olcEngine3D : public olcConsoleGameEngine // class that inherits from the olcConsoleGameEngine
{
public:
    olcEngine3D() //constructor
    {
        m_sAppName = L"3D Demo";
    }

private:
    mesh meshCube;
    mat4x4 matProj; // projection matrix

    vec3d vCamera; // position of camera in 3d space (simplification.. no direction information yet)

    float fTheta;

    void MultiplyMatrixVector(vec3d &i, vec3d &o, mat4x4 &m) // & gets memory address of i. o is the output
    {

        //implying that 4th element of input vector is 1, so only sum last elem here
        o.x = i.x * m.m[0][0] + i.y * m.m[1][0] + i.z * m.m[2][0] + m.m[3][0];
        o.y = i.x * m.m[0][1] + i.y * m.m[1][1] + i.z * m.m[2][1] + m.m[3][1];
        o.z = i.x * m.m[0][2] + i.y * m.m[1][2] + i.z * m.m[2][2] + m.m[3][2];
        float w = i.x * m.m[0][3] + i.y * m.m[1][3] + i.z * m.m[2][3] + m.m[3][3];

        if (w != 0.0f) // change back to 3d cartesian space
        {
            o.x /= w; o.y /= w; o.z /= w;
        }
    }

    // Taken From Command Line Webcam Video
    CHAR_INFO GetColour(float lum)
    {
        short bg_col, fg_col;
        wchar_t sym;
        int pixel_bw = (int)(13.0f * lum);
        switch (pixel_bw)
        {
        case 0: bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID; break;

        case 1: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_QUARTER; break;
        case 2: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_HALF; break;
        case 3: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_THREEQUARTERS; break;
        case 4: bg_col = BG_BLACK; fg_col = FG_DARK_GREY; sym = PIXEL_SOLID; break;

        case 5: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_QUARTER; break;
        case 6: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_HALF; break;
        case 7: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_THREEQUARTERS; break;
        case 8: bg_col = BG_DARK_GREY; fg_col = FG_GREY; sym = PIXEL_SOLID; break;

        case 9:  bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_QUARTER; break;
        case 10: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_HALF; break;
        case 11: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_THREEQUARTERS; break;
        case 12: bg_col = BG_GREY; fg_col = FG_WHITE; sym = PIXEL_SOLID; break;
        default:
            bg_col = BG_BLACK; fg_col = FG_BLACK; sym = PIXEL_SOLID;
        }

        CHAR_INFO c;
        c.Attributes = bg_col | fg_col;
        c.Char.UnicodeChar = sym;
        return c;
    }

public:
    bool OnUserCreate() override
    {
        meshCube.tris = { 
            // use doubly nested initializer list. inside=triangle w 3 vectors. outside=standard vector
            // see diagram in video at 14:00

            // SOUTH (face of cube)
            { 0.0f, 0.0f, 0.0f,     0.0f, 1.0f, 0.0f,   1.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 0.0f,     1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f },

            // EAST
            { 1.0f, 0.0f, 0.0f,     1.0f, 1.0f, 0.0f,   1.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 0.0f,     1.0f, 1.0f, 1.0f,   1.0f, 0.0f, 1.0f },

            // NORTH
            { 1.0f, 0.0f, 1.0f,     1.0f, 1.0f, 1.0f,   0.0f, 1.0f, 1.0f },
            { 1.0f, 0.0f, 1.0f,     0.0f, 1.0f, 1.0f,   0.0f, 0.0f, 1.0f },

            // WEST
            { 0.0f, 0.0f, 1.0f,     0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f },
            { 0.0f, 0.0f, 1.0f,     0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 0.0f },

            // TOP
            { 0.0f, 1.0f, 0.0f,     0.0f, 1.0f, 1.0f,   1.0f, 1.0f, 1.0f },
            { 0.0f, 1.0f, 0.0f,     1.0f, 1.0f, 1.0f,   1.0f, 1.0f, 0.0f },   
        
            // BOTTOM
            { 1.0f, 0.0f, 1.0f,     0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f },
            { 1.0f, 0.0f, 1.0f,     0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f },
        
        };

        // Projection Matrix
        float fNear = 0.1f;
        float fFar = 1000.0f;
        float fFov = 90.0f; // 90 degrees
        float fAspectRatio = (float)ScreenHeight() / (float)ScreenWidth(); //grab from console
        float fFovRad = 1.0f / tan(fFov * 0.5f / 180.0f * 3.14159f); // tangent calculation. convert from degrees to rads
            
        matProj.m[0][0] = fAspectRatio * fFovRad;
        matProj.m[1][1] = fFovRad;
        matProj.m[2][2] = fFar / (fFar - fNear);
        matProj.m[3][2] = (-fFar * fNear) / (fFar - fNear);
        matProj.m[2][3] = 1.0f;
        matProj.m[3][3] = 0.0f;

        return true;
    }

    bool OnUserUpdate(float fElapsedTime) override
    {
        // Clear screen
        Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

        // set up roation matrices
        mat4x4 matRotZ, matRotX;
        fTheta += 1.0f * fElapsedTime;

        // Rotation Z
        matRotZ.m[0][0] = cosf(fTheta);
        matRotZ.m[0][1] = sinf(fTheta);
        matRotZ.m[1][0] = -sinf(fTheta);
        matRotZ.m[1][1] = cosf(fTheta);
        matRotZ.m[2][2] = 1;
        matRotZ.m[3][3] = 1;

        // Rotation X
        matRotX.m[0][0] = 1;
        matRotX.m[1][1] = cosf(fTheta * 0.5f);
        matRotX.m[1][2] = sinf(fTheta * 0.5f);
        matRotX.m[2][1] = -sinf(fTheta * 0.5f);
        matRotX.m[2][2] = cosf(fTheta * 0.5f);
        matRotX.m[3][3] = 1;

        

        // Draw triangles. contained inside a vector inside a mesh
        for (auto tri : meshCube.tris)
        {
            triangle triProjected, triTranslated,triRotatedZ, triRotatedZX; 

            // Rotate in Z-Axis
            MultiplyMatrixVector(tri.p[0], triRotatedZ.p[0], matRotZ);
            MultiplyMatrixVector(tri.p[1], triRotatedZ.p[1], matRotZ);
            MultiplyMatrixVector(tri.p[2], triRotatedZ.p[2], matRotZ);

            // rotate in x-axis
            MultiplyMatrixVector(triRotatedZ.p[0], triRotatedZX.p[0], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[1], triRotatedZX.p[1], matRotX);
            MultiplyMatrixVector(triRotatedZ.p[2], triRotatedZX.p[2], matRotX);

            // Offset into the screen
            triTranslated = triRotatedZX;
            triTranslated.p[0].z = triRotatedZX.p[0].z + 3.0f;
            triTranslated.p[1].z = triRotatedZX.p[1].z + 3.0f;
            triTranslated.p[2].z = triRotatedZX.p[2].z + 3.0f;

            //after translated triangle into worldspace, but before any projection
            // use Cross-Product to get NORMALS
            vec3d normal, line1, line2;
            line1.x = triTranslated.p[1].x - triTranslated.p[0].x; //subtracting the origin of the triangle from a point to get line for each dimension
            line1.y = triTranslated.p[1].y - triTranslated.p[0].y;
            line1.z = triTranslated.p[1].z - triTranslated.p[0].z;

            line2.x = triTranslated.p[2].x - triTranslated.p[0].x; 
            line2.y = triTranslated.p[2].y - triTranslated.p[0].y;
            line2.z = triTranslated.p[2].z - triTranslated.p[0].z;

            normal.x = line1.y * line2.z - line1.z * line2.y;
            normal.y = line1.z * line2.x - line1.x * line2.z;
            normal.z = line1.x * line2.y - line1.y * line2.x;

            // normalizing normal to unit vector
            float l = sqrtf(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z); // length of normal using pythagorean theorem
            normal.x /= l; normal.y /= l; normal.z /= l; 

            //if (normal.z < 0)
            if (normal.x * (triTranslated.p[0].x - vCamera.x) +  // calc dot product between line from camera to triangle and the normal
                normal.y * (triTranslated.p[0].y - vCamera.y) +
                normal.z * (triTranslated.p[0].z - vCamera.z) < 0.0f) // can take any point from triTranslated because they lie in the same plane
                // do opposite, then cube is open box!
            {
                // Illumination
                vec3d light_direction = { 0.0f, 0.0f, -1.0f };
                float l = sqrtf(light_direction.x * light_direction.x + light_direction.y * light_direction.y + light_direction.z * light_direction.z);
                light_direction.x /= l; light_direction.y /= l; light_direction.z /= l;

                // How similar is normal to light direction
                float dp = normal.x * light_direction.x + normal.y * light_direction.y + normal.z * light_direction.z;
               
                CHAR_INFO c = GetColour(dp); // console-specific stuff
                triTranslated.col = c.Attributes;
                triTranslated.sym = c.Char.UnicodeChar;



                // PROJECT triangles from 3D to 2D
                MultiplyMatrixVector(triTranslated.p[0], triProjected.p[0], matProj); // reference each point
                MultiplyMatrixVector(triTranslated.p[1], triProjected.p[1], matProj);
                MultiplyMatrixVector(triTranslated.p[2], triProjected.p[2], matProj);

                triProjected.col = triTranslated.col; // carry colour info into projection
                triProjected.sym = triTranslated.sym;

                // Scale into view
                triProjected.p[0].x += 1.0f; triProjected.p[0].y += 1.0f; // between 0 and 2
                triProjected.p[1].x += 1.0f; triProjected.p[1].y += 1.0f;
                triProjected.p[2].x += 1.0f; triProjected.p[2].y += 1.0f;
                triProjected.p[0].x *= 0.5f * (float)ScreenWidth();
                triProjected.p[0].y *= 0.5f * (float)ScreenHeight();
                triProjected.p[1].x *= 0.5f * (float)ScreenWidth();
                triProjected.p[1].y *= 0.5f * (float)ScreenHeight();
                triProjected.p[2].x *= 0.5f * (float)ScreenWidth();
                triProjected.p[2].y *= 0.5f * (float)ScreenHeight();


                // rasterize the triangle
                FillTriangle(triProjected.p[0].x, triProjected.p[0].y,
                    triProjected.p[1].x, triProjected.p[1].y,
                    triProjected.p[2].x, triProjected.p[2].y,
                    triProjected.sym, triProjected.col);

                /*DrawTriangle(triProjected.p[0].x, triProjected.p[0].y,
                    triProjected.p[1].x, triProjected.p[1].y,
                    triProjected.p[2].x, triProjected.p[2].y,
                    PIXEL_SOLID, FG_WHITE);
                */
            }
            
               
        }

        return true;
    }
};

int main()
{
    olcEngine3D demo;
    if (demo.ConstructConsole(256, 240, 4, 4))
        demo.Start(); //do error message if does not construct
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file




// finish tutorial, debug. see whats happening on screen.