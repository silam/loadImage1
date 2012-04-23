



































































/*
 *Skeleton lighting program
 *COS490
 *Fall 2011
 **/


#include <stdio.h>
#include <GL/Angel.h>
#include <stdlib.h>
#include <math.h>
#include <IL/il.h> //notice the OpenIL include
#include "matrix_stack.h"

#pragma comment(lib, "glew32.lib")
//We have additional libraries to link to now as well
#pragma comment(lib,"ILUT.lib")
#pragma comment(lib,"DevIL.lib")
#pragma comment(lib,"ILU.lib")

//store window width and height
int ww=500, wh=500;

#define M_PI 3.14159265358979323846

matrix_stack stack;

GLuint programAllFeatures, programColor, programSpec, programNorm, programCloud;

//GLuint vao[1];
//GLuint vbo[2];
int spherevertcount;
int mode = 0;

int right_button_down = FALSE;
int left_button_down = FALSE;

int prevMouseX;
int prevMouseY;

double view_rotx = 0.0;
double view_roty = 0.0;
double view_rotz = 0.0;
double z_distance;

int rotateYEarth;
int rotateXEarth;
float rotateZEarth;

//our modelview and perspective matrices
mat4 mv, p;

//and we'll need pointers to our shader variables
GLuint model_view;
GLuint projection;
GLuint vPosition;
GLuint vAmbientDiffuseColor;
GLuint vSpecularColor;
GLuint vSpecularExponent;
GLuint vNormal;
GLuint light_position;
GLuint light_color;
GLuint ambient_light;


//We need three texture files
static GLuint texName[5];

GLuint texCoord;
GLuint texMap, cloudMap;
GLuint normalMap;
GLuint specMap;
GLuint nightMap;

vec4* sphere_verts;
vec3* sphere_normals;

// EARTH 
GLuint * spherevao;
GLuint * spherevbo;
GLuint * spherenormals;


GLuint * cloudvao;
GLuint * cloudvbo;
GLuint * cloudnormals;


/////// EARTH
int space = 10; //10; // 259200
int VertexCount = (180 / space) * (360 / space) * 4; // 2592


//Modified slightly from the OpenIL tutorials
ILuint loadTexFile(const char* filename){
	
	ILboolean success; /* ILboolean is type similar to GLboolean and can equal GL_FALSE (0) or GL_TRUE (1)
    it can have different value (because it's just typedef of unsigned char), but this sould be
    avoided.
    Variable success will be used to determine if some function returned success or failure. */


	/* Before calling ilInit() version should be checked. */
	  if (ilGetInteger(IL_VERSION_NUM) < IL_VERSION)
	  {
		/* wrong DevIL version */
		printf("Wrong IL version");
		exit(1);
	  }
 
	  success = ilLoadImage((LPTSTR)filename); /* Loading of image from file */
	if (success){ /* If no error occured: */
		//We need to figure out whether we have an alpha channel or not
		  if(ilGetInteger(IL_IMAGE_BPP) == 3){
			success = ilConvertImage(IL_RGB, IL_UNSIGNED_BYTE); /* Convert every color component into
		  unsigned byte. If your image contains alpha channel you can replace IL_RGB with IL_RGBA */
		  }else if(ilGetInteger(IL_IMAGE_BPP) == 4){
			  success = ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
		  }else{
			  success = false;
		  }
		if (!success){
		  /* Error occured */
		 printf("failed conversion to unsigned byte");
		 exit(1);
		}
	}else{
		/* Error occured */
	   printf("Failed to load image ");
	   printf(filename);
		exit(1);
	}
}

