#include "Render.h"

#include <sstream>
#include <iostream>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"


inline double fL1(double p1, double p2, double p3, double t)
{
	return p1 * (1 - t) * (1 - t) + 2 * p2 * t * (1 - t) + p3 * t * t;
}

inline double f(double p1, double p2, double p3, double p4, double t)
{
	return p1 * (1 - t) * (1 - t) * (1 - t) + 3 * p2 * t * (1 - t) * (1 - t) + 3 * p3 * t * t * (1 - t) + p4 * t * t * t;
}
inline double ff(double p1, double p2, double r1, double r2, double t)
{
	return p1 * (2 * t * t * t - 3 * t * t + 1) + p2 * (-2 * t * t * t + 3 * t * t) + r1 * (t * t * t - 2 * t * t + t) + r2 * (t * t * t - t * t);
}

double t_max = 0;

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;


	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}


	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist * cos(fi2) * cos(fi1),
			camDist * cos(fi2) * sin(fi1),
			camDist * sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}


	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);


		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale * 0.08;
		s.Show();

		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale * 1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL* ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01 * dx;
		camera.fi2 += -0.01 * dy;
	}


	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k * r.direction.X() + r.origin.X();
		y = k * r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02 * dy);
	}


}

void mouseWheelEvent(OpenGL* ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01 * delta;

}

void keyDownEvent(OpenGL* ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL* ogl, int key)
{

}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL* ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);


	//������ ����������� ���������  (R G B)
	RGBTRIPLE* texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char* texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);



	//���������� �� ��� ��������
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH);


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}





