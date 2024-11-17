#include <glut.h>
#include <math.h>
#include <string>
#include <random>
//#include <Windows.h>
#include <cstdlib>
#include <ctime>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
using namespace std;

GLuint textureID;	

struct Toado
{
	int x, y;	
};
struct Thienthach
{
	float tocdo, dixuong, dilen;
	int chimucmaxx, chimucminy, chimucmaxy;
	Toado D[8];
};

Toado V[13]; // Tọa độ vẽ tàu bay
Thienthach T1[20]; // Mảng chứa thiên thạch
float dd = 12, d = 9; // Tịnh tiến cho cục thiên thạch, Tịnh tiến cho tàu bay
bool baohieu = false; // Báo hiệu để game chạy 
bool replay = false; // Bắt đầu lại trò chơi
bool pause = false; // Dừng trò chơi
bool vuno = false;
bool nhanlen = false, nhanxuong = false, nhantrai = false, nhanphai = false;   // ĐIều kiện để tàu bay di chuyển lên xuống trái phái 
int sott1 = 0; // Chỉ mục của các cục thiên thạch
int diem = 0; // Biến tính điểm 
float centerX = 0, centerY = 0; // Vị trí của vụ nổ
const int numPoints = 100;
float explosionRadius = 100.0f;  // Tăng kích thước vụ nổ lên 80
float points[numPoints][2];
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> distra(0, 19);


void initGL()
{
    glClearColor(0.06f, 0.0f, 0.06f, 0.0f); 
    glOrtho(-1600,1600,-800, 800,-1,1);
}
// Tạo ảnh nền cho trò chơi
void loadTexture(const char* filePath)
{
    int width, height, channels;
    unsigned char* image = stbi_load(filePath, &width, &height, &channels, STBI_rgb_alpha);
 
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(image);
}
void drawBackground()
{
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f); glVertex2f(3200.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f); glVertex2f(3200.0f, 1600.0f);
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, 1600.0f);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}
// Kết thúc
// Thuật toán va chạm
long dtStamgiac(int xA, int yA, int xB, int yB, int xC, int yC)
{
	return fabs((xB*yA - xA*yB) + (xC*yB - xB*yC) + (xA*yC - xC*yA));
}
long dtSdagiac(int xI, int yI)
{
	long tongdt = 0;
	
	for (int i = 0; i < 8; i++)
	{
		int j = i + 1;
		if (i == 7) 
		{
			j = 0;
		}
		
		tongdt += dtStamgiac(xI, yI, T1[sott1].D[i].x, T1[sott1].D[i].y, T1[sott1].D[j].x, T1[sott1].D[j].y);
	}
	
	return tongdt;
}
bool kiemtraPthuocS(int xI, int yI)
{
 	if (dtSdagiac(xI, yI) == dtSdagiac(T1[sott1].D[0].x, T1[sott1].D[0].y))
	{
		return true;
	}
	
	return false; 
} 
bool kiemtrattvoitau(int xi, int yi)
{
	long tongdt1 = dtStamgiac(V[0].x, V[0].y, V[0].x, V[0].y, V[5].x, V[5].y);
		tongdt1 += dtStamgiac(V[0].x, V[0].y, V[5].x, V[5].y, V[6].x, V[6].y);
		tongdt1 += dtStamgiac(V[0].x, V[0].y, V[6].x, V[6].y, V[11].x, V[11].y);
		tongdt1 += dtStamgiac(V[0].x, V[0].y, V[11].x, V[11].y, V[12].x, V[12].y);
		tongdt1 += dtStamgiac(V[0].x, V[0].y, V[12].x, V[12].y, V[0].x, V[0].y);	
	long tongdt2 = dtStamgiac(xi, yi, V[0].x, V[0].y, V[5].x, V[5].y);
		tongdt2 += dtStamgiac(xi, yi, V[5].x, V[5].y, V[6].x, V[6].y);
		tongdt2 += dtStamgiac(xi, yi, V[6].x, V[6].y, V[11].x, V[11].y);
		tongdt2 += dtStamgiac(xi, yi, V[11].x, V[11].y, V[12].x, V[12].y);
		tongdt2 += dtStamgiac(xi, yi, V[12].x, V[12].y, V[0].x, V[0].y);
	
	if (tongdt1 == tongdt2)
	{
		return true;
	} 
	
	return false;
}
bool kiemtrattvoicanhtren(int xi, int yi)
{
	long tongdt1 = dtStamgiac(V[1].x, V[1].y, V[1].x, V[1].y, V[2].x, V[2].y);
		tongdt1 += dtStamgiac(V[1].x, V[1].y, V[2].x, V[2].y, V[3].x, V[3].y);
		tongdt1 += dtStamgiac(V[1].x, V[1].y, V[3].x, V[3].y, V[4].x, V[4].y);
		tongdt1 += dtStamgiac(V[1].x, V[1].y, V[4].x, V[4].y, V[1].x, V[1].y);	
	long tongdt2 = dtStamgiac(xi, yi, V[1].x, V[1].y, V[2].x, V[2].y);
		tongdt2 += dtStamgiac(xi, yi, V[2].x, V[2].y, V[3].x, V[3].y);
		tongdt2 += dtStamgiac(xi, yi, V[3].x, V[3].y, V[4].x, V[4].y);
		tongdt2 += dtStamgiac(xi, yi, V[4].x, V[4].y, V[1].x, V[1].y);
	
	if (tongdt1 == tongdt2)
	{
		return true;
	} 
	
	return false;
}
bool kiemtrattvoicanhduoi(int xi, int yi)
{
	long tongdt1 = dtStamgiac(V[7].x, V[7].y, V[7].x, V[7].y, V[8].x, V[8].y);
		tongdt1 += dtStamgiac(V[7].x, V[7].y, V[8].x, V[8].y, V[9].x, V[9].y);
		tongdt1 += dtStamgiac(V[7].x, V[7].y, V[9].x, V[9].y, V[10].x, V[10].y);
		tongdt1 += dtStamgiac(V[7].x, V[7].y, V[10].x, V[10].y, V[7].x, V[7].y);	
	long tongdt2 = dtStamgiac(xi, yi, V[7].x, V[7].y, V[8].x, V[8].y);
		tongdt2 += dtStamgiac(xi, yi, V[8].x, V[8].y, V[9].x, V[9].y);
		tongdt2 += dtStamgiac(xi, yi, V[9].x, V[9].y, V[10].x, V[10].y);
		tongdt2 += dtStamgiac(xi, yi, V[10].x, V[10].y, V[7].x, V[7].y);
	
	if (tongdt1 == tongdt2)
	{
		return true;
	} 
	
	return false;
}
// Kết thúc
void  Vevuno()
{
	srand(time(0));
    for (int i = 0; i < numPoints; ++i) {
       	float angle = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;
        float radius = static_cast<float>(rand()) / RAND_MAX * explosionRadius;
        points[i][0] = centerX + radius * cos(angle);
        points[i][1] = centerY + radius * sin(angle);
    }
	glBegin(GL_POLYGON);
	glColor3f(1.0f, 0.3f, 0.0f);
    for (int i = 0; i < numPoints; ++i) 
	{
        glVertex2f(points[i][0], points[i][1]);
    }
	glEnd();
	vuno = true;
}
// Tọa độ để vẽ tàu bay
void ToadoVatthe()
{
	V[0].x = -1240; V[0].y = 50;
	V[1].x = -1405; V[1].y = 50;
	V[2].x = -1435; V[2].y = 100;
	V[3].x = -1460; V[3].y = 100;
	V[4].x = -1435; V[4].y = 50;
	V[5].x = -1460; V[5].y = 50;
	V[6].x = -1460; V[6].y = -50;
	V[7].x = -1435; V[7].y = -50;
	V[8].x = -1470; V[8].y = -110;
	V[9].x = -1430; V[9].y = -110;
	V[10].x = -1395; V[10].y = -50;
	V[11].x = -1275; V[11].y = -50;
	V[12].x = -1240; V[12].y = -20;
}
// Kết thúc