/////////////////////////////////////////
// myIdle
/////////////////////////////////////////
void myIdle()
{
	rotateZEarth = rotateZEarth + 0.1;

	
	rotateXEarth++;

	glutPostRedisplay();

}
/////////////////////////////////////////
// Create Sphere
/////////////////////////////////////////
void CreateSphere (vec4 verts[2592], vec2 texcoords[2592], vec3 normals[2592], vec4 tangents[2592], double R, double H, double K, double Z) {
    
	int n;
    double a;
    double b;
    n = 0;
    
	for( b = 0; b <= 180 - space; b+=space){
       
		for( a = 0; a <= 360 - space; a+=space){
            
			verts[n] = vec4( R * sin((a) / 180 * M_PI) * sin((b) / 180 * M_PI) - H,
								   R * cos((a) / 180 * M_PI) * sin((b) / 180 * M_PI) + K,
								   R * cos((b) / 180 * M_PI) - Z, 
								   1);
            texcoords[n] = vec2((a) / 360 , (2 * b) / 360);
            
			normals[n] = vec3(verts[n].x, verts[n].y, verts[n].z);
			

			n++;
            
			verts[n] = vec4(R * sin((a) / 180 * M_PI) * sin((b + space) / 180 * M_PI) - H,
					             R * cos((a) / 180 * M_PI) * sin((b + space) / 180 * M_PI) + K,
								 R * cos((b + space) / 180 * M_PI) - Z,
								 1);

			texcoords[n] = vec2((a) / 360 , 
							(2 *  (b + space)) / 360);
            
            
			normals[n] = vec3(verts[n].x, verts[n].y, verts[n].z);

			n++;
            

            
			verts[n] = vec4(R * sin((a + space) / 180 * M_PI) * sin((b) / 180 * M_PI) - H,
								R * cos((a + space) / 180 * M_PI) * sin((b) / 180 * M_PI) + K,
								R * cos((b) / 180 * M_PI) - Z,
								1);
         
			texcoords[n] = vec2((a + space) / 360 ,  (2 *  (b)) / 360 );


			normals[n] = vec3(verts[n].x, verts[n].y, verts[n].z);

			n++;
            
			verts[n] = vec4( R * sin((a + space) / 180 * M_PI) * sin((b + space) /180 * M_PI) - H,
									R * cos((a + space) / 180 * M_PI) * sin((b + space) / 180 * M_PI) + K,
									 R * cos((b + space) / 180 * M_PI) - Z,
									 1);
            
			texcoords[n] = vec2((a + space) / 360, (2 *  (b + space)) / 360);

			normals[n] = vec3(verts[n].x, verts[n].y, verts[n].z);

			n++;
            

            
		}
    
	}

}

///////////////////////////////
// Reshape image
//////////////////////////////
void reshape(int width, int height){
	ww= width;
	wh = height;
	//field of view angle, aspect ratio, closest distance from camera to object, largest distanec from camera to object
	p = Perspective(45.0, (float)width/(float)height, 1.0, 100.0);
	glUniformMatrix4fv(projection, 1, GL_TRUE, p);
	glViewport( 0, 0, width, height );
}


