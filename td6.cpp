#include <iostream>
#include <vector>
#include <array>
#include <fstream>

#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>
 
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut.h>
#endif

#define concat(first, second) first second

#include "config.h"
#include "GLError.h"
#include "repere.h"

#define ENABLE_SHADERS

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

repere rep(1.0);

unsigned int progid;
unsigned int mvpid;
unsigned int modelid;
unsigned int viewid;
unsigned int projid;

// Matrices 4x4 contenant les transformations.
glm::mat4 model;
glm::mat4 view;
glm::mat4 proj;
glm::mat4 mvp;

// Variables globales pour stocker les angles de rotation
float angleX = 0.0f;
float angleY = 0.0f;

float angle = 0.0f;
float scale = 0.0f;
float inc = 0.1f;

unsigned int vaoids[ 1 ];

unsigned int nbtriangles;

float x, y, z;

std::array< float, 3 > eye = { 0.0f, 0.0f, 5.0f };


void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    // Positionnement de la caméra en ( 0.0f, 0.0f, 5.0f ),
    // on regarde en direction du point ( 0.0f, 0.0f, 0.0f ),
    // la tête est orienté suivant vers le haut l'axe y ( 0.0f, 1.0f, 0.0f ).
    view = glm::lookAt(glm::vec3(eye[0], eye[1], eye[2]), glm::vec3(eye[0], eye[1], eye[2] - 1.0), glm::vec3(0.0f, 1.0f, 0.0f));

    // initialisation de la matrice de modelisation
    model = glm::mat4(1.0f);

    // On recale le maillage à l'origine du repère
    model = glm::translate(glm::mat4(1.0f), glm::vec3(-x, -y, -z)) * model;

    // Le modèle est mis à l'échelle
    model = glm::scale(glm::mat4(1.0f), glm::vec3(scale * 3.5)) * model;

    // Appliquer les rotations suivant les axes X et Y
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f)) * model;
    model = glm::rotate(glm::mat4(1.0f), glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f)) * model;

    // Calcul de la matrice mvp.
    mvp = proj * view * model;

    // tester aussi:  (quelle différence?)
    //rep.trace_repere(proj*  view *rot);
    rep.trace_repere(proj * view);

    glUseProgram(progid); // Choix du shader à appliquer.

    glUniformMatrix4fv(mvpid, 1, GL_FALSE, &mvp[0][0]); // Passage de la matrice mvp au shader.
    glUniformMatrix4fv(modelid, 1, GL_FALSE, &model[0][0]); // Passage de la matrice model au shader.
    glUniformMatrix4fv(viewid, 1, GL_FALSE, &view[0][0]); // Passage de la matrice view au shader.
    glUniformMatrix4fv(projid, 1, GL_FALSE, &proj[0][0]); // Passage de la matrice proj au shader.

    glBindVertexArray(vaoids[0]); // Choix du vao

    glDrawElements(GL_TRIANGLES, nbtriangles * 3, GL_UNSIGNED_INT, 0);

    check_gl_error(); // pour le debugage d'openGL

    glutSwapBuffers();
}


void idle()
{
    angle += 0.0001f;
    if( angle >= 360.0f )
    {
        angle = 0.0f;
    }

//    if( scale <= 0.0f )
//    {
//        inc = 0.1f;
//    }
//    else if( scale > 2.0f )
//    {
//        inc = -0.1f;
//    }
//
//    scale += inc;

    glutPostRedisplay();
}


void reshape( int w, int h )
{
    glViewport(0, 0, w, h);
    // Modification de la matrice de projection à chaque redimensionnement de la fenêtre.
    proj = glm::perspective( 45.0f, w/static_cast< float >( h ), 0.1f, 100.0f );
}


void special( int key, int x, int y )
{
    switch( key )
    {
        case GLUT_KEY_LEFT:
            eye[ 0 ] -= 0.1f;
            break;
        case GLUT_KEY_RIGHT:
            eye[ 0 ] += 0.1f;
            break;
        case GLUT_KEY_UP:
            eye[ 2 ] -= 0.1f;
            break;
        case GLUT_KEY_DOWN:
            eye[ 2 ] += 0.1f;
            break;
    }
    glutPostRedisplay();
}

// Fonction de gestion des touches normales
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 'd':
            angleY += 5.0f;
            break;
        case 'q':
            angleY -= 5.0f;
            break;
        case 'z':
            angleX -= 5.0f;
            break;
        case 's':
            angleX += 5.0f;
            break;
    }
    glutPostRedisplay();
}


