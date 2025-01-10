/*
 *  repere.h
 *  glutMaster
 *
 *  Created by Emmanuel Melin on 19/01/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */
#include <iostream>
#include <vector>
#include <array>
#include <fstream>

#ifndef config_h
#define config_h

#if defined(__APPLE__)
#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <GLUT/glut.h>

#else
#include <GL/glew.h>
#include <GL/glut.h>

#endif

#endif
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
class repere{
	
private:
	
		
  float taille;
	
    unsigned int rep_vaoids[1];
    void initVAOs();
    void initShaders();
    unsigned int rep_progid;
    unsigned int  rep_mvpid;
    
public:
	repere(float t);
    void init();
	~repere();
	
	void trace_repere(glm::mat4 m);
};
