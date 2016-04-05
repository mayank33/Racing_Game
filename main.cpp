/* Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above notice and this permission notice shall be included in all copies
 * or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
/* File for "Terrain" lesson of the OpenGL tutorial on
 * www.videotutorialsrock.com
 */



#include <iostream>
#include <stdlib.h>
#include <cmath>


#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "imageloader.h"
#include "vec3f.h"


#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)
#define RAD2DEG(rad) (rad * 180/PI)


#define WIN_WIDTH      600 
#define WIN_HEIGHT      600    
#define CYCLE_LENGTH   3.3f 
#define ROD_RADIUS      0.05f 
#define NUM_SPOKES      20 
#define SPOKE_ANGLE      18 
#define RADIUS_WHEEL   1.0f 
#define TUBE_WIDTH      0.08f 
#define RIGHT_ROD      1.6f 
#define RIGHT_ANGLE      48.0f 
#define MIDDLE_ROD      1.7f 
#define MIDDLE_ANGLE   106.0f 
#define BACK_CONNECTOR   0.5f 
#define LEFT_ANGLE      50.0f 
#define WHEEL_OFFSET   0.11f 
#define WHEEL_LEN      1.1f 
#define TOP_LEN         1.5f 
#define CRANK_ROD      0.7f 
#define CRANK_RODS      1.12f 
#define CRANK_ANGLE      8.0f 
#define HANDLE_ROD      1.2f 
#define FRONT_INCLINE   70.0f 
#define HANDLE_LIMIT   70.0f 

#define INC_STEERING   2.0f 
#define INC_SPEED      0.05f



 int i;
 float j[190];
 float jop[190];
 float j2[190];
 float jop2[190];
 float jop3[190];
 float jop4[190];
 float bik_x;
 float bik_z;
 float angl;
 int turn;
 float r1[20];
 float r2[20];
 float r3[20];
 float terangl;
 float perangl;
 float strangl;
 float diff;
 GLfloat pedalAngle, speed, steering; 


 /*-----------------------------------Angle Allocation------------------------*/
Vec3f yAx;
Vec3f zAx;
Vec3f xAx;
float xEarlier ;
float zEarlier ;
float cosAngle,sinAngle;
/*--------------------------------------------------------------------------*/

/******************************* 
*   User view realted variables 
********************************/ 
GLfloat camx,camy,camz; 
GLfloat anglex,angley,anglez; 

/**************************** 
*   Mouse related variables 
****************************/ 
int prevx,prevy; 
GLenum Mouse; 

/************************** 
*   Cycle position related 
*   variables 
***************************/ 
GLfloat xpos,zpos,direction; 

using namespace std;

//Represents a terrain, by storing a set of heights and normals at 2D locations
class Terrain {
	private:
		int w; //Width
		int l; //Length
		float** hs; //Heights
		Vec3f** normals;
		bool computedNormals; //Whether normals is up-to-date
	public:
		Terrain(int w2, int l2) {
			w = w2;
			l = l2;
			
			hs = new float*[l];
			for(int i = 0; i < l; i++) {
				hs[i] = new float[w];
			}
			
			normals = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals[i] = new Vec3f[w];
			}
			
			computedNormals = false;
		}
		
		~Terrain() {
			for(int i = 0; i < l; i++) {
				delete[] hs[i];
			}
			delete[] hs;
			
			for(int i = 0; i < l; i++) {
				delete[] normals[i];
			}
			delete[] normals;
		}
		
		int width() {
			return w;
		}
		
		int length() {
			return l;
		}
		
		//Sets the height at (x, z) to y
		void setHeight(int x, int z, float y) {
			hs[z][x] = y;
			computedNormals = false;
		}
		
		//Returns the height at (x, z)
		float getHeight(int x, int z) {
			return hs[z][x];
		}
		
		//Computes the normals, if they haven't been computed yet
		void computeNormals() {
			if (computedNormals) {
				return;
			}
			
			//Compute the rough version of the normals
			Vec3f** normals2 = new Vec3f*[l];
			for(int i = 0; i < l; i++) {
				normals2[i] = new Vec3f[w];
			}
			
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum(0.0f, 0.0f, 0.0f);
					
					Vec3f out;
					if (z > 0) {
						out = Vec3f(0.0f, hs[z - 1][x] - hs[z][x], -1.0f);
					}
					Vec3f in;
					if (z < l - 1) {
						in = Vec3f(0.0f, hs[z + 1][x] - hs[z][x], 1.0f);
					}
					Vec3f left;
					if (x > 0) {
						left = Vec3f(-1.0f, hs[z][x - 1] - hs[z][x], 0.0f);
					}
					Vec3f right;
					if (x < w - 1) {
						right = Vec3f(1.0f, hs[z][x + 1] - hs[z][x], 0.0f);
					}
					
					if (x > 0 && z > 0) {
						sum += out.cross(left).normalize();
					}
					if (x > 0 && z < l - 1) {
						sum += left.cross(in).normalize();
					}
					if (x < w - 1 && z < l - 1) {
						sum += in.cross(right).normalize();
					}
					if (x < w - 1 && z > 0) {
						sum += right.cross(out).normalize();
					}
					
					normals2[z][x] = sum.normalize();
				}
			}
			
			//Smooth out the normals
			const float FALLOUT_RATIO = 0.5f;
			for(int z = 0; z < l; z++) {
				for(int x = 0; x < w; x++) {
					Vec3f sum = normals2[z][x];
					
					if (x > 0) {
						sum += normals2[z][x - 1] * FALLOUT_RATIO;
					}
					if (x < w - 1) {
						sum += normals2[z][x + 1] * FALLOUT_RATIO;
					}
					if (z > 0) {
						sum += normals2[z - 1][x] * FALLOUT_RATIO;
					}
					if (z < l - 1) {
						sum += normals2[z + 1][x] * FALLOUT_RATIO;
					}
					
					if (sum.magnitude() == 0) {
						sum = Vec3f(0.0f, 1.0f, 0.0f);
					}
					normals[z][x] = sum;
				}
			}
			
			for(int i = 0; i < l; i++) {
				delete[] normals2[i];
			}
			delete[] normals2;
			
			computedNormals = true;
		}
		
		//Returns the normal at (x, z)
		Vec3f getNormal(int x, int z) {
			if (!computedNormals) {
				computeNormals();
			}
			return normals[z][x];
		}
};

