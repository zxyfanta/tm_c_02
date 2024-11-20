#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//计时
#include <time.h>

#define L0 1//Fourier截止项
#define Nr 1000//网格节点数

const double mm = 1.0;//极向模数
const double nn = 1.0;//环向模数
const double R0 = 4.4364;//大半径
const double Eta0 = 1e-6;//中心处电阻率
const double dr = 1.0 / Nr;//径向步长

const double rs = 0.3;//初始有理面位置
const double q0 = 0.9;//初始中心安全因子
const double Ew = 2 * Eta0 / q0;//边界电场

const double simT = 500;//模拟总时间
const double savT = 50;//数据保存时间
const double dt = 0.01;//模拟步长

int count;//储存文件编号
double Time;//判断是否保存的时间
double r0;//电流通道宽度
double r[Nr + 1];//节点位置
double eta[Nr + 1];//电阻率



//数据矩阵初始化，全部赋值为零
void ini( double data[L0 + 1][Nr + 1] )
{
    for ( int l = 0; l <= L0; l++ )
        for ( int nr = 0; nr <= Nr; nr++ )
            data[l][nr] = 0.0;
}



//Fourier系数共轭，当l<0时由对称性得到（实对称）
double Complex1( double u[L0 + 1][Nr + 1], int l, int nr )
{
    return u[abs( l )][nr];
}



//Fourier系数共轭，当l<0时由对称性得到（反对称）
double Complex2( double u[L0 + 1][Nr + 1], int l, int nr )
{
    return ( ( l >= 0 ) ? 1 : -1 ) * u[abs( l )][nr];
}



//从断点导入数据，重新开始模拟
void InputData( int count, double u1[L0 + 1][Nr + 1], double u2[L0 + 1][Nr + 1] )
{
    char filename[64]; sprintf( filename, "%05d.txt", count );
    FILE *fp = fopen( filename, "r" );
    //跳过前两行
    fscanf( fp, "%*[^\n]%*c" );
    fscanf( fp, "%*[^\n]%*c" );

    //从文件中读取数据
    for ( int nr = 0; nr <= Nr; nr++ )
    {
        for ( int l = 0; l <= L0; l++ )
            fscanf( fp, "%lf", &u1[l][nr] );

        for ( int l = 0; l <= L0; l++ )
            fscanf( fp, "%lf", &u2[l][nr] );
    }

    //关闭文件
    fclose( fp );
}


//导出数据
void OutputData( int count, double u1[L0 + 1][Nr + 1], double u2[L0 + 1][Nr + 1] )
{
    char filename[64]; sprintf( filename, "%05d.txt", count ); //文件不超过1e6
    FILE *fp = fopen( filename, "w" );
    fprintf( fp, "time=%lf\n", count * savT );
    fprintf( fp, "u1[0] u1[1] ... u2[0] u2[1] ...\n" );

    for ( int nr = 0; nr <= Nr; nr++ )
    {
        for ( int l = 0; l <= L0; l++ )
            fprintf( fp, "%e ", u1[l][nr] );

        for ( int l = 0; l <= L0; l++ )
            fprintf( fp, "%e ", u2[l][nr] );

        fprintf( fp, "\n" );
    }

    fclose( fp );
}



//计算一阶中心差分
void Difference( double ux[L0 + 1][Nr + 1], double u[L0 + 1][Nr + 1] )
{
    for ( int l = 0; l <= L0; l++ )
        for ( int nr = 1; nr < Nr; nr++ )
            ux[l][nr] = ( u[l][nr + 1] - u[l][nr - 1] ) / 2.0 / dr;
}



//计算Laplace算子
void Laplace( double Lu[L0 + 1][Nr + 1], double u[L0 + 1][Nr + 1] )
{
    for ( int l = 0; l <= L0; l++ )
    {
        //内点处中心差分
        for ( int nr = 1; nr < Nr; nr++ )
            Lu[l][nr] = ( u[l][nr + 1] - 2 * u[l][nr] + u[l][nr - 1] ) / dr / dr + ( u[l][nr + 1] - u[l][nr - 1] ) / 2.0 / dr / nr / dr - l * l * mm * mm * u[l][nr] / nr / dr / nr / dr;

        //中心处数值边界条件
        Lu[l][0] = ( l == 0 ) ? Lu[l][1] : 0.0;
        //外边界数值边界条件
        Lu[l][Nr] = ( 2 * u[l][Nr] - 5 * u[l][Nr - 1] + 4 * u[l][Nr - 2] - u[l][Nr - 3] ) / dr / dr + ( 3 * u[l][Nr] - 4 * u[l][Nr - 1] + u[l][Nr - 2] ) / 2 / dr - mm * mm * l * l * u[l][Nr];
    }
}