// Vẽ tàu bay
void Vevatthe()
{
   // Ve nen cho tàu bay
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2i(V[1].x, V[1].y);
		glVertex2i(V[2].x, V[2].y);
		glVertex2i(V[3].x, V[3].y);
		glVertex2i(V[4].x, V[4].y);
	glEnd();
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2i(V[7].x, V[7].y);
		glVertex2i(V[8].x, V[8].y);
		glVertex2i(V[9].x, V[9].y);
		glVertex2i(V[10].x, V[10].y);
	glEnd();
	glBegin(GL_POLYGON);
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex2i(V[0].x, V[0].y);
		glVertex2i(V[5].x, V[5].y);
		glVertex2i(V[6].x, V[6].y);
		glVertex2i(V[11].x, V[11].y);
		glVertex2i(V[12].x, V[12].y);
	glEnd();
	glBegin(GL_POLYGON);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2i(V[0].x, V[0].y);
		glVertex2i(V[0].x - 100, V[0].y);
		glVertex2i(V[12].x - 100, V[12].y + 30);
		glVertex2i(V[12].x, V[12].y + 30);
	glEnd();
	glBegin(GL_POLYGON);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2i(V[5].x + 10, V[5].y);
		glVertex2i(V[6].x + 10, V[6].y);
		glVertex2i(V[7].x - 5, V[7].y);
		glVertex2i(V[4].x - 5, V[4].y);
	glEnd();
	// To vien
	glLineWidth(4.0f); // Đặt độ dày của viền
    glColor3f(0.0f, 0.0f, 0.0f); // Màu đen
    glBegin(GL_LINE_LOOP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2i(V[1].x, V[1].y);
		glVertex2i(V[2].x, V[2].y);
		glVertex2i(V[3].x, V[3].y);
		glVertex2i(V[4].x, V[4].y);
	glEnd();
	glBegin(GL_LINE_LOOP);
		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2i(V[7].x, V[7].y);
		glVertex2i(V[8].x, V[8].y);
		glVertex2i(V[9].x, V[9].y);
		glVertex2i(V[10].x, V[10].y);
	glEnd();
	 glBegin(GL_LINE_LOOP);
        glVertex2i(V[0].x, V[0].y);
        glVertex2i(V[5].x, V[5].y);
        glVertex2i(V[6].x, V[6].y);
        glVertex2i(V[11].x, V[11].y);
        glVertex2i(V[12].x, V[12].y);
    glEnd();
    // Phan lua chay
    if (baohieu || pause)
    {
	    glBegin(GL_POLYGON);
			glColor3f(1.0f, 0.55f, 0.2f);
			glVertex2i(V[5].x, V[5].y);
			glVertex2i(V[5].x - 25, V[5].y);
			glVertex2i(V[5].x - 26, V[5].y - 12);
			glVertex2i(V[5].x - 51, V[5].y - 12);
			glVertex2i(V[5].x - 52, V[5].y - 24);
			glVertex2i(V[5].x - 77, V[5].y - 24);
			glVertex2i(V[5].x - 78, V[5].y - 36);
			glVertex2i(V[5].x - 100, V[5].y - 42);
			glVertex2i(V[6].x - 78, V[6].y + 36);
			glVertex2i(V[6].x - 77, V[6].y + 36);
			glVertex2i(V[6].x - 52, V[6].y + 24);
			glVertex2i(V[6].x - 51, V[6].y + 24);
			glVertex2i(V[6].x - 26, V[6].y + 12);
			glVertex2i(V[6].x - 25, V[6].y + 12);
			glVertex2i(V[6].x, V[6].y);
		glEnd();
	}
}
// Kết thúc 

// Tọa độ của các cục thiên thạch
void Toadothienthach()
{
	T1[0].tocdo = 1.4; T1[0].dixuong = 0; T1[0].dilen = 0; 
	T1[0].chimucmaxx = 4; T1[0].chimucminy = 3;T1[0].chimucmaxy = 6;
	T1[0].D[0].x = 1746; T1[0].D[0].y = 46;         
	T1[0].D[1].x = 1921; T1[0].D[1].y = -152; 
	T1[0].D[2].x = 2232; T1[0].D[2].y = -242; 
	T1[0].D[3].x = 2575; T1[0].D[3].y = -288; 
	T1[0].D[4].x = 2841; T1[0].D[4].y = 55; 
	T1[0].D[5].x = 2814; T1[0].D[5].y = 379; 
	T1[0].D[6].x = 2232; T1[0].D[6].y = 438; 
	T1[0].D[7].x = 1854; T1[0].D[7].y = 321; 
	T1[1].tocdo = 1.2; T1[1].dixuong = 0.4; T1[1].dilen = 0; 
	T1[1].chimucmaxx = 5; T1[1].chimucminy = 2; T1[1].chimucmaxy = 7;
	T1[1].D[0].x = 1795; T1[1].D[0].y = 1333;
	T1[1].D[1].x = 1938; T1[1].D[1].y = 806; 
	T1[1].D[2].x = 2318; T1[1].D[2].y = 650; 
	T1[1].D[3].x = 2318; T1[1].D[3].y = 650; 
	T1[1].D[4].x = 2984; T1[1].D[4].y = 912; 
	T1[1].D[5].x = 3190; T1[1].D[5].y = 1308; 
	T1[1].D[6].x = 2708; T1[1].D[6].y = 1460; 
	T1[1].D[7].x = 2237; T1[1].D[7].y = 1566;  
	T1[2].tocdo = 1.8; T1[2].dixuong = 0; T1[2].dilen = 0;
    T1[2].chimucmaxx = 4; T1[2].chimucminy = 1; T1[2].chimucmaxy = 7;
	T1[2].D[0].x = 1638; T1[2].D[0].y = 410;
	T1[2].D[1].x = 2580; T1[2].D[1].y = 68; 
	T1[2].D[2].x = 2580; T1[2].D[2].y = 68; 
	T1[2].D[3].x = 3248; T1[2].D[3].y = 144; 
	T1[2].D[4].x = 3418; T1[2].D[4].y = 333; 
	T1[2].D[5].x = 3000; T1[2].D[5].y = 604; 
	T1[2].D[6].x = 3000; T1[2].D[6].y = 604; 
	T1[2].D[7].x = 2168; T1[2].D[7].y = 628;  
	T1[3].tocdo = -0.8; T1[3].dixuong = 0; T1[3].dilen = 0.6;
	T1[3].chimucmaxx = 4; T1[3].chimucminy = 2; T1[3].chimucmaxy = 7;
	T1[3].D[0].x = -1920; T1[3].D[0].y = -1478;
	T1[3].D[1].x = -1920; T1[3].D[1].y = -1478; 
	T1[3].D[2].x = -1322; T1[3].D[2].y = -1620; 
	T1[3].D[3].x = -738; T1[3].D[3].y = -1506; 
	T1[3].D[4].x = -490; T1[3].D[4].y = -1200; 
	T1[3].D[5].x = -742; T1[3].D[5].y = -912; 
	T1[3].D[6].x = -742; T1[3].D[6].y = -912; 
	T1[3].D[7].x = -1680; T1[3].D[7].y = -880;  
	T1[4].tocdo = 1.4; T1[4].dixuong = 0; T1[4].dilen = 0;
	T1[4].chimucmaxx = 5; T1[4].chimucminy = 2; T1[4].chimucmaxy = 7;
	T1[4].D[0].x = 1800; T1[4].D[0].y = -180;
	T1[4].D[1].x = 1770; T1[4].D[1].y = -600; 
	T1[4].D[2].x = 2084; T1[4].D[2].y = -710; 
	T1[4].D[3].x = 2084; T1[4].D[3].y = -710; 
	T1[4].D[4].x = 2188; T1[4].D[4].y = -238; 
	T1[4].D[5].x = 2218; T1[4].D[5].y = 154; 
	T1[4].D[6].x = 2218; T1[4].D[6].y = 154; 
	T1[4].D[7].x = 1998; T1[4].D[7].y = 392;  
	T1[5].tocdo = 0.3; T1[5].dixuong = 0.9; T1[5].dilen = 0;
	T1[5].chimucmaxx = 5; T1[5].chimucminy = 3; T1[5].chimucmaxy = 7;
	T1[5].D[0].x = -410; T1[5].D[0].y = 1132;
	T1[5].D[1].x = -260; T1[5].D[1].y = 916; 
	T1[5].D[2].x = 280; T1[5].D[2].y = 840; 
	T1[5].D[3].x = 630; T1[5].D[3].y = 838; 
	T1[5].D[4].x = 982; T1[5].D[4].y = 900; 
	T1[5].D[5].x = 1298; T1[5].D[5].y = 1142; 
	T1[5].D[6].x = 952; T1[5].D[6].y = 1426; 
	T1[5].D[7].x = 246; T1[5].D[7].y = 1439;  
	T1[6].tocdo = 0.7; T1[6].dixuong = 1.6; T1[6].dilen = 0;
	T1[6].chimucmaxx = 5; T1[6].chimucminy = 3; T1[6].chimucmaxy = 0;
	T1[6].D[0].x = 24; T1[6].D[0].y = 1210;
	T1[6].D[1].x = -82; T1[6].D[1].y = 992; 
	T1[6].D[2].x = 26; T1[6].D[2].y = 836; 
	T1[6].D[3].x = 300; T1[6].D[3].y = 828; 
	T1[6].D[4].x = 388; T1[6].D[4].y = 955; 
	T1[6].D[5].x = 396; T1[6].D[5].y = 1098; 
	T1[6].D[6].x = 316; T1[6].D[6].y = 1208; 
	T1[6].D[7].x = 24; T1[6].D[7].y = 1202;                                                                                                                                                                                                                                                                                                              
	T1[7].tocdo = 1.5; T1[7].dixuong = 0; T1[7].dilen = 0;
	T1[7].chimucmaxx = 4; T1[7].chimucminy = 3; T1[7].chimucmaxy = 7;
	T1[7].D[0].x = 1742; T1[7].D[0].y = 198;
	T1[7].D[1].x = 1768; T1[7].D[1].y = -316; 
	T1[7].D[2].x = 1768; T1[7].D[2].y = -316; 
	T1[7].D[3].x = 2284; T1[7].D[3].y = -430; 
	T1[7].D[4].x = 2486; T1[7].D[4].y = -310; 
	T1[7].D[5].x = 2486; T1[7].D[5].y = -310; 
	T1[7].D[6].x = 2456; T1[7].D[6].y = 204; 
	T1[7].D[7].x = 2112; T1[7].D[7].y = 448;                                                                                                                                                                                                                                                                                                              
	T1[8].tocdo = 1.3; T1[8].dixuong = 0; T1[8].dilen = 0.4;
	T1[8].chimucmaxx = 6; T1[8].chimucminy = 3; T1[8].chimucmaxy = 0;
	T1[8].D[0].x = 1862; T1[8].D[0].y = -510;
	T1[8].D[1].x = 1862; T1[8].D[1].y = -510; 
	T1[8].D[2].x = 1558; T1[8].D[2].y = -996; 
	T1[8].D[3].x = 1842; T1[8].D[3].y = -1410; 
	T1[8].D[4].x = 2388; T1[8].D[4].y = -1350; 
	T1[8].D[5].x = 2676; T1[8].D[5].y = -1118; 
	T1[8].D[6].x = 2676; T1[8].D[6].y = -1118; 
	T1[8].D[7].x = 2594; T1[8].D[7].y = -532;                                                                                                                                                                                                                                                                                                              
	T1[9].tocdo = 1.6; T1[9].dixuong = 0.6; T1[9].dilen = 0;
	T1[9].chimucmaxx = 4; T1[9].chimucminy = 2; T1[9].chimucmaxy = 6;
	T1[9].D[0].x = 1980; T1[9].D[0].y = 1310;
	T1[9].D[1].x = 1690; T1[9].D[1].y = 758; 
	T1[9].D[2].x = 2196; T1[9].D[2].y = 686; 
	T1[9].D[3].x = 2546; T1[9].D[3].y = 880; 
	T1[9].D[4].x = 2612; T1[9].D[4].y = 1262; 
	T1[9].D[5].x = 2612; T1[9].D[5].y = 1262; 
	T1[9].D[6].x = 2492; T1[9].D[6].y = 1566; 
	T1[9].D[7].x = 2492; T1[9].D[7].y = 1566;                                                                                                                                                                                                                                                                                                              
	T1[10].tocdo = 0.2; T1[10].dixuong = 0; T1[10].dilen = 1.1;
	T1[10].chimucmaxx = 4; T1[10].chimucminy = 3; T1[10].chimucmaxy = 6;
	T1[10].D[0].x = -486; T1[10].D[0].y = -904;
	T1[10].D[1].x = -846; T1[10].D[1].y = -1194; 
	T1[10].D[2].x = -288; T1[10].D[2].y = -1480; 
	T1[10].D[3].x = 218; T1[10].D[3].y = -1502; 
	T1[10].D[4].x = 722; T1[10].D[4].y = -1310; 
	T1[10].D[5].x = 396; T1[10].D[5].y = -1030; 
	T1[10].D[6].x = 6; T1[10].D[6].y = -838; 
	T1[10].D[7].x = 6; T1[10].D[7].y = -838;                                                                                                                                                                                                                                                                                                              
	T1[11].tocdo = 1.5; T1[11].dixuong = 0; T1[11].dilen = 0;
	T1[11].chimucmaxx = 4; T1[11].chimucminy = 2; T1[11].chimucmaxy = 6;
	T1[11].D[0].x = 1828; T1[11].D[0].y = 288;
	T1[11].D[1].x = 1640; T1[11].D[1].y = 5; 
	T1[11].D[2].x = 1808; T1[11].D[2].y = -230; 
	T1[11].D[3].x = 2570; T1[11].D[3].y = 80; 
	T1[11].D[4].x = 2992; T1[11].D[4].y = 636; 
	T1[11].D[5].x = 2992; T1[11].D[5].y = 636; 
	T1[11].D[6].x = 2190; T1[11].D[6].y = 484; 
	T1[11].D[7].x = 2190; T1[11].D[7].y = 484;                                                                                                                                                                                                                                                                                                              
	T1[12].tocdo = 1.4; T1[12].dixuong = 0; T1[12].dilen = 0;
	T1[12].chimucmaxx = 3; T1[12].chimucminy = 4; T1[12].chimucmaxy = 7;
	T1[12].D[0].x = 1678; T1[12].D[0].y = -64;
	T1[12].D[1].x = 2100; T1[12].D[1].y = -432; 
	T1[12].D[2].x = 2100; T1[12].D[2].y = -432; 
	T1[12].D[3].x = 2702; T1[12].D[3].y = -650; 
	T1[12].D[4].x = 2702; T1[12].D[4].y = -650; 
	T1[12].D[5].x = 2362; T1[12].D[5].y = -94; 
	T1[12].D[6].x = 2362; T1[12].D[6].y = -94; 
	T1[12].D[7].x = 1658; T1[12].D[7].y = 262;                                                                                                                                                                                                                                                                                                              
	T1[13].tocdo = 1.4; T1[13].dixuong = 0; T1[13].dilen = 0;
	T1[13].chimucmaxx = 5; T1[13].chimucminy = 2; T1[13].chimucmaxy = 6;
	T1[13].D[0].x = 1692; T1[13].D[0].y = -230;
	T1[13].D[1].x = 1802; T1[13].D[1].y = -604; 
	T1[13].D[2].x = 2252; T1[13].D[2].y = -644; 
	T1[13].D[3].x = 2648; T1[13].D[3].y = -400; 
	T1[13].D[4].x = 2648; T1[13].D[4].y = -400; 
	T1[13].D[5].x = 2924; T1[13].D[5].y = -12; 
	T1[13].D[6].x = 2390; T1[13].D[6].y = 370; 
	T1[13].D[7].x = 1850; T1[13].D[7].y = 212;                                                                                                                                                                                                                                                                                                              
	T1[14].tocdo = 1.6; T1[14].dixuong = 0; T1[14].dilen = 1.6;
	T1[14].chimucmaxx = 3; T1[14].chimucminy = 2; T1[14].chimucmaxy = 6;
	T1[14].D[0].x = 936; T1[14].D[0].y = -1012;
	T1[14].D[1].x = 1030; T1[14].D[1].y = -1190; 
	T1[14].D[2].x = 1374; T1[14].D[2].y = -1264; 
	T1[14].D[3].x = 1484; T1[14].D[3].y = -1048; 
	T1[14].D[4].x = 1484; T1[14].D[4].y = -1048; 
	T1[14].D[5].x = 1288; T1[14].D[5].y = -840; 
	T1[14].D[6].x = 1288; T1[14].D[6].y = -840; 
	T1[14].D[7].x = 1030; T1[14].D[7].y = -850;                                                                                                                                                                                                                                                                                                              
	T1[15].tocdo = -0.7; T1[15].dixuong = 0; T1[15].dilen = 1.5;
	T1[15].chimucmaxx = 5; T1[15].chimucminy = 4; T1[15].chimucmaxy = 7;
	T1[15].D[0].x = -1390; T1[15].D[0].y = -982;
	T1[15].D[1].x = -1390; T1[15].D[1].y = -982; 
	T1[15].D[2].x = -1360; T1[15].D[2].y = -1210; 
	T1[15].D[3].x = -1028; T1[15].D[3].y = -1264; 
	T1[15].D[4].x = -1028; T1[15].D[4].y = -1264; 
	T1[15].D[5].x = -928; T1[15].D[5].y = -1028; 
	T1[15].D[6].x = -928; T1[15].D[6].y = -1028; 
	T1[15].D[7].x = -1112; T1[15].D[7].y = -840;                                                                                                                                                                                                                                                                                                              
	T1[16].tocdo = 1.2; T1[16].dixuong = 0.2; T1[16].dilen = 0;
	T1[16].chimucmaxx = 4; T1[16].chimucminy = 2; T1[16].chimucmaxy = 7;
	T1[16].D[0].x = 1716; T1[16].D[0].y = 390;
	T1[16].D[1].x = 1646; T1[16].D[1].y = -120; 
	T1[16].D[2].x = 2194; T1[16].D[2].y = -490; 
	T1[16].D[3].x = 2792; T1[16].D[3].y = -462; 
	T1[16].D[4].x = 3114; T1[16].D[4].y = 196; 
	T1[16].D[5].x = 3080; T1[16].D[5].y = 700; 
	T1[16].D[6].x = 2480; T1[16].D[6].y = 980; 
	T1[16].D[7].x = 1916; T1[16].D[7].y = 990;                                                                                                                                                                                                                                                                                                              
	T1[17].tocdo = 1.1; T1[17].dixuong = 0; T1[17].dilen = 0.18;
	T1[17].chimucmaxx = 5; T1[17].chimucminy = 3; T1[17].chimucmaxy = 6;
	T1[17].D[0].x = 1806; T1[17].D[0].y = -58;
	T1[17].D[1].x = 1636; T1[17].D[1].y = -672; 
	T1[17].D[2].x = 2088; T1[17].D[2].y = -1054; 
	T1[17].D[3].x = 2604; T1[17].D[3].y = -1058; 
	T1[17].D[4].x = 3060; T1[17].D[4].y = -880; 
	T1[17].D[5].x = 3200; T1[17].D[5].y = 178; 
	T1[17].D[6].x = 2752; T1[17].D[6].y = 468; 
	T1[17].D[7].x = 2194; T1[17].D[7].y = 290;                                                                                                                                                                                                                                                                                                              
	T1[18].tocdo = 0; T1[18].dixuong = 0; T1[18].dilen = 0.62;
	T1[18].chimucmaxx = 6; T1[18].chimucminy = 3; T1[18].chimucmaxy = 7;
	T1[18].D[0].x = -1562; T1[18].D[0].y = -894;
	T1[18].D[1].x = -1562; T1[18].D[1].y = -894; 
	T1[18].D[2].x = -1348; T1[18].D[2].y = -1312; 
	T1[18].D[3].x = -980; T1[18].D[3].y = -1450; 
	T1[18].D[4].x = -486; T1[18].D[4].y = -1302; 
	T1[18].D[5].x = -314; T1[18].D[5].y = -910; 
	T1[18].D[6].x = -884; T1[18].D[6].y = -834; 
	T1[18].D[7].x = -884; T1[18].D[7].y = -834;                                                                                                                                                                                                                                                                                                              
	T1[19].tocdo = 0.4; T1[19].dixuong = 0; T1[19].dilen = 0.8;
	T1[19].chimucmaxx = 9; T1[19].chimucminy = 3; T1[19].chimucmaxy = 6;
	T1[19].D[0].x = -1134; T1[19].D[0].y = -1170;
	T1[19].D[1].x = -972; T1[19].D[1].y = -1700; 
	T1[19].D[2].x = -972; T1[19].D[2].y = -1700; 
	T1[19].D[3].x = -500; T1[19].D[3].y = -1786; 
	T1[19].D[4].x = 156; T1[19].D[4].y = -1590; 
	T1[19].D[5].x = 256; T1[19].D[5].y = -1094; 
	T1[19].D[6].x = -520; T1[19].D[6].y = -846; 
	T1[19].D[7].x = -826; T1[19].D[7].y = -892;                                                                                                                                                                                                                                                                                                              
}
// Kết thúc

// Vẽ thiên thạch
void Vethienthach()
{
	glBegin(GL_POLYGON);
		glColor3f(0.1f, 0.1f, 0.1f);
		glVertex2i(T1[sott1].D[0].x, T1[sott1].D[0].y);
		glVertex2i(T1[sott1].D[1].x, T1[sott1].D[1].y);
		glVertex2i(T1[sott1].D[2].x, T1[sott1].D[2].y);
		glVertex2i(T1[sott1].D[3].x, T1[sott1].D[3].y);
		glVertex2i(T1[sott1].D[4].x, T1[sott1].D[4].y);
		glVertex2i(T1[sott1].D[5].x, T1[sott1].D[5].y);
		glVertex2i(T1[sott1].D[6].x, T1[sott1].D[6].y);
		glVertex2i(T1[sott1].D[7].x, T1[sott1].D[7].y);
	glEnd();
	
	glLineWidth(4.0f); // Đặt độ dày của viền
	glBegin(GL_LINE_LOOP);
	 	glColor3f(0.0f, 0.0f, 0.0f);
		glVertex2i(T1[sott1].D[0].x, T1[sott1].D[0].y);
		glVertex2i(T1[sott1].D[1].x, T1[sott1].D[1].y);
		glVertex2i(T1[sott1].D[2].x, T1[sott1].D[2].y);
		glVertex2i(T1[sott1].D[3].x, T1[sott1].D[3].y);
		glVertex2i(T1[sott1].D[4].x, T1[sott1].D[4].y);
		glVertex2i(T1[sott1].D[5].x, T1[sott1].D[5].y);
		glVertex2i(T1[sott1].D[6].x, T1[sott1].D[6].y);
		glVertex2i(T1[sott1].D[7].x, T1[sott1].D[7].y);
	glEnd();
}
// Kết thúc
// In dòng chữ lên màn hình 
void dongchu(float x, float y, string chu)
{
    glRasterPos2f(x, y);
    for (int i = 0; i < chu.size(); ++i) 
	{
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, chu[i]);
    }
}
// Kết thúc