GLUquadricObj* myReusableQuadric = 0;
void drawGluCylinder( float height, float radius, float slices, float stacks ) {
	if ( ! myReusableQuadric ) {
		myReusableQuadric = gluNewQuadric();  
		// Should (but don't) check if pointer is still null --- to catch memory allocation errors.
		gluQuadricNormals( myReusableQuadric, GL_TRUE );
	}
	
	gluCylinder( myReusableQuadric, radius, radius, height, slices, stacks );
	
	
	
}





void ZCylinder(GLfloat radius,GLfloat length) 
{ 
   GLUquadricObj *cylinder; 
   cylinder=gluNewQuadric(); 
   glPushMatrix(); 
      glTranslatef(0.0f,0.0f,0.0f); 
      gluCylinder(cylinder,radius,radius,length,15,5); 
   glPopMatrix(); 
} 

void XCylinder(GLfloat radius,GLfloat length) 
{ 
   glPushMatrix(); 
      glRotatef(90.0f,0.0f,1.0f,0.0f); 
      ZCylinder(radius,length); 
   glPopMatrix(); 
} 

void drawSeat() 
{ 
   /********************************* 
   *   Draw the top of the seat 
   **********************************/ 
   glBegin(GL_POLYGON); 
      glVertex3f(-0.1f, 1.0f, -0.5f); 
      glVertex3f(   1.0f, 1.0f, -0.3f); 
      glVertex3f( 1.0f, 1.0f,  0.3f); 
      glVertex3f(-0.1f, 1.0f,  0.5f); 
      glVertex3f(-0.5f, 1.0f,  1.0f); 
      glVertex3f(-1.0f, 1.0f,  1.0f); 
      glVertex3f(-1.0f, 1.0f, -1.0f); 
      glVertex3f(-0.5f, 1.0f, -1.0f); 
   glEnd(); 

   /********************************** 
   *   Draw the bottom base part of the 
   *   seat 
   ************************************/ 
   glBegin(GL_POLYGON); 
      glVertex3f(-0.1f, -1.0f, -0.5f); 
      glVertex3f(   1.0f, -1.0f, -0.3f); 
      glVertex3f( 1.0f, -1.0f,  0.3f); 
      glVertex3f(-0.1f, -1.0f,  0.5f); 
      glVertex3f(-0.5f, -1.0f,  1.0f); 
      glVertex3f(-1.0f, -1.0f,  1.0f); 
      glVertex3f(-1.0f, -1.0f, -1.0f); 
      glVertex3f(-0.5f, -1.0f, -1.0f); 
   glEnd(); 

   /********************** 
   *   Draw the sides! 
   ***********************/ 
   glBegin(GL_QUADS); 
      glVertex3f(1.0f,1.0f,-0.3f); 
      glVertex3f(1.0f,1.0f,0.3f); 
      glVertex3f(1.0f,-1.0f,0.3f); 
      glVertex3f(1.0f,-1.0f,-0.3f); 

      glVertex3f(1.0f,1.0f,0.3f); 
      glVertex3f(-0.1f,1.0f,0.5f); 
      glVertex3f(-0.1f,-1.0f,0.5f); 
      glVertex3f(1.0f,-1.0f,0.3f); 

      glVertex3f(1.0f,1.0f,-0.3f); 
      glVertex3f(-0.1f,1.0f,-0.5f); 
      glVertex3f(-0.1f,-1.0f,-0.5f); 
      glVertex3f(1.0f,-1.0f,-0.3f); 

      glVertex3f(-0.1f,1.0f,0.5f); 
      glVertex3f(-0.5f,1.0f,1.0f); 
      glVertex3f(-0.5f,-1.0f,1.0f); 
      glVertex3f(-0.1f,-1.0f,0.5f); 

      glVertex3f(-0.1f,1.0f,-0.5f); 
      glVertex3f(-0.5f,1.0f,-1.0f); 
      glVertex3f(-0.5f,-1.0f,-1.0f); 
      glVertex3f(-0.1f,-1.0f,-0.5f); 

      glVertex3f(-0.5f,1.0f,1.0f); 
      glVertex3f(-1.0f,1.0f,1.0f); 
      glVertex3f(-1.0f,-1.0f,1.0f); 
      glVertex3f(-0.5f,-1.0f,1.0f); 

      glVertex3f(-0.5f,1.0f,-1.0f); 
      glVertex3f(-1.0f,1.0f,-1.0f); 
      glVertex3f(-1.0f,-1.0f,-1.0f); 
      glVertex3f(-0.5f,-1.0f,-1.0f); 

      glVertex3f(-1.0f,1.0f,1.0f); 
      glVertex3f(-1.0f,1.0f,-1.0f); 
      glVertex3f(-1.0f,-1.0f,-1.0f); 
      glVertex3f(-1.0f,-1.0f,1.0f); 

   glEnd(); 


} 