//计算源项
void CalculatS( double S1[L0 + 1][Nr + 1], double S2[L0 + 1][Nr + 1], double u1[L0 + 1][Nr + 1], double u2[L0 + 1][Nr + 1] )
{
    //初始化返回值
    ini( S1 ); ini( S2 );
    //计算Laplace算子的值
    double f1[L0 + 1][Nr + 1], f2[L0 + 1][Nr + 1];
    Laplace( f1, u1 ); Laplace( f2, u2 );
    //计算一阶差分算子的值
    double u1x[L0 + 1][Nr + 1], u2x[L0 + 1][Nr + 1], f1x[L0 + 1][Nr + 1], f2x[L0 + 1][Nr + 1];
    Difference( u1x, u1 ); Difference( u2x, u2 ); Difference( f1x, f1 ); Difference( f2x, f2 );

    for ( int nr = 1; nr < Nr; nr++ )
    {
        //当l=0时，只需要计算S1，因为此时S2等于零
        S1[0][nr] =  2 * eta[nr] * nn / mm + eta[nr] * f1[0][nr] - Ew;

        //计算卷积项，当l<0时由共轭条件取值
        for ( int k = -L0; k <= L0; k++ )
            S1[0][nr] -= 1 / R0 / r[nr] * mm * k * ( Complex2( u2, k, nr ) * Complex1( u1x, -k, nr ) - Complex1( u1, k, nr ) * Complex2( u2x, -k, nr ) );
    }

    for ( int l = 1; l <= L0; l++ )
        for ( int nr = 1; nr < Nr; nr++ )
        {
            //计算非卷积项
            S1[l][nr] =  eta[nr] * f1[l][nr];

            //计算卷积项，当l<0时由共轭条件取值
            for ( int k = l - L0; k <= L0; k++ )
            {
                S1[l][nr] -= 1 / R0 / r[nr] * mm * k * ( Complex2( u2, k, nr ) * Complex1( u1x, l - k, nr ) - Complex1( u1, k, nr ) * Complex2( u2x, l - k, nr ) );
                S2[l][nr] += 1 / R0 / r[nr] * mm * k * ( Complex1( f1, k, nr ) * Complex1( u1x, l - k, nr ) - Complex1( u1, k, nr ) * Complex1( f1x, l - k, nr ) );
                S2[l][nr] += 1 / R0 / r[nr] * mm * k * ( Complex2( f2, k, nr ) * Complex2( u2x, l - k, nr ) - Complex2( u2, k, nr ) * Complex2( f2x, l - k, nr ) );
            }
        }
}