void Render(OpenGL* ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  


	//������ ��������� 
	/*double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();*/
	//������ �����
	double A[] = { 0, 0, 0 };
	double B[] = { 3, 7, 0 };
	double C[] = { -1, 8, 0 };
	double D[] = { 3, 12, 0 };
	double E[] = { 6, 5, 0 };
	double F[] = { 8, 12, 0 };
	double G[] = { 13, 13, 0 };
	double H[] = { 6, 0, 0 };
	//��������������� ����� S ��� ����������
	double S[] = { -2.2, 13.2, 0 };
	//����������� ����� M � ����� �������� ��� �������	
	double M[] = { 4, 1.5, 0 };
	double Mass7End[] = { 5.5, 1.5, 0 };
	double MEnd[] = { 2.5, 1.5, 0 };
	double Mass0End[] = { -2, -5.5, 0 };

	//�������
	double N1[3], N2[3], N3[3], N4[3], N5[3], N6[3], N7[3], N8[3];
	double* Normals[] = { N1, N2, N3, N4, N5, N6, N7, N8 };
	double v1[3], v2[] = { 0, 0, 2 };


	double* mass[8] = { A,B,C,D,E,F,G,H };

	//�����
	glBegin(GL_QUADS);
	glColor3d(0.3, 0.3, 0.5);

	for (int i = 0; i < 7; i++)
	{
		//������� �������
		v1[0] = mass[i][0] - mass[i + 1][0];
		v1[1] = mass[i][1] - mass[i + 1][1];
		v1[2] = mass[i][2] - mass[i + 1][2];

		Normals[i][0] = v1[1] * v2[2] - v2[1] * v1[2];
		Normals[i][1] = -v1[0] * v2[2] + v2[0] * v1[2];
		Normals[i][2] = v1[0] * v2[1] - v2[0] * v1[1];
		//
		if (i == 2)//����� ����� S
		{
			double N[3];
			double next[3];
			double prev[] = { mass[i][0], mass[i][1], mass[i][2] };

			for (double t = 0; t <= 1.0001; t += 0.01)
			{
				double prevY[3] = { prev[0], prev[1], prev[2] + 2 };

				next[0] = fL1(mass[i][0], S[0], mass[i + 1][0], t);
				next[1] = fL1(mass[i][1], S[1], mass[i + 1][1], t);
				next[2] = fL1(mass[i][2], S[2], mass[i + 1][2], t);
				double nextY[3] = { next[0], next[1], next[2] + 2 };

				//������� �������
				v1[0] = prev[0] - next[0];
				v1[1] = prev[1] - next[1];
				v1[2] = prev[2] - next[2];
				N[0] = v1[1] * v2[2] - v2[1] * v1[2];
				N[1] = -v1[0] * v2[2] + v2[0] * v1[2];
				N[2] = v1[0] * v2[1] - v2[0] * v1[1];
				glNormal3dv(N);

				glVertex3dv(prev);
				glVertex3dv(prevY);
				glVertex3dv(nextY);
				glVertex3dv(next);

				prev[0] = next[0];
				prev[1] = next[1];
				prev[2] = next[2];

			}
			continue;
		}
		double p1[3] = { mass[i][0], mass[i][1], mass[i][2] + 2 };
		double p2[3] = { mass[i + 1][0], mass[i + 1][1], mass[i + 1][2] + 2 };

		glNormal3dv(Normals[i]);
		glEnable(GL_NORMALIZE);
		glVertex3dv(mass[i]);
		glVertex3dv(mass[i + 1]);
		glVertex3dv(p2);
		glVertex3dv(p1);

	}
	//����� 7-M
	{
		double N[3];
		double next[3];
		double prev[] = { mass[7][0], mass[7][1], mass[7][2] };

		for (double t = 0; t <= 1.0001; t += 0.01)
		{
			double prevY[3] = { prev[0], prev[1], prev[2] + 2 };

			next[0] = ff(mass[7][0], M[0], Mass7End[0] - mass[7][0], MEnd[0] - M[0], t);
			next[1] = ff(mass[7][1], M[1], Mass7End[1] - mass[7][1], MEnd[1] - M[1], t);
			next[2] = ff(mass[7][2], M[2], Mass7End[2] - mass[7][2], MEnd[2] - M[2], t);
			double nextY[3] = { next[0], next[1], next[2] + 2 };

			//������� �������
			v1[0] = prev[0] - next[0];
			v1[1] = prev[1] - next[1];
			v1[2] = prev[2] - next[2];
			N[0] = v1[1] * v2[2] - v2[1] * v1[2];
			N[1] = -v1[0] * v2[2] + v2[0] * v1[2];
			N[2] = v1[0] * v2[1] - v2[0] * v1[1];
			glNormal3dv(N);

			glVertex3dv(prev);
			glVertex3dv(prevY);
			glVertex3dv(nextY);
			glVertex3dv(next);

			prev[0] = next[0];
			prev[1] = next[1];
			prev[2] = next[2];
		}
	}
	//����� M-0
	{
		double N[3];
		double next[3];
		double prev[] = { M[0], M[1], M[2] };

		for (double t = 0; t <= 1.0001; t += 0.01)
		{
			double prevY[3] = { prev[0], prev[1], prev[2] + 2 };

			next[0] = ff(M[0], mass[0][0], MEnd[0] - M[0], Mass0End[0] - mass[0][0], t);
			next[1] = ff(M[1], mass[0][1], MEnd[1] - M[1], Mass0End[1] - mass[0][1], t);
			next[2] = ff(M[2], mass[0][2], MEnd[2] - M[2], Mass0End[2] - mass[0][2], t);
			double nextY[3] = { next[0], next[1], next[2] + 2 };

			//������� �������
			v1[0] = prev[0] - next[0];
			v1[1] = prev[1] - next[1];
			v1[2] = prev[2] - next[2];
			N[0] = v1[1] * v2[2] - v2[1] * v1[2];
			N[1] = -v1[0] * v2[2] + v2[0] * v1[2];
			N[2] = v1[0] * v2[1] - v2[0] * v1[1];
			glNormal3dv(N);

			glVertex3dv(prev);
			glVertex3dv(prevY);
			glVertex3dv(nextY);
			glVertex3dv(next);

			prev[0] = next[0];
			prev[1] = next[1];
			prev[2] = next[2];
		}
	}
	glEnd();

	//������ �����������
	glBegin(GL_POLYGON);
	glNormal3d(0, 0, -1);
	glColor3d(0.6, 0.6, 1);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[2]);
	glVertex3dv(mass[3]);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[3]);
	glVertex3dv(mass[4]);
	glVertex3dv(M);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[4]);
	glVertex3dv(mass[5]);
	glVertex3dv(mass[6]);
	glVertex3dv(mass[7]);
	glEnd();
	//S
	{
		glBegin(GL_TRIANGLE_FAN);
		glVertex3dv(mass[2]);
		double next[3];
		for (double t = 0; t <= 1.0001; t += 0.01)
		{
			next[0] = fL1(mass[2][0], S[0], mass[3][0], t);
			next[1] = fL1(mass[2][1], S[1], mass[3][1], t);
			next[2] = fL1(mass[2][2], S[2], mass[3][2], t);
			glVertex3dv(next);
		}
		glEnd();
	}
	//7-M
	{
		glBegin(GL_TRIANGLE_FAN);
		glVertex3dv(mass[4]);
		double next[3];
		for (double t = 0; t <= 1.0001; t += 0.01)
		{
			next[0] = ff(mass[7][0], M[0], Mass7End[0] - mass[7][0], MEnd[0] - M[0], t);
			next[1] = ff(mass[7][1], M[1], Mass7End[1] - mass[7][1], MEnd[1] - M[1], t);
			next[2] = ff(mass[7][2], M[2], Mass7End[2] - mass[7][2], MEnd[2] - M[2], t);
			glVertex3dv(next);
		}
		glEnd();
	}
	//M-0
	{
		glBegin(GL_TRIANGLE_FAN);
		glVertex3dv(mass[1]);
		double next[3];
		for (double t = 0; t <= 1.0001; t += 0.01)
		{
			next[0] = ff(M[0], mass[0][0], MEnd[0] - M[0], Mass0End[0] - mass[0][0], t);
			next[1] = ff(M[1], mass[0][1], MEnd[1] - M[1], Mass0End[1] - mass[0][1], t);
			next[2] = ff(M[2], mass[0][2], MEnd[2] - M[2], Mass0End[2] - mass[0][2], t);
			glVertex3dv(next);
		}
		glEnd();
	}

	//������� �����������
	for (int i = 0; i < 8; i++)
		mass[i][2] += 2;
	S[2] += 2;
	M[2] += 2;
	Mass7End[2] += 2;
	MEnd[2] += 2;
	Mass0End[2] += 2;

	glBegin(GL_POLYGON);
	glNormal3d(0, 0, 1);
	glColor3d(0.15, 0.15, 0.25);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[2]);
	glVertex3dv(mass[3]);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[3]);
	glVertex3dv(mass[4]);
	glVertex3dv(M);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[4]);
	glVertex3dv(mass[5]);
	glVertex3dv(mass[6]);
	glVertex3dv(mass[7]);
	glEnd();
	//S
	{
		glBegin(GL_TRIANGLE_FAN);
		glVertex3dv(mass[2]);
		double next[3];
		for (double t = 0; t <= 1.0001; t += 0.01)
		{
			next[0] = fL1(mass[2][0], S[0], mass[3][0], t);
			next[1] = fL1(mass[2][1], S[1], mass[3][1], t);
			next[2] = fL1(mass[2][2], S[2], mass[3][2], t);
			glVertex3dv(next);
		}
		glEnd();
	}
	//7-M
	{
		glBegin(GL_TRIANGLE_FAN);
		glVertex3dv(mass[4]);
		double next[3];
		for (double t = 0; t <= 1.0001; t += 0.01)
		{
			next[0] = ff(mass[7][0], M[0], Mass7End[0] - mass[7][0], MEnd[0] - M[0], t);
			next[1] = ff(mass[7][1], M[1], Mass7End[1] - mass[7][1], MEnd[1] - M[1], t);
			next[2] = ff(mass[7][2], M[2], Mass7End[2] - mass[7][2], MEnd[2] - M[2], t);
			glVertex3dv(next);
		}
		glEnd();
	}
	//M-0
	{
		glBegin(GL_TRIANGLE_FAN);
		glVertex3dv(mass[1]);
		double next[3];
		for (double t = 0; t <= 1.0001; t += 0.01)
		{
			next[0] = ff(M[0], mass[0][0], MEnd[0] - M[0], Mass0End[0] - mass[0][0], t);
			next[1] = ff(M[1], mass[0][1], MEnd[1] - M[1], Mass0End[1] - mass[0][1], t);
			next[2] = ff(M[2], mass[0][2], MEnd[2] - M[2], Mass0End[2] - mass[0][2], t);
			glVertex3dv(next);
		}
		glEnd();
	}
	//������� �������





	//����� ���������

   //��������� ������ ������


	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
									//(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R=" << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;

	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}