void drawTyre(void) 
{ 
   int i; 
   //   Draw The Rim 
   glColor3f(1.0f,1.0f,1.0f); 
   glutSolidTorus(0.06f,0.92f,4,30); 
   //   Draw The Central Cylinder 
   //   Length of cylinder  0.12f 
   glColor3f(1.0f,1.0f,0.5f); 
   glPushMatrix(); 
      glTranslatef(0.0f,0.0f,-0.06f); 
      ZCylinder(0.02f,0.12f); 
   glPopMatrix(); 
   glutSolidTorus(0.02f,0.02f,3,20); 

   //   Draw The Spokes 
   glColor3f(1.0f,1.0f,1.0f); 
   for(i=0;i<NUM_SPOKES;++i) 
   { 
      glPushMatrix(); 
         glRotatef(i*SPOKE_ANGLE,0.0f,0.0f,1.0f); 
         glBegin(GL_LINES); 
            glVertex3f(0.0f,0.02f,0.0f); 
            glVertex3f(0.0f,0.86f,0.0f); 
         glEnd(); 
      glPopMatrix(); 
   } 

   //   Draw The Tyre 
   glColor3f(0.0f,0.0f,0.0f); 
   glutSolidTorus(TUBE_WIDTH,RADIUS_WHEEL,10,30); 
   glColor3f(1.0f,0.0f,0.0f); 
} 