// Tăng độ khó cho game
void tangtocdo()
{
	if (diem == 10)
	{
		dd == 22;
		d == 14;
	}
}
// Kết thức

// Hàm display
void mydisplay()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 1600, 800);
// Đè trục tọa độ vẽ ảnh nền và gameplay chồng lên nhau 
	// Cho ảnh nền
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 3200, 0, 1200, -1, 1); 
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    drawBackground(); 
    // Cho Gameplay
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1600, 1600, -800, 800, -1, 1);// Khai bao ma tran bien doi cho chuong trinh
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	
// Vẽ, hiện bảng thông báo
    Vevatthe();
    Vethienthach(); 
    glColor3f(1.0f, 1.0f, 1.0f);
    dongchu(1020, 650, to_string(diem));
    if (!baohieu && !replay) 
	{
		if (pause)
		{
			glColor3f(1.0f, 1.0f, 1.0f);
       		dongchu(-500, -400, "Nhan phim 'cach' de tiep tuc choi");
		}
		else
		{
			glColor3f(1.0f, 1.0f, 1.0f);
        	dongchu(-500, -400, "Nhan phim 'Enter' de bat dau tro choi");
		}
    }
	else if (diem == 10)
	{
		glColor3f(1.0f, 1.0f, 1.0f);
   		dongchu(-300, 300, "Tang toc!");
	}
	else if (replay)
   {	
   		if (!vuno)
   		{
   			Vevuno();
   		}
   		else
   		{
			char _diem[10];
	    	itoa(diem,_diem,10);
	    	char text[50]="Your score: ";
	    	strcat(text,_diem);
	    	strcat(text,"\n\nBan co muon tiep tuc ?");
	        int result =  MessageBox(NULL,text,"GAME OVER", MB_YESNO | MB_ICONQUESTION);
		    if (result == IDYES )
			{
				replay = false;
				vuno = false;
		    	diem = 0;
				sott1 = distra(gen); 
				nhanlen = false, nhanxuong = false, nhantrai = false, nhanphai = false;
				ToadoVatthe();
				Toadothienthach();
			}
			else if (result == IDNO)
			{
				exit(0);
			} 
		}
		 
	}
    glColor3f(1.0f, 1.0f, 1.0f);
    glFlush();
}
// Kết thúc 
void changeX(float d)
{	
    for (int i = 0; i < 13; i++)
    {
    	V[i].x += d;
	}
} 
void changeY(float d)
{
    for (int i = 0; i < 13; i++)
    {
    	V[i].y += d;
	}
}

