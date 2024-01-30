#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

int x;
int N = 102;
double y = 1.4;
double t, dx;

double max(double array[], int n)
{
    int i;
    double max = fabs(array[0]);
    for (i = 1; i < n; i++)
        if (fabs(array[i]) > max)
            max = fabs(array[i]);
    return max;
}

double min(double array[], int n)
{
    int i;
    double min = fabs(array[0]);
    for (i = 1; i < n; i++)
        if (fabs(array[i]) < min)
            min = fabs(array[i]);
    return min;
}

void q(double d, double v, double e, double k[])
{
    k[0] = d;
    k[1] = d*v;
    k[2] = e;
}

void f(double d, double v, double e, double p, double k[])
{
    k[0] = d*v;
    k[1] = d*v*v + p;
    k[2] = v*(e + p);
}

double qh(double qr, double qm, double dt, double fm, double fr, double dx)
{
    double qh = 0.5*(qr + qm) + 0.5*(dt/dx)*(fm - fr);
    return qh;
}

double qhfinal(double dt, double fr, double fl, double qm, double dx)
{
    double q = qm - (dt/dx)*(fr - fl);
    return q;
}

void qh_all(double qr[], double qm[], double dt, double fr[], double fm[], double qhr[], double dx){
    qhr[0] = qh(qr[0], qm[0], dt, fm[0], fr[0], dx);
    qhr[1] = qh(qr[1], qm[1], dt, fm[1], fr[1], dx);
    qhr[2] = qh(qr[2], qm[2], dt, fm[2], fr[2], dx);
}

double total_e(double e, double d, double v)
{
    double total_e = e/d - 0.5*v*v;
    return total_e;
}

double fh1, fh2, fh3, denl, denvell, enel, int_el, presl, vell, denr, denvelr, ener, int_er, presr, velr, dt;

int main(){
    t = 0;
    
    FILE *data;
    data = fopen("1a_laxw_fluid_comp.txt", "w");
    double *p = (double*) malloc(N * sizeof(double));
    double *d = (double*) malloc(N * sizeof(double));
    double *v = (double*) malloc(N * sizeof(double));
    double *e = (double*) malloc(N * sizeof(double));
    double *energy = (double*) malloc(N * sizeof(double));
    double *q1 = (double*) malloc(N * sizeof(double));
    double *q2 = (double*) malloc(N * sizeof(double));
    double *q3 = (double*) malloc(N * sizeof(double));
    double *q1n = (double*) malloc(N * sizeof(double));
    double *q2n = (double*) malloc(N * sizeof(double));
    double *q3n = (double*) malloc(N * sizeof(double));
    

    double flux[3][N];
    double qmid[3][N];
    
    double ql[3];
    double qr[3];
    
    double fl[3];
    double fr[3];

    double qh[3];
    double fh[3];

    double dx = 0.01;

    for (x = 0; x < 31; ++x)
      {
          p[x] = 1;
          d[x] = 1;
          v[x] = 0.75;
       
          energy[x] = (p[x])/((d[x])*(y-1));
          e[x] = energy[x]*d[x] + 0.5*v[x]*v[x]*d[x];
      }
    for (x = 31; x < N; ++x)
      {
          p[x] = 0.1;
          d[x] = 0.125;
          v[x] = 0;
          
          energy[x] = (p[x])/((d[x])*(y-1));
          e[x] = energy[x]*d[x] + 0.5*v[x]*v[x]*d[x];
      }
    
    
    while (t < 0.2){

        dt = 0.9*(0.01)/(max(v, N) + sqrt(y*(max(p, N)/min(d, N))));
        
        for (x = 0; x < N; x++)
        {
            q(d[x], v[x], e[x], ql);
            q(d[x+1], v[x+1], e[x+1], qr);
            
            f(d[x], v[x], e[x], p[x], fl);
            f(d[x+1], v[x+1], e[x+1], p[x+1], fr);
            
            qh_all(qr, ql, dt, fr, fl, qh, dx);
          
            f(qh[0], qh[1]/qh[0], qh[2], (y-1)*qh[0]*total_e(qh[2], qh[0], qh[1]/qh[0]), fh);

            flux[0][x] = fh[0];
            flux[1][x] = fh[1];
            flux[2][x] = fh[2];
            
            qmid[0][x] = ql[0];
            qmid[1][x] = ql[1];
            qmid[2][x] = ql[2];
        }
        for (x = 1; x < N-1; x++)
        {
            q1[x] = qhfinal(dt, flux[0][x], flux[0][x-1], qmid[0][x], dx);
            q2[x] = qhfinal(dt, flux[1][x], flux[1][x-1], qmid[1][x], dx);
            q3[x] = qhfinal(dt, flux[2][x], flux[2][x-1], qmid[2][x], dx);
        }

        
        for (x = 1; x < N-1; ++x)
          {
              d[x] = q1[x];
              e[x] = q3[x];
              v[x] = q2[x]/q1[x];
              energy[x] = total_e(e[x], d[x], v[x]);
              p[x] = (y-1)*d[x]*energy[x];
          }
        t = t + dt;
    }
    for (x = 1; x < N-1; ++x)
    {
        fprintf(data,"%d %f %f %f %f\n",x, d[x], v[x], p[x], energy[x]);
        printf("%d %f %f %f %f\n",x, d[x], v[x], p[x], energy[x]);
    }
    free(p);
    free(d);
    free(v);
    free(e);
    free(q1);
    free(q2);
    free(q3);
    free(q1n);
    free(q2n);
    free(q3n);
}