void DrawBike(){

   glRotatef(angl,0.0,1.0,0.0);
   glRotatef(terangl,1.0,0.0,0.0);
   glRotatef(strangl,0.0,0.0,1.0);
   glColor3f(1.0f,0.0f,0.0f); 
   glRotatef(90,0.0f,1.0f,0.0f);
   /******************************** 
   *   First draw the all the items 
   *   at the center of the frame. 
   *   Draw the bigger gear,the small 
   *   cylinder acting as the socket 
   *   for the pedals.Also DON'T 
   *   forget to draw the two 
   *   connecting cemtral rods 
   *********************************/ 
   glPushMatrix(); 

      /***************************** 
      *   Lets first draw the 
      *   rightmost rod of the frame 
      *******************************/ 
      glRotatef(RIGHT_ANGLE,0.0f,0.0f,1.0f); 
      XCylinder(ROD_RADIUS,RIGHT_ROD); 
       
      /******************************* 
      *   Now draw the centre rod of 
      *   the frame which also supports 
      *   the seat 
      *********************************/ 
      glRotatef(MIDDLE_ANGLE-RIGHT_ANGLE,0.0f,0.0f,1.0f); 
      XCylinder(ROD_RADIUS,MIDDLE_ROD); 
      /******************************** 
      *   We have drawn the support.So 
      *   let's draw the seat with a 
      *   new color 
      *********************************/ 
      glColor3f(1.0f,1.0f,0.0f); 
      glTranslatef(MIDDLE_ROD,0.0f,0.0f); 
      glRotatef(-MIDDLE_ANGLE,0.0f,0.0f,1.0f); 
      glScalef(0.3f,ROD_RADIUS,0.25f); 
      drawSeat(); 
      /********************** 
      *   Restore the color ! 
      ************************/ 
      glColor3f(1.0f,0.0f,0.0f); 
   glPopMatrix(); 
   /********************************* 
   *   Draw the horizontal part of 
   *   the frame. 
   *********************************/ 
    
   /********************************* 
   *   Draw the main single rod 
   *   connecting the center of the 
   *   frame to the back wheel of the 
   *   cycle 
   **********************************/ 
   glPushMatrix(); 
      glRotatef(-180.0f,0.0f,1.0f,0.0f); 
      XCylinder(ROD_RADIUS,BACK_CONNECTOR); 
    
      /*********************************** 
      *   Draw the two rods on the either 
      *   side of the wheel 
      *   These rods are part of the 
      *   horizontal part of the cycle 
      ************************************/ 
      glPushMatrix(); 
         glTranslatef(0.5f,0.0f,WHEEL_OFFSET); 
         XCylinder(ROD_RADIUS,RADIUS_WHEEL+TUBE_WIDTH); 
      glPopMatrix(); 
      glPushMatrix(); 
         glTranslatef(0.5f,0.0f,-WHEEL_OFFSET); 
         XCylinder(ROD_RADIUS,RADIUS_WHEEL+TUBE_WIDTH); 
      glPopMatrix(); 
   glPopMatrix(); 
    
   /************************************ 
   *   Draw the leftmost rods of the 
   *   frame of the cycle 
   *************************************/ 
   glPushMatrix(); 
      glTranslatef(-(BACK_CONNECTOR+RADIUS_WHEEL+TUBE_WIDTH),0.0f,0.0f); 
         /******************************** 
      *   Transalted to the back wheel 
      *   position.Why not draw the back 
      *   wheel and also the gear ? :)) 
      **********************************/ 
      glPushMatrix(); 
         glRotatef(-2*pedalAngle,0.0f,0.0f,1.0f); 
         drawTyre(); 
      glPopMatrix(); 
      glRotatef(LEFT_ANGLE,0.0f,0.0f,1.0f); 
          
      /************************************ 
      *   Draw the two rods on the either 
      *   side of the wheel connecting the 
      *   backwheel and topmost horizontal 
      *   part of the wheel 
      *************************************/ 
      glPushMatrix(); 
         glTranslatef(0.0f,0.0f,-WHEEL_OFFSET); 
         XCylinder(ROD_RADIUS,WHEEL_LEN); 
      glPopMatrix(); 
      glPushMatrix(); 
         glTranslatef(0.0f,0.0f,WHEEL_OFFSET); 
         XCylinder(ROD_RADIUS,WHEEL_LEN); 
      glPopMatrix(); 
       
      /***************************** 
      *   Draw the single rod of the 
      *   same setup 
      ******************************/ 
      glTranslatef(WHEEL_LEN,0.0f,0.0f); 
      XCylinder(ROD_RADIUS,CRANK_ROD); 
    
      /***************************** 
      *   Now Draw The topmost 
      *   Horizontal rod 
      *****************************/ 
      glTranslatef(CRANK_ROD,0.0f,0.0f); 
      glRotatef(-LEFT_ANGLE,0.0f,0.0f,1.0f); 
      XCylinder(ROD_RADIUS,TOP_LEN); 
       
      /******************************* 
      *   Now instead of again traversing 
      *   all the way back and again 
      *   forward.WHY NOT DRAW THE 
      *   HANDLE FROM HERE ITSELF? 
      ********************************/ 
      /***************************** 
      *   Now draw the handle and 
      *   small support rod which 
      *   is incorporated in the 
      *   frame itself. 
      *   Set y-axis at te required 
      *   incline. 
      ******************************/ 
      glTranslatef(TOP_LEN,0.0f,0.0f); 
      glRotatef(-FRONT_INCLINE,0.0f,0.0f,1.0f); 
       
      /****************************** 
      *   Draw the small rod that acts 
      *   as the socket joining the 
      *   frame and the handle 
      ******************************/ 
      glPushMatrix(); 
         glTranslatef(-0.1f,0.0f,0.0f); 
         XCylinder(ROD_RADIUS,0.45f); 
      glPopMatrix(); 
       
      /****************************** 
      *   I Hope the handle can rotate 
      *   about its mean position 
      *******************************/ 
      glPushMatrix(); 
         glRotatef(-steering,1.0f,0.0f,0.0f); 
         /****************************** 
         *   Roll back to the height of 
         *   the handle to draw it 
         *******************************/ 
         glTranslatef(-0.3f,0.0f,0.0f); 

         /******************************** 
         *   We cannot use the incline    
         *   the incline to draw the 
         *   horizontal part of the rod 
         ********************************/ 
         glPushMatrix(); 
            glRotatef(FRONT_INCLINE,0.0f,0.0f,1.0f); 

            glPushMatrix(); 
               glTranslatef(0.0f,0.0f,-HANDLE_ROD/2); 
               ZCylinder(ROD_RADIUS,HANDLE_ROD); 
            glPopMatrix(); 

            glPushMatrix(); 
               glColor3f(1.0f,1.0f,0.0f); 
               glTranslatef(0.0f,0.0f,-HANDLE_ROD/2); 
               ZCylinder(0.07f,HANDLE_ROD/4); 
               glTranslatef(0.0f,0.0f,HANDLE_ROD*3/4); 
               ZCylinder(0.07f,HANDLE_ROD/4); 
               glColor3f(1.0f,0.0f,0.0f); 
            glPopMatrix(); 
         glPopMatrix(); 

         /********************************* 
         *   Using this incline now draw 
         *   the handle.Maybe use this again 
         *   to draw the wheel. ;) 
         **********************************/ 
         glPushMatrix(); 
            /**************************** 
            *   Draw the main big rod 
            ****************************/ 
            XCylinder(ROD_RADIUS,CRANK_ROD); 

            /****************************** 
            *   Why not draw the two rods and 
            *   the WHEEL?   :) 
            *   Yes!So,first go to the 
            *   end of the main rod. 
            *******************************/ 
            glTranslatef(CRANK_ROD,0.0f,0.0f); 
            glRotatef(CRANK_ANGLE,0.0f,0.0f,1.0f); 

            /******************************* 
            *   Draw the two rods connecting 
            *   the handle and the front 
            *   wheel. 
            *   The two rods are at a incline 
            *   to the connector. 
            ********************************/ 
            glPushMatrix(); 
               glTranslatef(0.0f,0.0f,WHEEL_OFFSET); 
               XCylinder(ROD_RADIUS,CRANK_RODS); 
            glPopMatrix(); 
            glPushMatrix(); 
               glTranslatef(0.0f,0.0f,-WHEEL_OFFSET); 
               XCylinder(ROD_RADIUS,CRANK_RODS); 
            glPopMatrix(); 
               /******************************** 
            *   Why not draw the wheel. 
            *   The FRONT wheel to be precise 
            *********************************/ 
            glTranslatef(CRANK_RODS,0.0f,0.0f); 
            glRotatef(-2*pedalAngle,0.0f,0.0f,1.0f); 
            drawTyre(); 
         glPopMatrix(); 
      glPopMatrix();   /*   End of the rotation of the handle effect   */ 
   glPopMatrix(); 

}