//时间推进
void EvaluateU( double nu1[L0 + 1][Nr + 1], double nu2[L0 + 1][Nr + 1], double u1[L0 + 1][Nr + 1], double u2[L0 + 1][Nr + 1], double S1[L0 + 1][Nr + 1], double S2[L0 + 1][Nr + 1], double Dt )
{
    //u1推进，推进为nu1
    for ( int l = 0; l <= L0; l++ )
    {
        for ( int nr = 1; nr < Nr; nr++ )
            nu1[l][nr] = u1[l][nr] + Dt * S1[l][nr];

        //中心处数值边界条件
        nu1[l][0] = ( l == 0 ) ? nu1[l][1] : 0;
        //外边界数值边界条件
        nu1[l][Nr] = 0.0;
    }

    //u2推进，追赶法推进为nu2
    double du2[L0 + 1][Nr + 1]; ini( du2 );

    for ( int l = 1; l <= L0; l++ )
    {
        //追赶法的系数矩阵
        double x[Nr + 1]; double L[Nr + 1], D[Nr + 1], R[Nr + 1];

        for ( int nr = 1; nr < Nr; nr++ )
        {
            L[nr] = 1.0 - 0.5 / nr;
            D[nr] = -2.0 - mm * mm * l * l / nr / nr;
            R[nr] = 1.0 + 0.5 / nr;
        }

        //求已知向量
        for ( int nr = 1; nr < Nr; nr++ )
            x[nr] = Dt * dr * dr * S2[l][nr];

        //追赶法求未知向量
        double u[Nr], v[Nr], w[Nr];
        u[1] = D[1]; v[1] = R[1] / u[1]; w[1] = x[1] / u[1];

        //追
        for ( int nr = 2; nr < Nr; nr++ )
        {
            u[nr] = D[nr] - L[nr] * v[nr - 1];
            v[nr] = R[nr] / u[nr];
            w[nr] = ( x[nr] - L[nr] * w[nr - 1] ) / u[nr];
        }

        //赶
        du2[l][Nr - 1] =  w[Nr - 1];

        for ( int nr = Nr - 2; nr >= 1; nr-- )
            du2[l][nr] = w[nr] - v[nr] * du2[l][nr + 1];
    }

    for ( int l = 1; l <= L0; l++ )
    {
        for ( int nr = 1; nr < Nr; nr++ )
            nu2[l][nr] = u2[l][nr] + du2[l][nr];

        //中心处数值边界条件
        nu2[l][0] = 0.0;
        //外边界数值边界条件
        nu2[l][Nr] = 0.0;
    }
}



//模拟初始化，模拟开始时计算一次
void Initialize( double u1[L0 + 1][Nr + 1], double u2[L0 + 1][Nr + 1] )
{
    //时间统计归零
    Time = 0; count = 0;
    //计算电流通道宽度
    r0 = ( rs * sqrt( nn * q0 / ( mm - nn * q0 ) ) );
    ini( u1 ); ini( u2 );

    for ( int nr = 0; nr <= Nr; nr++ )
    {
        //节点计算
        r[nr] = nr * dr;
        //电阻率计算
        eta[nr] = Eta0 * ( 1.0 + r[nr] * r[nr] / r0 / r0 ) * ( 1.0 + r[nr] * r[nr] / r0 / r0 );
        //平衡磁通
        u1[0][nr] = r0 * r0 / 2.0 / q0 * ( log( 1.0 + r[nr] * r[nr] / r0 / r0 ) - log( 1.0 + 1.0 / r0 / r0 ) ) - ( nn / mm / 2.0 * r[nr] * r[nr] - nn / mm / 2.0 );
        //扰动磁通
        u1[1][nr] = 1e-9 * r[nr] * r[nr] * ( 1 - r[nr] * r[nr] ) * ( 1 - r[nr] * r[nr] );
    }

    //count=1000;InputData(count,u1,u2);//从断点开始重新计算
}

int main()
{
    clock_t start,end;
    double cpu_time_used;
    start=clock();
    
    double u1[L0 + 1][Nr + 1], u2[L0 + 1][Nr + 1]; ini( u1 ); ini( u2 );
    double pu1[L0 + 1][Nr + 1], pu2[L0 + 1][Nr + 1]; ini( pu1 ); ini( pu2 );
    double S1[L0 + 1][Nr + 1], S2[L0 + 1][Nr + 1]; ini( S1 ); ini( S2 );
    //模拟初始化
    Initialize( u1, u2 );
    //导出模拟初始数据
    OutputData( count, u1, u2 );

    while ( count * savT < simT )
    {
        //step1
        CalculatS( S1, S2, u1, u2 );
        EvaluateU( pu1, pu2, u1, u2, S1, S2, dt / 2 );
        //step2
        CalculatS( S1, S2, pu1, pu2 );
        EvaluateU( u1, u2, u1, u2, S1, S2, dt );
        //时间推进，输出数据
        Time += dt;

        //判断是否保存
        if ( Time > savT )
        {
            count += 1; Time = 0.0;
            // OutputData( count, u1, u2 );
        }
    }
    end=clock();
    cpu_time_used=((double)(end-start))/CLOCKS_PER_SEC;
    printf("%f",cpu_time_used);
}