// Cách mà tàu di chuyển và phần khung hạn chế tàu di chuyển ra ngoài cửa sổ
  // Cách tàu di chuyển không chỉ là thông qua sự kiện nhấn phím nhả phím mà thông qua điều  
  //kiện để tàu bay lên xuống trái phái, nếu các biến đó là true thi di chuyển, false thì ngược lại,
  //mục đích nhằm giảm độ khựng của tàu khi di chuyển vì sự kiện nhấn phím 
void updatePosition()
{
    if (nhanlen && V[2].y <= 720) 
	{
        changeY(d);
    }
    if (nhanxuong && V[8].y >= -800) 
	{
        changeY(-d);
    }
    if (nhantrai && V[8].x >= -1500) 
	{
        changeX(-d);
    }
    if (nhanphai && V[0].x <= 1120) 
	{
        changeX(d);
    }
}
   // sự kiện nhấn phím
void keypressSpecial(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
    {
    	nhanlen = true;
    }
    if (key == GLUT_KEY_DOWN)
    {
    	nhanxuong = true;
    }
    if (key == GLUT_KEY_LEFT)
    {
    	nhantrai = true;
    }
    if (key == GLUT_KEY_RIGHT)
    {
    	nhanphai = true;
    }
} 
    // sự kiện nhả phím