//Draw bike
void Bike()

{glRotatef(angl,0.0,1.0,0.0);
	glPushMatrix();
		
		//glutSolidCube(2.0);
		glBegin(GL_QUADS);                // Begin drawing the color cube with 6 quads
      // Top face (y = 1.0f)
      // Define vertices in counter-clockwise (CCW) order with normal pointing out
      glColor3f(0.0f, 1.0f, 0.0f);     // Green
      glVertex3f( 2.0f, 3.0f, -0.5f);
      glVertex3f(-2.0f, 3.0f, -0.5f);
      glVertex3f(-2.0f, 3.0f,  0.5f);
      glVertex3f( 2.0f, 3.0f,  0.5f);
 
      // Bottom face (y = -1.0f)
      glColor3f(0.0f, 1.0f, 0.0f);     // Orange
      glVertex3f( 2.0f, 2.0f, -0.5f);
      glVertex3f( 2.0f, 2.0f,  0.5f);
      glVertex3f(-2.0f, 2.0f,  0.5f);
      glVertex3f(-2.0f, 2.0f, -0.5f);
      


      // Front face  (z = 1.0f)
      glColor3f(0.0f, 1.0f, 0.0f);     // Red
      glVertex3f( 2.0f, 3.0f, 0.5f);
      glVertex3f(-2.0f, 3.0f, 0.5f);
      glVertex3f(-2.0f, 2.0f, 0.5f);
      glVertex3f( 2.0f, 2.0f, 0.5f);
      
      
      
     
 
      // Back face (z = -1.0f)
      glColor3f(0.0f, 1.0f, 0.0f);    // Yellow2.0
      glVertex3f( 2.0f, 3.0f, -0.5f);
      glVertex3f( 2.0f, 2.0f, -0.5f);
      glVertex3f(-2.0f, 2.0f, -0.5f);
      glVertex3f(-2.0f, 3.0f, -0.5f);
      
      
 
      // Left face (x = -1.0f)
      glColor3f(0.0f, 0.0f, 1.0f);     // Blue
      glVertex3f(-2.0f,  3.0f,  0.5f);
      glVertex3f(-2.0f,  3.0f, -0.5f);
      glVertex3f(-2.0f,  2.0f, -0.5f);
      glVertex3f(-2.0f,  2.0f,  0.5f);
 
      // Right face (x = 1.0f)
      glColor3f(0.0f, 1.0f, 0.0f);    // Magenta
      glVertex3f(2.0f,  3.0f, -0.5f);
      glVertex3f(2.0f,  3.0f,  0.5f);
      glVertex3f(2.0f,  2.0f,  0.5f);
      glVertex3f(2.0f,  2.0f, -0.5f);
   glEnd();  // End of drawing color-cube



	glPopMatrix();
	glPushMatrix();
		glTranslatef(1.0,1.0,-0.5);
		drawGluCylinder(1.0, 1.0, 50, 50 );
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-1.0,1.0,-0.5);
		drawGluCylinder(1.0, 1.0, 50, 50 );
	glPopMatrix();

}