void initVAOs()
{
    unsigned int vboids[ 4 ];

    std::ifstream ifs( concat(MY_SHADER_PATH, "/meshes/milleniumfalcon.off/milleniumfalcon.off" ));
    if (!ifs)
    {
        throw std::runtime_error("can't find the meshe!! Check the name and the path of this file? ");
    }

    
    std::string off;

    unsigned int nbpoints, tmp;

    ifs >> off;
    ifs >> nbpoints;
    ifs >> nbtriangles;
    ifs >> tmp;

    std::cout<<nbtriangles;
    std::vector< float > vertices( nbpoints * 3 );
    std::vector< float > colors( nbpoints * 3 );
    std::vector< unsigned int > indices( nbtriangles *3 );
    std::vector< float > normals( nbpoints * 3 );

    //std::fill( std::begin( normals ), std::end( normals ), 0.0f );

    for( unsigned int i = 0 ; i < vertices.size() ; ++i) {
        ifs >> vertices[ i ];

    }

    for( unsigned int i = 0 ; i < nbtriangles ; ++i) {
        ifs >> tmp;
        ifs >> indices[ i * 3 ];
        ifs >> indices[ i * 3 + 1 ];
        ifs >> indices[ i * 3 + 2 ];

    }

    /**
     * Calcul de la boîte englobante du modèle
     */
    float dx, dy, dz;
    float xmin, xmax, ymin, ymax, zmin, zmax;

    xmin = xmax = vertices[0];
    ymin = ymax = vertices[1];
    zmin = zmax = vertices[2];
    for(unsigned int i = 1 ; i < nbpoints ; ++i) {
        if(xmin > vertices[i*3]) xmin = vertices[i*3];
        if(xmax < vertices[i*3]) xmax = vertices[i*3];
        if(ymin > vertices[i*3+1]) ymin = vertices[i*3+1];
        if(ymax < vertices[i*3+1]) ymax = vertices[i*3+1];
        if(zmin > vertices[i*3+2]) zmin = vertices[i*3+2];
        if(zmax < vertices[i*3+2]) zmax = vertices[i*3+2];
    }

    // calcul du centre de la boîte englobante

    x = (xmax + xmin)/2.0f;
    y = (ymax + ymin)/2.0f;
    z = (zmax + zmin)/2.0f;

    // calcul des dimensions de la boîte englobante

    dx = xmax - xmin;
    dy = ymax - ymin;
    dz = zmax - zmin;

    // calcul du coefficient de mise à l'échelle

    scale = 1.0f/fmax(dx, fmax(dy, dz));
std::cout<<scale;

    // Calcul des normales.
    for( std::size_t i = 0 ; i < indices.size() ; i+=3 )
    {
        auto x0 = vertices[ 3 * indices [ i ]     ] - vertices[ 3 * indices [ i+1 ]     ];
        auto y0 = vertices[ 3 * indices [ i ] + 1 ] - vertices[ 3 * indices [ i+1 ] + 1 ];
        auto z0 = vertices[ 3 * indices [ i ] + 2 ] - vertices[ 3 * indices [ i+1 ] + 2 ];

        auto x1 = vertices[ 3 * indices [ i ]     ] - vertices[ 3 * indices [ i+2 ]     ];
        auto y1 = vertices[ 3 * indices [ i ] + 1 ] - vertices[ 3 * indices [ i+2 ] + 1 ];
        auto z1 = vertices[ 3 * indices [ i ] + 2 ] - vertices[ 3 * indices [ i+2 ] + 2 ];

        auto x01 = y0 * z1 - y1 * z0;
        auto y01 = z0 * x1 - z1 * x0;
        auto z01 = x0 * y1 - x1 * y0;

        auto norminv = 1.0f / std::sqrt( x01 * x01 + y01 * y01 + z01 * z01 );
        x01 *= norminv;
        y01 *= norminv;
        z01 *= norminv;

        normals[ 3 * indices[ i ]     ] += x01;
        normals[ 3 * indices[ i ] + 1 ] += y01;
        normals[ 3 * indices[ i ] + 2 ] += z01;

        normals[ 3 * indices[ i + 1 ]     ] += x01;
        normals[ 3 * indices[ i + 1 ] + 1 ] += y01;
        normals[ 3 * indices[ i + 1 ] + 2 ] += z01;

        normals[ 3 * indices[ i + 2 ]     ] += x01;
        normals[ 3 * indices[ i + 2 ] + 1 ] += y01;
        normals[ 3 * indices[ i + 2 ] + 2 ] += z01;
    }

    for( std::size_t i = 0 ; i < normals.size() ; i+=3 )
    {
        auto & x = normals[ i     ];
        auto & y = normals[ i + 1 ];
        auto & z = normals[ i + 2 ];

        auto norminv = 1.0f / std::sqrt( x * x + y * y + z * z );

        x *= norminv;
        y *= norminv;
        z *= norminv;
    }

check_gl_error();
    // Génération d'un Vertex Array Object contenant 3 Vertex Buffer Objects.
    glGenVertexArrays( 1, &vaoids[ 0 ] );
    glBindVertexArray( vaoids[ 0 ] );

    // Génération de 4 VBO.
    glGenBuffers( 4, vboids );

    // VBO contenant les sommets.

    glBindBuffer( GL_ARRAY_BUFFER, vboids[ 0 ] );
    glBufferData( GL_ARRAY_BUFFER, vertices.size() * sizeof( float ), vertices.data(), GL_STATIC_DRAW );

    // L'attribut in_pos du vertex shader est associé aux données de ce VBO.
    auto pos = glGetAttribLocation( progid, "in_pos" );
    glVertexAttribPointer( pos, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( pos );
check_gl_error();
    // VBO contenant les couleurs.
/*
    glBindBuffer( GL_ARRAY_BUFFER, vboids[ 1 ] );
    glBufferData( GL_ARRAY_BUFFER, colors.size() * sizeof( float ), colors.data(), GL_STATIC_DRAW );

    // L'attribut in_color du vertex shader est associé aux données de ce VBO.
    auto color = glGetAttribLocation( progid, "in_color" );
    glVertexAttribPointer( color, 3, GL_FLOAT, GL_FALSE, 0, 0 );
    glEnableVertexAttribArray( color );
*/
 check_gl_error();
    // VBO contenant les indices.

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vboids[ 2 ] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof( unsigned int ), indices.data(), GL_STATIC_DRAW );
check_gl_error();
    // VBO contenant les normales.

   glBindBuffer( GL_ARRAY_BUFFER, vboids[ 3 ] );
    glBufferData( GL_ARRAY_BUFFER, normals.size() * sizeof( float ), normals.data(), GL_STATIC_DRAW );

    auto normal = glGetAttribLocation( progid, "in_normal" );
    glVertexAttribPointer( normal, 3, GL_FLOAT, GL_TRUE, 0, 0 );
    glEnableVertexAttribArray( normal );
check_gl_error();
    glBindVertexArray( 0 );
}