void keyReleased(int key, int x, int y)
{
   	if (key == GLUT_KEY_UP)
    {
    	nhanlen = false;
    }
    if (key == GLUT_KEY_DOWN)
    {
        nhanxuong = false;
    }
    if (key == GLUT_KEY_LEFT)
    {
        nhantrai = false;
    }
    if (key == GLUT_KEY_RIGHT)
    {
        nhanphai = false;
    }
}
// Kết thúc 

// Cách thức bay của các cục thiên thạch, điều kiện để đổi sang cục thiên thạch mới và thuật toán kiểm tra va chạm
   // Bằng cách sử dụng hàm gọi thời gian, liên tục cập nhật vị trí của tàu bay và các cục thiên thạch 
void update(int value)
{
	if (baohieu)
	{
		tangtocdo();
	    updatePosition();
	    // Cho thiên thạch di chuyển tự động từ có thể từ mọi hướng, tùy theo các hướng đã thiết lập sẵn của từng cục thiên thạch
		for (int i = 0; i < 8; i++)
		{
			T1[sott1].D[i].x -= (T1[sott1].tocdo)*dd;
			T1[sott1].D[i].y -= (T1[sott1].dixuong)*dd;
			T1[sott1].D[i].y += (T1[sott1].dilen)*dd;
		}
		// Doi sang thien thach khac và random chúng một cách ngẫu nhiên
	    if ((T1[sott1].D[T1[sott1].chimucmaxx].x < -1600) ||  (T1[sott1].D[T1[sott1].chimucminy].y > 800 && T1[sott1].dixuong == 0) ||  (T1[sott1].D[T1[sott1].chimucmaxy].y < -800 && T1[sott1].dilen == 0))
	    {
	    	sott1 = distra(gen);    
	    	diem++;
	    	Toadothienthach();
        } 
		//  Kiem tra va cham
		for (int i = 0; i < 13; i++)
		{
			if (kiemtraPthuocS(V[i].x, V[i].y))
			{
				baohieu = false;
				replay = true;
				centerX = V[i].x; centerY = V[i].y;
			}
		}
		for (int i = 0; i < 8; i++)
		{
			if (kiemtrattvoitau(T1[sott1].D[i].x, T1[sott1].D[i].y) || kiemtrattvoicanhtren(T1[sott1].D[i].x, T1[sott1].D[i].y) || kiemtrattvoicanhduoi(T1[sott1].D[i].x, T1[sott1].D[i].y))
			{
				baohieu = false;
				replay = true;
				centerX = T1[sott1].D[i].x; centerY = T1[sott1].D[i].y;
			}
		}
	    // Goi ham ve lai hinh
	    glutPostRedisplay();
	}
	// Thiet lap lai ham goi lap lai sau mot khoang thoi gian
    glutTimerFunc(8, update, 0);
} 
// Sự kiện nhấn phím để bắt đầu, pause game và tiếp tục trò chơi
void myKey(unsigned char key, int x, int y)
{
	if (key == 13)
	{
		baohieu = true;
		pause = false;
	}
	if (key == 27)
	{
		baohieu = false;
		pause = true;
	}
	if (key == ' ')
	{
		baohieu = true;
		pause = false;
	}
	glFlush();
}
// Kết thúc
main(int argc, char* argv[])
{
    glutInit(&__argc, __argv);
	int mode = GLUT_SINGLE | GLUT_RGB;
	glutInitDisplayMode(mode);
    
    glutInitWindowSize(1600, 800);
    glutInitWindowPosition(0, 0);
    glutCreateWindow("SpaceshipAvoid");
    
    glutFullScreen();
    ToadoVatthe();
	Toadothienthach();
    loadTexture("anhvutru5.jpg");
    glutDisplayFunc(mydisplay);    	
    
    glutTimerFunc(8, update, 0);
    
    glutKeyboardFunc(myKey);
 	glutIdleFunc(mydisplay);
    glutSpecialFunc(keypressSpecial);  
    glutSpecialUpFunc(keyReleased);
    glutMainLoop();
}