//Draw Snowman 
void drawSnowman()
{
	// Draw body (a 20x20 spherical mesh of radius 0.75 at height 0.75)
	glColor3f(1.0, 1.0, 1.0); // set drawing color to white
	glPushMatrix();
		glTranslatef(0.0, 0.0, 0.75);
		glutSolidSphere(0.75, 20, 20);
	glPopMatrix();

	// Draw the head (a sphere of radius 0.25 at height 1.75)
	glPushMatrix();
		glTranslatef(0.0, 0.0, 1.75); // position head
		glutSolidSphere(0.25, 20, 20); // head sphere

		// Draw Eyes (two small black spheres)
		glColor3f(0.0, 0.0, 0.0); // eyes are black
		glPushMatrix();
			glTranslatef(0.0, -0.18, 0.10); // lift eyes to final position
			glPushMatrix();
				glTranslatef(-0.05, 0.0, 0.0);
				glutSolidSphere(0.05, 10, 10); // right eye
			glPopMatrix();
			glPushMatrix();
				glTranslatef(+0.05, 0.0, 0.0);
				glutSolidSphere(0.05, 10, 10); // left eye
			glPopMatrix();
		glPopMatrix();

		// Draw Nose (the nose is an orange cone)
		glColor3f(1.0, 0.5, 0.5); // nose is orange
		glPushMatrix();
			glRotatef(90.0, 1.0, 0.0, 0.0); // rotate to point along -y
			glutSolidCone(0.08, 0.5, 10, 2); // draw cone
		glPopMatrix();
	glPopMatrix();

	// Draw a faux shadow beneath snow man (dark green circle)
	glColor3f(0.0, 0.5, 0.0);
	glPushMatrix();
		glTranslatef(0.2, 0.2, 0.001);	// translate to just above ground
		glScalef(1.0, 1.0, 0.0); // scale sphere into a flat pancake
		glutSolidSphere(0.75, 20, 20); // shadow same size as body
	glPopMatrix();
}


//Loads a terrain from a heightmap.  The heights of the terrain range from
//-height / 2 to height / 2.
Terrain* loadTerrain(const char* filename, float height) {
	Image* image = loadBMP(filename);
	Terrain* t = new Terrain(image->width, image->height);
	for(int y = 0; y < image->height; y++) {
		for(int x = 0; x < image->width; x++) {
			unsigned char color =
				(unsigned char)image->pixels[3 * (y * image->width + x)];
			float h = height * ((color / 255.0f) - 0.5f);
			t->setHeight(x, y, h);
		}
	}
	
	delete image;
	t->computeNormals();
	return t;
}

float _angle = 60.0f;
Terrain* _terrain;

void cleanup() {
	delete _terrain;
}

void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		case 27: //Escape key
			cleanup();
			exit(0);
			break;
		case 'v':
			turn = turn+1;
			
			break;
      case 'd':
         strangl = strangl - 0.5;
         break;
      case 'a':
         strangl = strangl + 0.5;
         break;


	}
}







void handleKeypress2(int key, int x, int y) {

    if (key == GLUT_KEY_LEFT)
        angl += 1.0;
    if (key == GLUT_KEY_RIGHT)
        angl -= 1.0;
    if (key == GLUT_KEY_UP)
        {bik_x += cos(DEG2RAD(-angl));
    	bik_z += sin(DEG2RAD(-angl));
    	}
    //if (key == GLUT_KEY_DOWN)
        //{bik_x -= cos(DEG2RAD(angl));
    	//bik_z -= sin(DEG2RAD(angl));
    	//}
}


void initRendering() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glShadeModel(GL_SMOOTH);
}

void handleResize(int w, int h) {
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}

