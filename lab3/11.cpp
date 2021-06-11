
#include <math.h>                                  // для функции Sin()



float points[ 45 ][ 45 ][3]; // массив точек сетки нашей "волны"

int wiggle_count = 0;        // счетчик для контроля быстроты развевания флага

GLfloat hold;                // временно содержит число с плавающей запятой

  

if (TextureImage[0]=LoadBMP("Data/Tim.bmp"))       // загружаем изображение

  
 

glPolygonMode( GL_BACK, GL_FILL );       // нижняя (задняя) сторона заполнена

glPolygonMode( GL_FRONT, GL_LINE );      // верхняя (передняя) сторона прорисована линиями

 


 

// пройдемся по плоскости X

      for(int x=0; x<45; x++)

      {

            // пройдемся по плоскости Y

            for(int y=0; y<45; y++)

            {

                  // применим волну к нашей сетке

                  points[x][y][0]=float((x/5.0f)-4.5f);

                  points[x][y][1]=float((y/5.0f)-4.5f);

                  points[x][y][2]=float(sin((((x/5.0f)*40.0f)/360.0f)*3.141592654*2.0f));

            }

      }

 

int DrawGLScene(GLvoid)                          // рисуем нашу сцену

{

      int x, y;                                  // переменные циклов

      // для разбиения флага на маленькие квадраты

      float float_x, float_y, float_xb, float_yb;

  

glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // очистить экран и буфер глубины

glLoadIdentity();                                   // сброс текущей матрицы

 

glTranslatef(0.0f,0.0f,-12.0f);                     // перенести 17 единиц в глубь экрана

 

glRotatef(xrot,1.0f,0.0f,0.0f);                     // вращение по оси X

glRotatef(yrot,0.0f,1.0f,0.0f);                     // вращение по оси Y 

glRotatef(zrot,0.0f,0.0f,1.0f);                     // вращение по оси Z

 

glBindTexture(GL_TEXTURE_2D, texture[0]);           // выбрать нашу текстуру

  

glBegin(GL_QUADS);              // начинаем рисовать квадраты

      for( x = 0; x < 44; x++ ) // пройдемся по плоскости X 0-44 (45 точек)

      {

            for( y = 0; y < 44; y++ ) // пройдемся по плоскости Y 0-44 (45 точек)

            {

 
 

float_x = float(x)/44.0f;         // создать значение X как float

float_y = float(y)/44.0f;         // создать значение Y как float

float_xb = float(x+1)/44.0f;      // создать значение X как float плюс 0.0227f

float_yb = float(y+1)/44.0f;      // создать значение Y как float плюс 0.0227f

 


 

// первая координата текстуры (нижняя левая)

         glTexCoord2f( float_x, float_y);  

         glVertex3f( points[x][y][0], points[x][y][1], points[x][y][2] );

                 

      // вторая координата текстуры (верхняя левая)

         glTexCoord2f( float_x, float_yb );

         glVertex3f( points[x][y+1][0], points[x][y+1][1], points[x][y+1][2]);

 

// третья координата текстуры (верхняя правая)                

         glTexCoord2f( float_xb, float_yb );

glVertex3f( points[x+1][y+1][0], points[x+1][y+1][1], points[x+1][y+1][2]);

                 

            // четвертая координата текстуры (нижняя правая)

         glTexCoord2f( float_xb, float_y );

         glVertex3f( points[x+1][y][0], points[x+1][y][1], points[x+1][y][2]);

      }

}

glEnd();                                  // закончили с квадратами

  
 

if( wiggle_count == 2 )      // для замедления волны (только каждый второй кадр)

      {

  

      for( y = 0; y < 45; y++ )           // пройдемся по плоскости Y

      {

// сохраним текущее значение одной точки левой стороны волны

            hold=points[0][y][2];

            for( x = 0; x < 44; x++)      // пройдемся по плоскости X

            {

                 // текущее значение волны равно значению справа

                 points[x][y][2] = points[x+1][y][2];

            }

      // последнее значение берется из дальнего левого сохраненного значения

            points[44][y][2]=hold;

      }

      wiggle_count = 0;                    // снова сбросить счетчик

}

wiggle_count++;                            // увеличить счетчик

  
 

xrot+=0.3f;             // увеличить значение переменной вращения по X

yrot+=0.2f;             // увеличить значение переменной вращения по Y

zrot+=0.4f;             // увеличить значение переменной вращения по Z

 

return TRUE;            // возврат из функции

}



