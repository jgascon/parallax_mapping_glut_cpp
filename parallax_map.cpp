/*
    Simple demo of parallax mapping

    Jorge Gascon Perez
*/

#include <stdio.h>
#include <stdlib.h>

#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GL/glut.h>
#include "textfile.h"
#include <iostream>

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

static float LIGHT_MOVE_OFFSET = 0.5;

using namespace std;

const string BASE_PATH = "../..";
GLuint program;
float light_pos[4] = {0.0, 0.0, 1.0, 0.0};
GLuint DiffuseMap;
GLuint NormalMap;
GLint lightLoc;
GLfloat depth_factor;
GLuint number_lod_iterations;

int global_last_x;
int global_last_y;

bool draw_wireframe = false;



void textured_quad(GLuint texture_descriptor0, GLuint texture_descriptor1) {

    GLint tangent_id = glGetAttribLocation(program, "external_tangent");
    if (tangent_id == GL_INVALID_OPERATION || tangent_id < 0) {
        cout << "ERROR at tangent --> " << tangent_id << "\n";
    }

    glColor4f (1.0, 1.0, 1.0, 1.0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,  texture_descriptor0);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,  texture_descriptor1);

    glPolygonMode(GL_FRONT, GL_FILL);

    glBegin(GL_TRIANGLE_FAN);
        glVertexAttrib3f(tangent_id, 1.0f, 0.0f, 0.0f);
        glMultiTexCoord2d(GL_TEXTURE1, 0.0f, 1.0f);
        glMultiTexCoord2d(GL_TEXTURE2, 0.0f, 1.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-2.0f, -2.0f, 0.0f);

        glVertexAttrib3f(tangent_id, 1.0f, 0.0f, 0.0f);
        glMultiTexCoord2d(GL_TEXTURE1, 1.0f, 1.0f);
        glMultiTexCoord2d(GL_TEXTURE2, 1.0f, 1.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(2.0f, -2.0f, 0.0f);

        glVertexAttrib3f(tangent_id, 1.0f, 0.0f, 0.0f);
        glMultiTexCoord2d(GL_TEXTURE1, 1.0f, 0.0f);
        glMultiTexCoord2d(GL_TEXTURE2, 1.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(2.0f, 2.0f, 0.0f);

        glVertexAttrib3f(tangent_id, 1.0f, 0.0f, 0.0f);
        glMultiTexCoord2d(GL_TEXTURE1, 0.0f, 0.0f);
        glMultiTexCoord2d(GL_TEXTURE2, 0.0f, 0.0f);
        glNormal3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-2.0f, 2.0f, 0.0f);
    glEnd();
}



void wireframe_quad() {
    glColor4f (1.0, 1.0, 1.0, 1.0);
    glLineWidth(2.0);

    glBegin(GL_LINE_STRIP);
        glVertex3f(-2.0f, -2.0f, 0.0f);
        glVertex3f(2.0f, -2.0f, 0.0f);
        glVertex3f(2.0f, 2.0f, 0.0f);
        glVertex3f(-2.0f, 2.0f, 0.0f);
        glVertex3f(-2.0f, -2.0f, 0.0f);
        glVertex3f(2.0f, 2.0f, 0.0f);
    glEnd();
    glLineWidth(1.0);
}



GLuint loadTexture(string filename) {
    SDL_Surface * surface;
    GLuint texture;

    if ( (surface = IMG_Load(filename.c_str())) ) {
        printf("Texture size: %dx%d\n",surface->w, surface->h);
        glGenTextures( 1, &texture );
        glBindTexture( GL_TEXTURE_2D, texture);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels );
        SDL_FreeSurface( surface );
        return texture;
    } else {
        printf("SDL could not load image: %s  ERROR: %s\n", filename.c_str(), SDL_GetError());
        //SDL_Quit();
        return 0;
    }
} //GLuint loadTexture(string filename)



void changeSize(int w, int h) {
    // Prevent a divide by zero, when window is too short
    // (you cant make a window of zero width).
    if(h == 0)
        h = 1;

    float ratio = 1.0f * w / h;

    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, ratio, 1, 1000);
    glViewport(0, 0, w, h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
} //void changeSize(int w, int h)



void renderScene(void) {
    static GLuint list_id = glGenLists(1);
    static bool is_clean = true;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (draw_wireframe) {
        glUseProgram(0);
        glDisable(GL_LIGHTING);
        glDisable(GL_TEXTURE_2D);
        wireframe_quad();
        glEnable(GL_TEXTURE_2D);
        glEnable(GL_LIGHTING);
        glutSwapBuffers();
        return;
    }

    if (is_clean) {
        glNewList(list_id, GL_COMPILE);
            textured_quad(NormalMap, DiffuseMap);
        glEndList();
        is_clean = false;
    };

    glUseProgram(program);
    glCallList(list_id);

    glPointSize(14.0);
    glEnable(GL_POINT_SMOOTH);
    glBegin(GL_POINTS);
        glVertex3fv(light_pos);
    glEnd();
    glPointSize(1.0);

    glutSwapBuffers();
} //void renderScene(void)