//In this particular case, our normal vectors and vertex vectors are identical since the sphere is centered at the origin
//For most objects this won't be the case, so I'm treating them as separate values for that reason
//This could also be done as separate triangle strips, but I've chosen to make them just triangles so I don't have to execute multiple glDrawArrays() commands
int generateSphere(float radius, int subdiv){
	float step = (360.0/subdiv)*(M_PI/180.0);

	int totalverts = ceil(subdiv/2.0)*subdiv * 6;

	if(sphere_normals){
		delete[] sphere_normals;
	}
	sphere_normals = new vec3[totalverts];
	if(sphere_verts){
		delete[] sphere_verts;
	}
	sphere_verts = new vec4[totalverts];

	int k = 0;
	for(float i = -M_PI/2; i<=M_PI/2; i+=step){
		for(float j = -M_PI; j<=M_PI; j+=step){
			//triangle 1
			sphere_normals[k]= vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			k++;
	
			sphere_normals[k]= vec3(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i+step), radius*cos(j)*cos(i+step), radius*sin(i+step), 1.0);
			k++;
			
			sphere_normals[k]= vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			k++;

			//triangle 2
			sphere_normals[k]= vec3(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step));
			sphere_verts[k]=   vec4(radius*sin((j+step))*cos((i+step)), radius*cos(j+step)*cos(i+step), radius*sin(i+step), 1.0);
			k++;

			sphere_normals[k]= vec3(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j+step)*cos(i), radius*cos(j+step)*cos(i), radius*sin(i), 1.0);
			k++;

			sphere_normals[k]= vec3(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i));
			sphere_verts[k]=   vec4(radius*sin(j)*cos(i), radius*cos(j)*cos(i), radius*sin(i), 1.0);
			k++;
		}
	}
	return totalverts;
}
/////////////////////////////////////////
// Display
/////////////////////////////////////////
void display(void)
{
  /*clear all pixels*/
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    mv = LookAt(vec4(0, 0, 10+z_distance, 1.0), vec4(0, 0, 0, 1.0), vec4(0, 1, 0, 0.0)) * RotateX(-90) * RotateZ(rotateZEarth);

	mv = mv *  RotateY(view_roty) ;//RotateZ(view_rotz); //RotateX(view_rotx)
	

	//////////////////////////////
	// Display all features
	/////////////////////////////

	if ( true )
	{
	glUseProgram(programAllFeatures);
	
	stack.push(mv);
	mv = mv*Scale(2,2,2);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	
	
	glVertexAttrib4fv(vAmbientDiffuseColor, vec4(.7, 0.7, 0.7, 1));
	glVertexAttrib4fv(vSpecularColor, vec4(1.0f,1.0f,1.0f,1.0f));
	glVertexAttrib1f(vSpecularExponent, 50.0);
	glUniform4fv(light_position, 1, vec4(50.0, 0.0, 0.0, 1));
	glUniform4fv(light_color, 1, vec4(1,1,1,1));
	glUniform4fv(ambient_light, 1, vec4(.5, .5, .5, 5));

	
	// texture unit 0 : color earth
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texName[0]); //which texture do we want?
	glDrawArrays( GL_QUAD_STRIP, 0, VertexCount );
	mv = stack.pop();

	// texture unit 1 : cloud earth

	//stack.push(mv);
	//mv = mv*Scale(2.05,2.05,2.05);
	//glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, texName[1]); //which texture do we want?
	//glDrawArrays( GL_QUAD_STRIP, 0, VertexCount );
	//mv = stack.pop();


	stack.push(mv);
	// texture unit 2 : specular earth
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, texName[2]); //which texture do we want?
	glDrawArrays( GL_QUAD_STRIP, 0, VertexCount );


	// texture unit 4 : night earth
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, texName[4]); //which texture do we want?
	glDrawArrays( GL_QUAD_STRIP, 0, VertexCount );
	mv = stack.pop();

	}

	//////////////////////////////
	// Display cloud
	/////////////////////////////

	if ( true )
	{
	glUseProgram(programCloud);
	glActiveTexture(GL_TEXTURE1);
	mv = mv*Scale(2.2,2.2,2.2);
	glUniformMatrix4fv(model_view, 1, GL_TRUE, mv);

	glBindTexture(GL_TEXTURE_2D, texName[1]); //which texture do we want?
	glDrawArrays( GL_QUAD_STRIP, 0, VertexCount );

	}



    glUseProgram(programAllFeatures);

    glFlush();
    /*start processing buffered OpenGL routines*/
    glutSwapBuffers();
}