void drawScene() {

   if(diff<40 && diff>-40)
   {
	float height;
	float height1;
	float height2;
	float height3;
   float height4;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float bkx;
	float bky;

	bkx=bik_x*0.03f;
	bky=bik_z*0.03f;
	
	height = _terrain -> getHeight(bik_x,bik_z);
	height3 = _terrain -> getHeight(0.5,0.5);
	glTranslatef(0.0, 0.0f, 0.0);

	if(turn%4==0)  //wheel view
	{
	gluLookAt(
			bik_x+5*cos(DEG2RAD(-angl)),height+11,bik_z+5*sin(DEG2RAD(-angl)),
			bik_x+15*cos(DEG2RAD(-angl)),height+6,bik_z+15*sin(DEG2RAD(-angl)),
			0.0,    1.0,    0.0);
	}

	else if(turn%4==1) //driver view
	{
		gluLookAt(
			bik_x-5*cos(DEG2RAD(-angl)),height+10,bik_z-5*sin(DEG2RAD(-angl)),
			bik_x+30*cos(DEG2RAD(-angl)),height+8,bik_z+30*sin(DEG2RAD(-angl)),
			0.0,    1.0,    0.0);

	}

	else if(turn%4==2) //overhead view
	{
		gluLookAt(
			bik_x-40*cos(DEG2RAD(-angl)),height+30,bik_z-40*sin(DEG2RAD(-angl)),
			bik_x+10*cos(DEG2RAD(-angl)),height+10,bik_z+10*sin(DEG2RAD(-angl)),
			0.0,    1.0,    0.0);
	}

	else if(turn%4==3) //origin view
	{
		gluLookAt(
			0.5,height3+100,0.5,
			bik_x,height,bik_z,
			0.0,    1.0,    0.0);
	}
	//glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
	//glScalef(2.0,2.0,2.0);
	//glRotatef(-_angle, 0.0f, 1.0f, 0.0f);
	
	GLfloat ambientColor[] = {0.4f, 0.4f, 0.4f, 1.0f};
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	GLfloat lightColor0[] = {0.6f, 0.6f, 0.6f, 1.0f};
	GLfloat lightPos0[] = {-0.5f, 0.8f, 0.1f, 0.0f};
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	//float scale = 5.0f / max(_terrain->width() - 1, _terrain->length() - 1);
	//glScalef(scale, scale, scale);
	//glTranslatef(-(float)(_terrain->width() - 1) / 2,
	//			 0.0f,
	//			 -(float)(_terrain->length() - 1) / 2);
	
	glColor3f(0.3f, 0.9f, 0.0f);
	for(int z = 0; z < _terrain->length() - 1; z++) {
		//Makes OpenGL draw a triangle at every three consecutive vertices
		glBegin(GL_TRIANGLE_STRIP);
		for(int x = 0; x < _terrain->width(); x++) {
			Vec3f normal = _terrain->getNormal(x, z);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z), z);
			normal = _terrain->getNormal(x, z + 1);
			glNormal3f(normal[0], normal[1], normal[2]);
			glVertex3f(x, _terrain->getHeight(x, z + 1), z + 1);
		}
		glEnd();
	}



	/* ----------------------calculation of normal with y axis at bike coords-------------------------------*/

	xEarlier = bik_x - cos(DEG2RAD(-angl));
	zEarlier = bik_z - sin(DEG2RAD(-angl));
	zAx = Vec3f(0.0f,0.0f,1.0f);
	cosAngle = (zAx.dot(_terrain->getNormal(bik_x,bik_z)))/(_terrain->getNormal(bik_x,bik_z).magnitude()*zAx.magnitude());
	//sinAngle = (yAx.cross(_terrain->getNormal(bik_x,bik_z))).magnitude()/(_terrain->getNormal(bik_x,bik_z).magnitude()*yAx.magnitude());
	//cout<<"cos value"<<cosAngle<<endl;
	//cout<<"sin value"<<sinAngle<<endl;
	terangl = acos(cosAngle);
	terangl = RAD2DEG(terangl);
   terangl = 90 - terangl;
   //cout<<terangl<<endl;


   if(terangl>-0.5 && terangl<0.5)
      {
         height4 = -(_terrain -> getHeight(bik_x,bik_z));
      }

	if(_terrain->getHeight(bik_x,bik_z)<_terrain->getHeight(xEarlier,zEarlier))
		{
         terangl = 0;
      height1 = (_terrain -> getHeight(xEarlier,zEarlier))+6;
      }
   if(_terrain->getHeight(bik_x,bik_z)>=_terrain->getHeight(xEarlier,zEarlier))
      {
         terangl = terangl;
      height1 = _terrain -> getHeight(bik_x,bik_z);
      }

	//cout<<"angle in deg"<<terangl<<endl;



   /* ----------------------calculation of normal with y axis at bike coords-------------------------------*/


   xAx = Vec3f(1.0f,0.0f,0.0f);
   cosAngle = (xAx.dot(_terrain->getNormal(bik_x,bik_z)))/(_terrain->getNormal(bik_x,bik_z).magnitude()*xAx.magnitude());
   //sinAngle = (yAx.cross(_terrain->getNormal(bik_x,bik_z))).magnitude()/(_terrain->getNormal(bik_x,bik_z).magnitude()*yAx.magnitude());
   //cout<<"cos value"<<cosAngle<<endl;
   //cout<<"sin value"<<sinAngle<<endl;
   perangl = acos(cosAngle);
   perangl = RAD2DEG(perangl);
   perangl = 90 - perangl;
   perangl = -perangl;
   cout<<perangl-strangl<<endl;
   diff = perangl-strangl;

   







   /* ----------------------calculation of normal with y axis at bike coords-------------------------------*/