void processNormalKeys(unsigned char key, int x, int y) {
    switch (key){
        case 'w':
            light_pos[1] += LIGHT_MOVE_OFFSET;
            break;
        case 's':
            light_pos[1] -= LIGHT_MOVE_OFFSET;
            break;
        case 'a':
            light_pos[0] -= LIGHT_MOVE_OFFSET;
            break;
        case 'd':
            light_pos[0] += LIGHT_MOVE_OFFSET;
            break;
        case ' ':
            draw_wireframe = !draw_wireframe;
            cout << "Drawing wireframe: " << draw_wireframe << endl;
            break;
        case 27: //ESC
            exit(0);
            break;
    }
    printf("Light position (%f %f %f)\n",light_pos[0], light_pos[1], light_pos[2]);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glutPostRedisplay();
} //void processNormalKeys(unsigned char key, int x, int y)



void setShaders(string vertex_shader_filename, string fragment_shader_filename) {
    const GLsizei LogLength = 500;
    GLchar compilationLog[LogLength];
    GLsizei lengthObtained;

    GLuint v = glCreateShader(GL_VERTEX_SHADER);
    GLuint f = glCreateShader(GL_FRAGMENT_SHADER);

    char * vs = textFileRead(vertex_shader_filename.c_str());
    char * fs = textFileRead(fragment_shader_filename.c_str());
    const char * ff = fs;
    const char * vv = vs;
    glShaderSource(v, 1, &vv, NULL);
    glShaderSource(f, 1, &ff, NULL);
    free(vs);
    free(fs);

    glCompileShader(v);
    glGetShaderInfoLog(v, LogLength, &lengthObtained,  compilationLog);
    if (lengthObtained>0) { printf("Log del Vertex Shader \n %s\n", compilationLog); }

    glCompileShader(f);
    glGetShaderInfoLog(f, LogLength, &lengthObtained,  compilationLog);
    if (lengthObtained>0) { printf("Log del Fragment Shader \n %s\n", compilationLog); }
    program = glCreateProgram();
    glAttachShader(program,f);
    glAttachShader(program,v);

    glDeleteShader(f);
    glDeleteShader(v);

    glLinkProgram(program);
    glUseProgram(program);
} //void setShaders(string vertex_shader_filename, string fragment_shader_filename)



static void mouseMotion(int x, int y) {
    static float rotation_x = 0.0f;
    static float rotation_y = 0.0f;
    static float centre_distance = 5.0f;
    static float camera_right = 0.0f;
    static float camera_altitude = 0.0f;
    static float center_x = 0.0f;
    static float center_y = 0.0f;
    static float center_z = 0.0f;

    //Rotating the camera around our scene.
    rotation_x += (y - global_last_y) * 0.3f;
    rotation_y += (x - global_last_x) * 0.3f;

    if (rotation_y > 60.0f) {
        rotation_y = 60.0f;
    }
    if (rotation_y < -60.0f) {
        rotation_y = -60.0f;
    }

    if (rotation_x > 60.0f) {
        rotation_x = 60.0f;
    }
    if (rotation_x < -60.0f) {
        rotation_x = -60.0f;
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(-camera_right, 0.0, -centre_distance);
    glRotatef(rotation_x, 1, 0, 0);
    glRotatef(rotation_y, 0, 1, 0);
    glTranslatef(0.0, -camera_altitude, 0.0);
    glTranslatef(-center_x, -center_y, -center_z);

    global_last_x = x;
    global_last_y = y;
    glutPostRedisplay();
} //static void mouseMotion(int x, int y)



static void clickMouse(int button, int state, int x, int y) {
    global_last_x = x;
    global_last_y = y;
} //static void clickMouse(int button, int state, int x, int y)



int main(int argc, char **argv) {
    cout << "Starting" << endl;
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(500,500);
    glutCreateWindow("Parallax Mapping");

    GLenum err = glewInit();
    if (err != GLEW_OK) {
        cout << "ERROR: GLEW" << endl;
        exit(1);
    }

    glutDisplayFunc(renderScene);

    glutMouseFunc(clickMouse);
    glutMotionFunc(mouseMotion);

    //glutIdleFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutKeyboardFunc(processNormalKeys);
    //glutSpecialFunc(void (*func) (int key, int x, int y));

    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable( GL_BLEND );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glClearColor(0.0, 0.0, 0.0, 1.0);
    //glEnable(GL_CULL_FACE);

    NormalMap = loadTexture(BASE_PATH + "/images/earth_parallax.png");
    DiffuseMap = loadTexture(BASE_PATH + "/images/earth.png");

    setShaders(BASE_PATH + "/parallax_map.vert", BASE_PATH + "/parallax_map.frag");

    GLuint texLoc = glGetUniformLocation(program, "NormalMap");
    glUniform1i(texLoc, NormalMap);

    texLoc = glGetUniformLocation(program, "DiffuseMap");
    glUniform1i(texLoc, DiffuseMap);

    GLint shader_param = glGetUniformLocation(program, "depth_factor");
    glUniform1f(shader_param, 0.03f);

    shader_param = glGetUniformLocation(program, "number_lod_iterations");
    glUniform1i(shader_param, 16);

    lightLoc = glGetUniformLocation(program, "LightPosition");
    glUniform3f(lightLoc, 0.0, 0.0, 10.0);

    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );

    glScalef(0.5, 0.5, 0.5);

    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

    GLfloat ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
    GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.8f, 1.0f };
    GLfloat specularLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    glutMainLoop();

    return 0;
} //int main(int argc, char **argv)