void setupShader(GLuint prog){
	
	//glUseProgram( prog );
	////glLinkProgram( prog);
	//model_view = glGetUniformLocation(prog, "model_view");
	//projection = glGetUniformLocation(prog, "projection");
	//
	//vAmbientDiffuseColor = glGetAttribLocation(prog, "vAmbientDiffuseColor");
	//vSpecularColor = glGetAttribLocation(prog, "vSpecularColor");
	//vSpecularExponent = glGetAttribLocation(prog, "vSpecularExponent");
	//light_position = glGetUniformLocation(prog, "light_position");
	//light_color = glGetUniformLocation(prog, "light_color");
	//ambient_light = glGetUniformLocation(prog, "ambient_light");

	//glBindVertexArray( vao[0] );

	//glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	//vPosition = glGetAttribLocation(prog, "vPosition");
	//glEnableVertexAttribArray(vPosition);
	//glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	//glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	//vNormal = glGetAttribLocation(prog, "vNormal");
	//glEnableVertexAttribArray(vNormal);
	//glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

///////////////////////
// Setup Earth buffers for Shader
///////////////////////
void setupEarthShader(GLuint prog, GLuint vao[1], GLuint vbo[3])
{
	glUseProgram( prog );
	//glLinkProgram( prog);
	model_view = glGetUniformLocation(prog, "model_view");
	projection = glGetUniformLocation(prog, "projection");
	
	
	glBindVertexArray( vao[0] );

	glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
	GLuint vPosition = glGetAttribLocation(prog, "vPosition");
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vPosition, 4, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	GLuint texCoord = glGetAttribLocation(prog, "vtexCoord");
	glEnableVertexAttribArray(texCoord);
	glVertexAttribPointer(texCoord, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer( GL_ARRAY_BUFFER, vbo[2] );
	GLuint vNormal = glGetAttribLocation(prog, "vNormal");
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 0, 0);
}
/////////////////////////////////
// KEyboard function
/////////////////////////////////
void Keyboard(unsigned char key, int x, int y) {
	/*exit when the escape key is pressed*/
	if (key == 27)
		exit(0);

	if (key == 'g'){
		//setupShader(program1);
	}
	if (key == 'p'){
		setupShader(programAllFeatures);
	}
	if (key == 'c'){
		//setupShader(program3);
	}
	if (key == 's'){
		mode = 0;
	}
	if (key == 't'){
		mode = 1;
	}
	reshape(ww,wh);
	glutPostRedisplay();

}
/////////////////////////////////
// Mouse function
/////////////////////////////////
void mouse_dragged(int x, int y) {
	double thetaY, thetaX;
	if (left_button_down) {
		thetaY = 360.0 *(x-prevMouseX)/ww;    
		thetaX = 360.0 *(prevMouseY - y)/wh;
		prevMouseX = x;
		prevMouseY = y;
		view_rotx += thetaX;
		view_roty += thetaY;
	}
	else if (right_button_down) {
		z_distance = 5.0*(prevMouseY-y)/wh;
	}
  glutPostRedisplay();
}

/////////////////////////////////
// Mouse function
/////////////////////////////////
void mouse(int button, int state, int x, int y) {
  //establish point of reference for dragging mouse in window
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
      left_button_down = TRUE;
	  prevMouseX= x;
      prevMouseY = y;
    }

	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
      right_button_down = TRUE;
      prevMouseX = x;
      prevMouseY = y;
    }
    else if (state == GLUT_UP) {
      left_button_down = FALSE;
	  right_button_down = FALSE;
	}
}
/////////////////////////////////
// Initialize function
/////////////////////////////////
void init() {

  /*select clearing (background) color*/
  glClearColor(0.0, 0.0, 0.0, 1.0);

   rotateYEarth = 1;
   rotateZEarth = 1;


    //populate our arrays
    //spherevertcount = generateSphere(2, 30);

    vec4 sphereverts[2592];
	vec2 spheretexcoords[2592]; // 2592
	vec3 spherenormals[2592];
	vec4 spheretangents[2592];

	CreateSphere(sphereverts, spheretexcoords, spherenormals, spheretangents, 2, 0,0,0);

	/////////////////////////////////////////
	// Create a vertex array object
	spherevao = new GLuint[1];
	spherevbo = new GLuint[3];

	cloudvao = new GLuint[1];
	cloudvbo = new GLuint[3];

	//////////////////////////
	// earth buffer
	//////////////////////////
	glGenVertexArrays( 1, &spherevao[0] );

	// Create and initialize any buffer objects
	glBindVertexArray( spherevao[0] );
	glGenBuffers( 2, &spherevbo[0] );
	glBindBuffer( GL_ARRAY_BUFFER, spherevbo[0] );
	glBufferData( GL_ARRAY_BUFFER, VertexCount*sizeof(vec4), sphereverts, GL_STATIC_DRAW);

	glBindBuffer( GL_ARRAY_BUFFER, spherevbo[1] );
	glBufferData( GL_ARRAY_BUFFER, VertexCount*sizeof(vec2), spheretexcoords, GL_STATIC_DRAW);

	glBindBuffer( GL_ARRAY_BUFFER, spherevbo[2] );
	glBufferData( GL_ARRAY_BUFFER, VertexCount*sizeof(vec3), spherenormals, GL_STATIC_DRAW);

	//////////////////////
	// Cloud buffer
	//////////////////////
	glGenVertexArrays( 1, &cloudvao[0] );

	// Create and initialize any buffer objects
	glBindVertexArray( cloudvao[0] );
	glGenBuffers( 2, &cloudvbo[0] );
	glBindBuffer( GL_ARRAY_BUFFER, cloudvbo[0] );
	glBufferData( GL_ARRAY_BUFFER, VertexCount*sizeof(vec4), sphereverts, GL_STATIC_DRAW);

	glBindBuffer( GL_ARRAY_BUFFER, cloudvbo[1] );
	glBufferData( GL_ARRAY_BUFFER, VertexCount*sizeof(vec2), spheretexcoords, GL_STATIC_DRAW);

	glBindBuffer( GL_ARRAY_BUFFER, cloudvbo[2] );
	glBufferData( GL_ARRAY_BUFFER, VertexCount*sizeof(vec3), spherenormals, GL_STATIC_DRAW);

   // Load shaders and use the resulting shader program
    
	programAllFeatures = InitShader( "vshader-phongshading.glsl", "fshader-phongshading.glsl" );
	programColor = InitShader( "vshader-color.glsl", "fshader-color.glsl" );
	programCloud = InitShader( "vshader-cloud.glsl", "fshader-cloud.glsl" );


    glUseProgram(programAllFeatures);

	// Create a vertex array object
 //   glGenVertexArrays( 1, &vao[0] );

 //   // Create and initialize any buffer objects
	//glBindVertexArray( vao[0] );
	//glGenBuffers( 2, &vbo[0] );
 //   glBindBuffer( GL_ARRAY_BUFFER, vbo[0] );
 //   glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec4), sphere_verts, GL_STATIC_DRAW);
	//

	////and now our colors for each vertex
	//glBindBuffer( GL_ARRAY_BUFFER, vbo[1] );
	//glBufferData( GL_ARRAY_BUFFER, spherevertcount*sizeof(vec3), sphere_normals, GL_STATIC_DRAW );

	// setupShader(programAllFeatures);

	setupEarthShader(programAllFeatures, spherevao, spherevbo);

	// set up cloud shader
	// setupEarthShader(programCloud, cloudvao, cloudvbo);

	glUseProgram(programAllFeatures);

    ILuint ilTexID[5]; /* ILuint is a 32bit unsigned integer.

    //Variable texid will be used to store image name. */
   

	ilInit(); /* Initialization of OpenIL */
	ilGenImages(5, ilTexID); /* Generation of three image names for OpenIL image loading */
	glGenTextures(5, texName); //and we eventually want the data in an OpenGL texture
 

	/////////////////////////////////////////
	// EARTH BUFFERS
	////////////////////////////////////////////////////
	
	// load color map
	if ( true )
	{
		ilBindImage(ilTexID[0]); /* Binding of IL image name */
		loadTexFile("images/Earth.png");
		glBindTexture(GL_TEXTURE_2D, texName[0]); //bind OpenGL texture name

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	   //Note how we depend on OpenIL to supply information about the file we just loaded in
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
		ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());


	}

	// load cloud
	
	if (true)
	{
		glUseProgram(programCloud);

		ilBindImage(ilTexID[1]); /* Binding of IL image name */
		glBindTexture(GL_TEXTURE_2D, texName[1]); //bind OpenGL texture name
		loadTexFile("images/earthcloudmap.png");
		

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	   //Note how we depend on OpenIL to supply information about the file we just loaded in
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
		ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
				
	}


	// Spec map
	if (true)
	{
		glUseProgram(programAllFeatures);

		ilBindImage(ilTexID[2]); /* Binding of IL image name */
		glBindTexture(GL_TEXTURE_2D, texName[2]); //bind OpenGL texture name
		loadTexFile("images/EarthSpec.png");
		

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	   //Note how we depend on OpenIL to supply information about the file we just loaded in
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
		ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	}


	// Normal map
	if (true)
	{
		ilBindImage(ilTexID[3]); /* Binding of IL image name */
		glBindTexture(GL_TEXTURE_2D, texName[3]); //bind OpenGL texture name
		loadTexFile("images/EarthNormal.png");
		

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	   //Note how we depend on OpenIL to supply information about the file we just loaded in
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
		ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	}

	// Night map
	if (true)
	{
		ilBindImage(ilTexID[4]); /* Binding of IL image name */
		glBindTexture(GL_TEXTURE_2D, texName[4]); //bind OpenGL texture name
		loadTexFile("images/EarthNight.png");
		

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	   //Note how we depend on OpenIL to supply information about the file we just loaded in
		glTexImage2D(GL_TEXTURE_2D, 0, ilGetInteger(IL_IMAGE_BPP), ilGetInteger(IL_IMAGE_WIDTH), ilGetInteger(IL_IMAGE_HEIGHT),0,
		ilGetInteger(IL_IMAGE_FORMAT), ilGetInteger(IL_IMAGE_TYPE), ilGetData());
	}


	////////////////////////////////////////////////
    ilDeleteImages(5, ilTexID); //we're done with OpenIL, so free up the memory

	////////////////////////////////////////////////////

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texMap = glGetUniformLocation(programAllFeatures, "texture");
	glUniform1i(texMap, 0);//assign this one to texture unit 0
	
	cloudMap = glGetUniformLocation(programCloud, "cloudtexture");
	glUniform1i(cloudMap, 1);//assign cloud map to  texture unit 1

	specMap = glGetUniformLocation(programAllFeatures, "spectexture");
	glUniform1i(specMap, 2);//assign spec map to 2 texture unit
	
	normalMap = glGetUniformLocation(programAllFeatures, "normalMap");
	glUniform1i(normalMap, 3);//assign normal map to 3 texture unit
	
	nightMap = glGetUniformLocation(programAllFeatures, "nighttexture");
	glUniform1i(nightMap, 4);//assign spec map to 4 texture unit
	
	
	// setup lightning
	vAmbientDiffuseColor = glGetAttribLocation(programAllFeatures, "vAmbientDiffuseColor");
	vSpecularColor = glGetAttribLocation(programAllFeatures, "vSpecularColor");
	vSpecularExponent = glGetAttribLocation(programAllFeatures, "vSpecularExponent");
	light_position = glGetUniformLocation(programAllFeatures, "light_position");
	light_color = glGetUniformLocation(programAllFeatures, "light_color");
	ambient_light = glGetUniformLocation(programAllFeatures, "ambient_light");
	
  //Only draw the things in the front layer
	glEnable(GL_DEPTH_TEST);
}

int main(int argc, char **argv)
{
  /*set up window for display*/
  glutInit(&argc, argv);
  glutInitWindowPosition(0, 0); 
  glutInitWindowSize(ww, wh);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutCreateWindow("The Earth");  

  glewExperimental = GL_TRUE;

	glewInit();
  init();

  glutDisplayFunc(display);
  glutKeyboardFunc(Keyboard);
  glutReshapeFunc(reshape);
  //glutIdleFunc(idle);
  glutMouseFunc(mouse);
  glutMotionFunc(mouse_dragged);
  glutIdleFunc(myIdle);

  glutMainLoop();
  return 0;
}