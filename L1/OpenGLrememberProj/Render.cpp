

#include "Render.h"

#include <Windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

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


//Лаба 1
void Render(double delta_time)
{
	//Массив точек
	double A[] = { 0, 0, 0 };
	double B[] = { 3, 7, 0 };
	double C[] = { -1, 8, 0 };
	double D[] = { 3, 12, 0 };
	double E[] = { 6, 5, 0 };
	double F[] = { 8, 12, 0 };
	double G[] = { 13, 13, 0 };
	double H[] = { 6, 0, 0 };
	//Вспомогательная точка S для выпуклости
	double S[] = { -2.2, 13.2, 0 };
	//Обозначеная точка M и концы векторов для впадины	
	double M[] = { 4, 1.5, 0 };
	double Mass7End[] = { 5.5, 1.5, 0 };
	double MEnd[] = { 2.5, 1.5, 0 };
	double Mass0End[] = { -2, -5.5, 0 };

	double* mass[8] = { A,B,C,D,E,F,G,H };

	//Стены
	glBegin(GL_QUADS);
	glColor3d(0.3, 0.3, 0.5);

	for (int i = 0; i < 7; i++)
	{
		if (i == 2)//Стена точки S
		{
			double next[3];
			double prev[] = { mass[i][0], mass[i][1], mass[i][2] };

			for (double t = 0; t <= 1.0001; t += 0.01)
			{
				double prevY[3] = { prev[0], prev[1], prev[2] + 2 };

				next[0] = fL1(mass[i][0], S[0], mass[i + 1][0], t);
				next[1] = fL1(mass[i][1], S[1], mass[i + 1][1], t);
				next[2] = fL1(mass[i][2], S[2], mass[i + 1][2], t);
				double nextY[3] = { next[0], next[1], next[2] + 2 };

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

		glVertex3dv(mass[i]);
		glVertex3dv(mass[i + 1]);
		glVertex3dv(p2);
		glVertex3dv(p1);

	}
	//Стена 7-M
	{
		double next[3];
		double prev[] = { mass[7][0], mass[7][1], mass[7][2] };

		for (double t = 0; t <= 1.0001; t += 0.01)
		{
			double prevY[3] = { prev[0], prev[1], prev[2] + 2 };

			next[0] = ff(mass[7][0], M[0], Mass7End[0] - mass[7][0], MEnd[0] - M[0], t);
			next[1] = ff(mass[7][1], M[1], Mass7End[1] - mass[7][1], MEnd[1] - M[1], t);
			next[2] = ff(mass[7][2], M[2], Mass7End[2] - mass[7][2], MEnd[2] - M[2], t);
			double nextY[3] = { next[0], next[1], next[2] + 2 };

			glVertex3dv(prev);
			glVertex3dv(prevY);
			glVertex3dv(nextY);
			glVertex3dv(next);

			prev[0] = next[0];
			prev[1] = next[1];
			prev[2] = next[2];
		}
	}
	//Стена M-0
	{
		double next[3];
		double prev[] = { M[0], M[1], M[2] };

		for (double t = 0; t <= 1.0001; t += 0.01)
		{
			double prevY[3] = { prev[0], prev[1], prev[2] + 2 };

			next[0] = ff(M[0], mass[0][0], MEnd[0] - M[0], Mass0End[0] - mass[0][0], t);
			next[1] = ff(M[1], mass[0][1], MEnd[1] - M[1], Mass0End[1] - mass[0][1], t);
			next[2] = ff(M[2], mass[0][2], MEnd[2] - M[2], Mass0End[2] - mass[0][2], t);
			double nextY[3] = { next[0], next[1], next[2] + 2 };

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

	//Нижняя поверхность
	glBegin(GL_POLYGON);
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

	//Верхняя поверхность
	for (int i = 0; i < 8; i++)
		mass[i][2] += 2;
	S[2] += 2;
	M[2] += 2;
	Mass7End[2] += 2;
	MEnd[2] += 2;
	Mass0End[2] += 2;

	glBegin(GL_POLYGON);
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
}

//Лаба 1 (Прямые стены (недоработаный вариант, зачем-то сохранил отдельно) )
/*
void Render(double delta_time)
{
	//Массив точек
	double A[] = { 0, 0, 0 };
	double B[] = { 3,7, 0 };
	double C[] = { -1,8, 0 };
	double D[] = { 3,12, 0 };
	double E[] = { 6,5, 0 };
	double F[] = { 8,12, 0 };
	double G[] = { 13,13, 0 };
	double H[] = { 6,0, 0 };

	double* mass[8] = { A,B,C,D,E,F,G,H };

	//Стены
	glBegin(GL_QUADS);
	glColor3d(0.3, 0.3, 0.5);

	for (int i = 0; i < 7; i++)
	{
		double p1[3] = { mass[i][0], mass[i][1], mass[i][2] + 2 };
		double p2[3] = { mass[i+1][0], mass[i+1][1], mass[i+1][2] + 2 };

		glVertex3dv(mass[i]);
		glVertex3dv(mass[i + 1]);
		glVertex3dv(p2);
		glVertex3dv(p1);

	}

	double p1[3] = { mass[7][0], mass[7][1], mass[7][2] + 2 };
	double p2[3] = { mass[0][0], mass[0][1], mass[0][2] + 2 };

	glVertex3dv(mass[7]);
	glVertex3dv(mass[0]);
	glVertex3dv(p2);
	glVertex3dv(p1);

	glEnd();

	//Нижняя поверхность
	glBegin(GL_POLYGON);
	glColor3d(0.6, 0.6, 1);
	glVertex3dv(mass[0]);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[7]);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[2]);
	glVertex3dv(mass[3]);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[3]);
	glVertex3dv(mass[4]);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[4]);
	glVertex3dv(mass[7]);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[4]);
	glVertex3dv(mass[5]);
	glVertex3dv(mass[6]);
	glVertex3dv(mass[7]);
	glEnd();


	//Верхняя поверхность
	for (int i = 0; i < 8; i++)
		mass[i][2] += 2;

	glBegin(GL_POLYGON);
	glColor3d(0.15, 0.15, 0.25);
	glVertex3dv(mass[0]);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[7]);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[2]);
	glVertex3dv(mass[3]);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[3]);
	glVertex3dv(mass[4]);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[1]);
	glVertex3dv(mass[4]);
	glVertex3dv(mass[7]);
	glEnd();
	glBegin(GL_POLYGON);
	glVertex3dv(mass[4]);
	glVertex3dv(mass[5]);
	glVertex3dv(mass[6]);
	glVertex3dv(mass[7]);
	glEnd();


}
*/

//Лаба 3 (сделанное на паре лабы на 30)
/*
void Render(double delta_time)
{
	t_max += delta_time / 3;
	if (t_max > 1)
		t_max = 0;
	double P11[] = { -1,-1,-1 };
	double P22[] = { 3,3,1 };
	double P33[] = { 10,1,1 };
	double P44[] = { 10,10,1 };
	double P1[] = { 0,0,0 };
	double P2[] = { 10,10,10 };
	double P3[] = { 0,0,10 };
	double P4[] = { 10,0,0 };
	glLineWidth(2);
	glColor3b(100, 255, 50);
	glBegin(GL_LINE_STRIP);
	for (double t = 0; t <= 1.0001; t += 0.01)
	{
		double P[3];
		P[0] = f(P1[0], P2[0], P3[0], P4[0], t);
		P[1] = f(P1[1], P2[1], P3[1], P4[1], t);
		P[2] = f(P1[2], P2[2], P3[2], P4[2], t);
		glVertex3dv(P);
	}
	glEnd();
	glColor3b(255, 50, 100);
	glBegin(GL_LINE_STRIP);
	for (double t = 0; t <= 1.0001; t += 0.01)
	{
		double P[3];
		P[0] = f(P11[0], P22[0], P33[0], P44[0], t);
		P[1] = f(P11[1], P22[1], P33[1], P44[1], t);
		P[2] = f(P11[2], P22[2], P33[2], P44[2], t);
		glVertex3dv(P);
	}
	glEnd();
	glLineWidth(1);
	glColor3b(100, 255, 50);
	glBegin(GL_LINE_STRIP);
	glVertex3dv(P1);
	glVertex3dv(P2);
	glVertex3dv(P3);
	glVertex3dv(P4);
	glEnd();
	glColor3b(255, 50, 100);
	glBegin(GL_LINE_STRIP);
	glVertex3dv(P11);
	glVertex3dv(P22);
	glVertex3dv(P33);
	glVertex3dv(P44);
	glEnd();
	//Ermit's:
	double R1b[] = { 0,0,3 };
	double R1e[] = { 2,2,16 };
	double R2b[] = { 3,3,5 };
	double R2e[] = { 4,4,8 };
	glLineWidth(2);
	glColor3b(10, 120, 120);
	glBegin(GL_LINE_STRIP);
	for (double t = 0; t <= 1.0001; t += 0.01)
	{
		double P[3];
		P[0] = ff(R1b[0], R2b[0], R1e[0] - R1b[0], R2e[0] - R2b[0], t);
		P[1] = ff(R1b[1], R2b[1], R1e[1] - R1b[1], R2e[1] - R2b[1], t);
		P[2] = ff(R1b[2], R2b[2], R1e[2] - R1b[2], R2e[2] - R2b[2], t);
		glVertex3dv(P);
	}
	glEnd();
	glLineWidth(1);
	glBegin(GL_LINE_STRIP);
	glVertex3dv(R1b);
	glVertex3dv(R1e);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3dv(R2b);
	glVertex3dv(R2e);
	glEnd();
	//Ermit's 2:
	double R11b[] = { 0,10,10 };
	double R11e[] = { 5,15,15 };
	double R22b[] = { 8,9,9 };
	double R22e[] = { 3,3,3 };
	glLineWidth(2);
	glColor3b(1, 1, 1);
	glBegin(GL_LINE_STRIP);
	for (double t = 0; t <= 1.0001; t += 0.01)
	{
		double P[3];
		P[0] = ff(R11b[0], R22b[0], R11e[0] - R11b[0], R22e[0] - R22b[0], t);
		P[1] = ff(R11b[1], R22b[1], R11e[1] - R11b[1], R22e[1] - R22b[1], t);
		P[2] = ff(R11b[2], R22b[2], R11e[2] - R11b[2], R22e[2] - R22b[2], t);
		glVertex3dv(P);
	}
	glEnd();
	glLineWidth(1);
	glBegin(GL_LINE_STRIP);
	glVertex3dv(R11b);
	glVertex3dv(R11e);
	glEnd();
	glBegin(GL_LINE_STRIP);
	glVertex3dv(R22b);
	glVertex3dv(R22e);
	glEnd();
}*/