/*----------------------------------------------------------------------------------------------------*/

	glPushMatrix();
	height = _terrain -> getHeight(20.0,30.0);
	
	glTranslatef(20.0,height, 30.0);
	glRotatef(-90.0,1.0,0.0,0.0);
	glScalef(9.5,9.5,9.5);
	drawSnowman();
	glPopMatrix();


	glPushMatrix();
	
	glTranslatef(bik_x,height1+4.0,bik_z);
	//cout<<"bikx"<<bik_x<<endl;
	//cout<<"bikz"<<bik_z<<endl;
	glScalef(3.0,3.0,3.0);
	glRotatef(-90.0,0.0,1.0,0.0);
   //glRotatef(terangl,1.0,0.0,0.0);
	DrawBike();
	glPopMatrix();


	
	
	for(i=0;i<20;i++)
	{   
		glPushMatrix();
		glScalef(1.0,1.0,1.0);
	height2 = _terrain -> getHeight(r1[i],r2[i]);
	glColor3f(1.0f,0.0f,0.0f);
	glTranslatef(r1[i],height2,r2[i]);
	glutSolidSphere(r3[i], 20, 20);
	glPopMatrix();
	}
	// water
	for(i=0;i<190;i++)
	{
	glPushMatrix();
		glScalef(1.0,1.0,1.0);
		
		glColor4f(0.196078, 0.6, 0.8,1.0);
		glTranslatef(j[i], jop[i], j[i]);

		glutSolidSphere(0.75, 20, 20);
	glPopMatrix();
	glPushMatrix();
		glScalef(1.0,1.0,1.0);
		
		glColor4f(0.196078, 0.6, 0.8,1.0);
		glTranslatef(j[i]+1.0f, jop2[i], j[i]);
		
		glutSolidSphere(0.75, 20, 20);
	glPopMatrix();
	glPushMatrix();
		glScalef(1.0,1.0,1.0);
		
		glColor4f(0.196078, 0.6, 0.8,1.0);
		glTranslatef(j[i]-1.0f, jop3[i], j[i]);
		
		glutSolidSphere(0.75, 20, 20);
	glPopMatrix();
	glPushMatrix();
		glScalef(1.0,1.0,1.0);
		
		glColor4f(0.196078, 0.6, 0.8,1.0);
		glTranslatef(j[i]+2.0f, jop4[i], j[i]);
		
		glutSolidSphere(0.75, 20, 20);
	glPopMatrix();
	}
	glutSwapBuffers();
}

else
{
   exit(0);
}
}

void update(int value) {
	_angle += 1.0f;
	if (_angle > 360) {
		_angle -= 360;
	}
	for(i=0;i<190;i++)
	{
		if(j[i]<199.0f)
			{j[i]=j[i]+0.5f;
			//cout<<"last"<<j[i]<<endl;
			jop[i] = _terrain -> getHeight(j[i],j[i]);
			jop2[i] = _terrain -> getHeight(j[i]+1.0f,j[i]);
			jop3[i] = _terrain -> getHeight(j[i]-1.0f,j[i]);
			jop4[i] = _terrain -> getHeight(j[i]+2.0f,j[i]);
		}
		else if(j[i]>=199.0f)
		{
			j[i]=0.0f;
			jop[i] = _terrain -> getHeight(j[i],j[i]);
			jop2[i] = _terrain -> getHeight(j[i]+1.0f,j[i]);
			jop3[i] = _terrain -> getHeight(j[i]-1.0f,j[i]);
			jop4[i] = _terrain -> getHeight(j[i]+2.0f,j[i]);
		}

	}
   for(i=0;i<20;i++)
   {   
      float sd = r1[i]-bik_x;
      float ds = r2[i]-bik_z;
      float dis1 = pow(sd,2);
      float dis2 = pow(ds,2);
      float dis3 = sqrt(dis1+dis2);
      if(dis3<0.5)
      {
         r3[i] = 0.0; 
      }
  
   }







	glutPostRedisplay();
	glutTimerFunc(25, update, 0);
}

int main(int argc, char** argv) {
	glutInit(&argc, argv);

	

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(400, 400);
	
	glutCreateWindow("Terrain - videotutorialsrock.com");
	initRendering();
	
	


	_terrain = loadTerrain("heightmap.bmp", 20);


	bik_x=30.0;
	bik_z=20.0;
	angl=0.0f;
	turn=1;
	srand (time(0));
	for(i=0;i<20;i++)
	{ 
		
	
		 r1[i] = ( ( (float) rand()/(float) (RAND_MAX) ) * 100);
		 r2[i] = ( ( (float) rand()/(float) (RAND_MAX) ) * 200);
       r3[i] = 0.75;
	}

	for(i=0;i<190;i++)
	{
	
	
		int k;
		
		k=i;
		j[i]=(float)(k);
		
		jop[i] = _terrain -> getHeight(j[i],j[i]);
		
		jop2[i] = _terrain -> getHeight(j[i]+1.0f,j[i]);

		jop3[i] = _terrain -> getHeight(j[i]-1.0f,j[i]);

		jop4[i] = _terrain -> getHeight(j[i]+2.0f,j[i]);
		
	
	
	
	}

	
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(handleKeypress);
	glutSpecialFunc(handleKeypress2);
	glutReshapeFunc(handleResize);
	glutTimerFunc(25, update, 0);
	
	glutMainLoop();
	return 0;
}