void initShaders()
{
    unsigned int vsid, fsid;
    int status;
    int logsize;
    std::string log;

   std::ifstream vs_ifs( concat(MY_SHADER_PATH, "/shaders/basic.vert.glsl") );
   std::ifstream fs_ifs( concat(MY_SHADER_PATH, "/shaders/basic.frag.glsl") );



    auto begin = vs_ifs.tellg();
    vs_ifs.seekg( 0, std::ios::end );
    auto end = vs_ifs.tellg();
    vs_ifs.seekg( 0, std::ios::beg );
    auto size = end - begin;

    std::string vs;
    vs.resize( size );
    vs_ifs.read( &vs[ 0 ], size );

    begin = fs_ifs.tellg();
    fs_ifs.seekg( 0, std::ios::end );
    end = fs_ifs.tellg();
    fs_ifs.seekg( 0, std::ios::beg );
    size = end - begin;

    std::string fs;
    fs.resize( size );
    fs_ifs.read( &fs[0], size );

    vsid = glCreateShader( GL_VERTEX_SHADER );
    char const * vs_char = vs.c_str();
    glShaderSource( vsid, 1, &vs_char, nullptr );
    glCompileShader( vsid );

    // Get shader compilation status.
    glGetShaderiv( vsid, GL_COMPILE_STATUS, &status );

    if( !status )
    {
        std::cerr << "Error: vertex shader compilation failed.\n";
        glGetShaderiv( vsid, GL_INFO_LOG_LENGTH, &logsize );
        log.resize( logsize );
        glGetShaderInfoLog( vsid, log.size(), &logsize, &log[0] );
        std::cerr << log << std::endl;
    }

    fsid = glCreateShader( GL_FRAGMENT_SHADER );
    char const * fs_char = fs.c_str();
    glShaderSource( fsid, 1, &fs_char, nullptr );
    glCompileShader( fsid );

    // Get shader compilation status.
    glGetShaderiv( fsid, GL_COMPILE_STATUS, &status );

    if( !status )
    {
        std::cerr << "Error: fragment shader compilation failed.\n";
        glGetShaderiv( fsid, GL_INFO_LOG_LENGTH, &logsize );
        log.resize( logsize );
        glGetShaderInfoLog( fsid, log.size(), &logsize, &log[0] );
        std::cerr << log << std::endl;
    }

    progid = glCreateProgram();

    glAttachShader( progid, vsid );
    glAttachShader( progid, fsid );

    glLinkProgram( progid );

    glUseProgram( progid );

    mvpid = glGetUniformLocation( progid, "mvp" );
    modelid = glGetUniformLocation( progid, "model" );
    viewid = glGetUniformLocation( progid, "view" );
    projid = glGetUniformLocation( progid, "proj" );
}


int main( int argc, char * argv[] )
{
    glutInit( &argc, argv );
#if defined(__APPLE__) && defined(ENABLE_SHADERS)
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA|GLUT_3_2_CORE_PROFILE);
#else

glutInitContextVersion( 3, 2 );
//glutInitContextVersion( 4, 5 );
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glewInit();
#endif

    glutInitWindowSize( 640, 480 );


    glutCreateWindow( argv[ 0 ]  );

    glutDisplayFunc( display );
    glutReshapeFunc( reshape );
    //glutIdleFunc( idle );
    glutSpecialFunc( special );
    glutKeyboardFunc(keyboard);

    // Initialisation de la bibliothèque GLEW.
#if not defined(__APPLE__)
    glewInit();
#endif

    glEnable(GL_DEPTH_TEST);
check_gl_error();
    initShaders();
    check_gl_error();
    initVAOs();
    check_gl_error();
    rep.init();
    check_gl_error();

    glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );

    glutMainLoop();

    return 0;
}
