#include "stdafx.h"
#include "PartLib.h"
#include "AngleLibrary.h"


///////////////////////////////////////////////////////////////////////////////////////////////
//系统默认角钢规格库表格
//肢宽b;肢厚d;内圆半径r;截面面积;理论重量;平行轴惯性矩;平行轴回转半径;平行轴抗弯截面模量;最小轴惯性矩;最小轴回转半径
static const int MAX_ANGL_SIZE_COUNT =250;
int CURRENT_ANGL_SIZE_COUNT = 135;
JG_PARA_TYPE _xarrPresetAngleTbl[MAX_ANGL_SIZE_COUNT] =
{
	40,	3,	  5,	  2.359,   1.852, 3.59   ,1.23, 1.23  , 1.49   , 0.79 ,0.96  ,'L',
	40,	4,	  5,	  3.086,   2.422, 4.6    ,1.22, 1.6   , 1.91   , 0.79 ,1.19  ,'L',
	40,	5,	  5,	  3.791,   2.976, 5.53   ,1.21, 1.96  , 2.3    , 0.78 ,1.39  ,'L',
	45,	3,	  5,	  2.659,   2.088, 5.17   ,1.4 , 1.58  , 2.14   , 0.89 ,1.24  ,'L',
	45,	4,	  5,	  3.486,   2.736, 6.65   ,1.38, 2.05  , 2.75   , 0.89 ,1.54  ,'L',
	45,	5,	  5,	  4.292,   3.369, 8.04   ,1.37, 2.51  , 3.33   , 0.88 ,1.81  ,'L',
	45,	6,	  5,	  5.076,   3.985, 9.33   ,1.36, 2.95  , 3.89   , 0.88 ,2.06  ,'L',
	50,	3,	  5.5,    2.971,   2.332, 7.18   ,1.55, 1.96  , 2.98   , 1    ,1.57  ,'L',
	50,	4,	  5.5,    3.897,   3.059, 9.26   ,1.54, 2.56  , 3.82   , 0.99 ,1.96  ,'L',
	50,	5,	  5.5,    4.803,   3.77 , 11.21  ,1.53, 3.13  , 4.64   , 0.98 ,2.31  ,'L',
	50,	6,	  5.5,    5.688,   4.465, 13.05  ,1.52, 3.68  , 5.42   , 0.98 ,2.63  ,'L',
	56,	3,	  6  ,	  3.343,   2.624, 10.19  ,1.75, 2.48  , 4.24   , 1.13 ,2.02  ,'L',
	56,	4,	  6  ,	  4.39 ,   3.446, 13.18  ,1.73, 3.24  , 5.46   , 1.11 ,2.52  ,'L',
	56,	5,	  6  ,	  5.415,   4.251, 16.02  ,1.72, 3.97  , 6.61   , 1.1  ,2.98  ,'L',
	56,	8,	  6  ,	  8.367,   6.568, 23.63  ,1.68, 6.03  , 9.89   , 1.09 ,4.16  ,'L',
	63,	4,	  7  ,	  4.978,   3.907, 19.03  ,1.96, 4.13  , 7.89   , 1.26 ,3.29  ,'L',
	63,	5,	  7  ,	  6.143,   4.822, 23.17  ,1.94, 5.08  , 9.57   , 1.25 ,3.9   ,'L',
	63,	6,	  7  ,	  7.288,   5.721, 27.12  ,1.93, 6     , 11.2   , 1.24 ,4.46  ,'L',
	63,	8,	  7  ,	  9.515,   7.469, 34.46  ,1.9 , 7.75  , 14.33  , 1.23 ,5.47  ,'L',
	63,	10,	  7  ,	 11.657,   9.151, 41.09  ,1.88, 9.39  , 17.33  , 1.22 ,6.36  ,'L',
	70,	4,	  8  ,	  5.57 ,   4.372, 26.39  ,2.18, 5.14  , 10.99  , 1.4  ,4.17  ,'L',
	70,	5,	  8  ,	  6.875,   5.397, 32.21  ,2.16, 6.32  , 13.34  , 1.39 ,4.95  ,'L',
	70,	6,	  8  ,	  8.16 ,   6.406, 37.77  ,2.15, 7.48  , 15.61  , 1.38 ,5.67  ,'L',
	70,	7,	  8  ,	  9.424,   7.398, 43.09  ,2.14, 8.59  , 17.82  , 1.38 ,6.34  ,'L',
	70,	8,	  8  ,	 10.667,   8.373, 48.17  ,2.12, 9.68  , 19.98  , 1.37 ,6.98  ,'L',
	75,	5,	  9  ,	  7.367,   5.818, 39.97  ,2.33, 7.32  , 16.63  , 1.5  ,5.77  ,'L',
	75,	6,	  9  ,	  8.797,   6.905, 46.95  ,2.31, 8.64  , 19.51  , 1.49 ,6.67  ,'L',
	75,	7,	  9  ,	 10.16 ,   7.976, 53.57  ,2.3 , 9.93  , 22.18  , 1.48 ,7.44  ,'L',
	75,	8,	  9  ,	 11.503,   9.03 , 59.96  ,2.28, 11.2  , 24.86  , 1.47 ,8.19  ,'L',
	75,	10,	  9  ,	 14.126,  11.089, 71.98  ,2.26, 13.64 , 30.05  , 1.46 ,9.56  ,'L',
	80,	5,	  9  ,	  7.912,   6.211, 48.79  ,2.48, 8.34  , 20.25  , 1.60 ,6.66  ,'L',
	80,	6,	  9  ,	  9.397,   7.376, 57.35  ,2.47, 9.87  , 23.72  , 1.59 ,7.65  ,'L',
	80,	7,	  9  ,	 10.86 ,   8.525, 65.58  ,2.46, 11.37 , 27.09  , 1.58 ,8.58  ,'L',
	80,	8,	  9  ,   12.303,   9.658, 73.49  ,2.44, 12.83 , 30.39  , 1.57 ,9.46  ,'L',
	80,	10,	  9  ,	 15.126,  11.874, 88.43  ,2.42, 15.64 , 36.77  , 1.56 ,11.08 ,'L',
	90,	6,	  10,	 10.637,   8.35 , 82.77  ,2.79, 12.61 , 34.28  , 1.8  ,9.95  ,'L',
	90,	7,	  10,	 12.301,   9.656, 94.83  ,2.78, 14.54 , 39.18  , 1.78 ,11.19 ,'L',
	90,	8,	  10,	 13.944,  10.946, 106.47 ,2.76, 16.42 , 43.97  , 1.78 ,12.35 ,'L',
	90,	10,	  10,	 17.167,  13.476, 128.58 ,2.74, 20.07 , 53.26  , 1.76 ,14.52 ,'L',
	90,	12,	  10,	 20.306,  15.94 , 149.22 ,2.71, 23.57 , 62.22  , 1.75 ,16.49 ,'L',
	100,6,	  12,	 11.932,   9.366, 114.95 ,3.1 , 15.68 , 47.92  , 2    ,12.69 ,'L',
	100,7,	  12,	 13.796,  10.83 , 131.86 ,3.09, 18.1  , 54.74  , 1.99 ,14.26 ,'L',
	100,8,	  12,	 15.638,  12.276, 148.24 ,3.08, 20.47 , 61.41  , 1.98 ,15.75 ,'L',
	100,10,	  12,	 19.261,  15.12 , 179.51 ,3.05, 25.06 , 74.35  , 1.96 ,18.54 ,'L',
	100,12,	  12,	 22.8  ,  17.898, 208.9  ,3.03, 29.48 , 86.84  , 1.95 ,21.08 ,'L',
	100,14,	  12,	 26.256,  20.611, 236.53 ,3   , 33.73 , 99     , 1.94 ,23.44 ,'L',
	100,16,	  12,	 29.627,  23.257, 262.53 ,2.98, 37.82 , 110.89 , 1.94 ,25.63 ,'L',
	110,7,	  12,	 15.196,  11.928, 177.16 ,3.41, 22.05 , 73.38  , 2.2  ,17.51 ,'L',
	110,8,	  12,	 17.238,  13.532, 199.46 ,3.4 , 24.95 , 82.42  , 2.19 ,19.39 ,'L',
	110,10,	  12,	 21.261,  16.69 , 242.19 ,3.38, 30.6  , 99.98  , 2.17 ,22.91 ,'L',
	110,12,	  12,	 25.2  ,  19.782, 282.55 ,3.35, 36.05 , 116.93 , 2.15 ,26.15 ,'L',
	110,14,	  12,	 29.056,  22.809, 320.71 ,3.32, 41.31 , 133.4  , 2.14 ,29.14 ,'L',
	125,8,	  14,	 19.75 ,  15.504, 297.03 ,3.88, 32.52 , 123.16 , 2.5  ,25.86 ,'L',
	125,10,	  14,	 24.373,  19.133, 361.67 ,3.85, 39.97 , 149.46 , 2.48 ,30.62 ,'L',
	125,12,	  14,	 28.912,  22.696, 423.16 ,3.83, 41.17 , 174.88 , 2.46 ,35.03 ,'L',
	125,14,	  14,	 33.367,  26.193, 481.65 ,3.8 , 54.16 , 199.57 , 2.45 ,39.13 ,'L',
	140,10,	  14,	 27.373,  21.488, 514.65 ,4.34, 50.58 , 212.04 , 2.78 ,39.2  ,'L',
	140,12,	  14,	 32.512,  25.522, 603.68 ,4.31, 59.8  , 248.57 , 2.76 ,45.02 ,'L',
	140,14,	  14,	 37.567,  29.49 , 688.81 ,4.28, 68.75 , 284.06 , 2.75 ,50.45 ,'L',
	140,16,	  14,	 42.539,  33.393, 770.24 ,4.26, 77.46 , 318.67 , 2.74 ,55.55 ,'L',
	160,10,	  16,	 31.502,  24.729, 779.53 ,4.98, 66.7  , 321.76 , 3.2  ,52.76 ,'L',
	160,12,	  16,	 37.441,  29.391, 916.58 ,4.95, 78.98 , 377.49 , 3.18 ,60.74 ,'L',
	160,14,	  16,	 43.296,  33.987, 1048.36,4.92, 90.95 , 431.7  , 3.16 ,68.24 ,'L',
	160,16,	  16,	 49.067,  38.518, 1175.08,4.89, 102.63, 484.59 , 3.14 ,75.31 ,'L',
	180,12,	  16,	 42.242,  33.159, 1321.35,5.59, 100.82, 542.61 , 3.58 ,78.41 ,'L',
	180,14,	  16,	 48.896,  38.383, 1514.48,5.56, 116.25, 621.53 , 3.56 ,88.38 ,'L',
	180,16,	  16,	 55.467,  43.542, 1700.99,5.54, 131.13, 698.6  , 3.55 ,97.83 ,'L',
	180,18,	  16,	 61.955,  48.634, 1875.12,5.5 , 145.64, 762.01 , 3.51 ,105.14,'L',
	200,14,	  18,	 54.642,  42.894, 2103.55,6.2 , 144.7 , 863.83 , 3.98 ,111.82,'L',
	200,16,	  18,	 62.013,  48.68 , 2366.15,6.18, 163.65, 971.41 , 3.96 ,123.96,'L', 
	200,18,	  18,	 69.301,  54.401, 2620.64,6.15, 182.22, 1076.74, 3.94 ,135.52,'L',
	200,20,	  18,	 76.505,  60.056, 2867.3 ,6.12, 200.42, 1180.04, 3.93 ,146.55,'L',
	200,24,	  18,	 90.661,  71.168, 3338.25,6.07, 236.17, 1381.53, 3.9  ,166.65,'L',
	//新增肢宽220角钢6种，肢宽250角钢8种
	220,16,   21,	 68.664,  53.901, 3187.36,6.81, 199.55, 1310.99, 4.37, 153.81,'L',
	220,18,   21,	 76.752,  60.250, 3534.30,6.79, 222.37, 1453.27, 4.35, 168.29,'L', 
	220,20,   21,	 84.756,  66.533, 3871.49,6.76, 244.77, 1592.90, 4.34, 182.16,'L',
	220,22,   21,	 92.676,  72.751, 4199.23,6.73, 266.78, 1730.10, 4.32, 195.45,'L',
	220,24,   21,	100.512,  78.902, 4517.83,6.70, 288.39, 1865.11, 4.31, 208.21,'L',
	220,26,   21,	108.264,  84.987, 4827.58,6.68, 309.62, 1998.17, 4.30, 220.49,'L',
	250,18,   24,    87.842,  68.956, 5268.22,7.74, 290.12, 2167.41, 4.97, 224.03,'L',
	250,20,   24,    97.045,  76.180, 5779.34,7.72, 319.66, 2376.74, 4.95, 242.85,'L',
	250,22,   24,   106.166,  83.340, 5779.34,7.69, 319.66, 2376.74, 4.94, 242.85,'L',
	250,24,   24,   115.201,  90.433, 6763.93,7.66, 377.34, 2785.19, 4.92, 278.38,'L',
	250,26,   24,   124.154,  97.461, 7238.08,7.63, 406.50, 2984.84, 4.90, 295.19,'L',
	250,28,   24,   133.022, 104.422, 7700.60,7.61, 433.22, 3181.81, 4.89, 311.42,'L',
	250,30,   24,   141.807, 111.318, 8151.80,7.58, 460.51, 3376.34, 4.88, 327.12,'L',
	250,32,   24,   150.508, 118.149, 8592.01,7.56, 487.39, 3568.71, 4.87, 342.33,'L',
	250,35,   24,   163.402, 128.271, 9232.44,7.52, 526.97, 3853.72, 4.86, 364.30,'L',
	//新增肢宽280角钢8种，肢宽300角钢8种 肢宽320角钢7种 肢宽360角钢6种
    280,20,   26,   109.260, 85.769,  8229.11,  8.68, 404.57,3382.15, 5.56,312.22,'L',
	280,22,	  26,   119.580, 93.870,  8949.56,  8.65, 441.71,3677.13, 5.55,335.98,'L',
	280,24,	  26,   129.816, 101.906, 9654.05,  8.62, 478.32,3967.64, 5.53,358.92,'L',
	280,26,	  26,   139.968, 109.875, 10342.96, 8.60, 514.40,4254.00, 5.51,381.09,'L',
	280,28,	  26,   150.037, 117.779, 11016.67, 8.57, 549.97,4536.52, 5.50,402.55,'L',	
	280,30,	  26,   160.022, 125.617, 11675.57, 8.54, 585.05,4815.49, 5.49,423.33,'L',	
	280,32,	  26,   169.922, 133.389, 12320.02, 8.51, 619.64,5091.22, 5.47,443.49,'L',	
	280,35,	  26,   184.617, 144.924, 13260.42, 8.48, 670.65,5499.35, 5.46,472.64,'L',	
	300,20,   28,   117.492, 92.231 , 10199.99, 9.32, 466.79,4195.35, 5.98,364.06,'L',	
	300,22,   28,   128.612, 100.960, 11100.12, 9.29, 509.86,4562.18, 5.96,392.02,'L',	
	300,24,   28,   139.648, 109.624, 11981.60, 9.26, 552.35,4923.63, 5.94,419.06,'L',	
	300,26,   28,   150.600, 118.221, 12844.84, 9.24, 594.27,5280.02, 5.92,445.24,'L',	
	300,28,   28,   161.469, 126.753, 13690.26, 9.21, 635.63,5631.70, 5.91,470.60,'L',	
	300,30,   28,   172.253, 135.219, 14518.26, 9.18, 676.45,5978.99, 5.89,495.20,'L',	
	300,32,   28,   182.954, 143.619, 15329.25, 9.15, 716.73,6322.22, 5.88,519.08,'L',
	300,35,   28,   198.848, 156.096, 16514.72, 9.11, 776.19,6830.12, 5.86,553.64,'L',
	320,22,   28,   137.412, 107.868, 13563.78, 9.94, 582.84,5565.91, 6.36,450.92,'L',
	320,24,   28,   149.248, 117.160, 14650.10, 9.91, 631.67,6009.84, 6.35,482.51,'L',
	320,26,   28,   161.000, 126.385, 15715.30, 9.88, 679.87,6447.73, 6.33,513.14,'L',	
	320,28,   28,   172.669, 135.545, 16759.84, 9.85, 727.48,6879.93, 6.31,542.85,'L',	
	320,30,   28,   184.253, 144.639, 17784.15, 9.82, 774.49,7306.81, 6.30,571.70,'L',	
	320,32,   28,   195.754, 153.667, 18788.67, 9.80, 820.91,7728.71, 6.28,599.73,'L',	
	320,35,   28,   212.848, 167.086, 20259.29, 9.76, 889.50,8352.97, 6.26,640.34,'L',	
	360,24,   30,   168.697, 132.427, 21130.77, 11.19,806.42,8660.20, 7.16,625.08,'L',	
	360,26,   30,   182.049, 142.908, 22689.66, 11.16,868.52,9296.36, 7.15,665.63,'L',	
	360,28,   30,   195.318, 153.324, 24221.57, 11.14,929.93,9924.66, 7.13,705.07,'L',	
	360,30,   30,   208.502, 163.674, 25727.01, 11.11,990.65,10545.52,7.11,743.44,'L',	
	360,32,   30,   221.603, 173.958, 27206.47, 11.08,1050.69,11159.33,7.10,780.79,'L',	
    360,35,   30,   241.097, 189.261, 29378.04, 11.04,1139.52,12067.69,7.07, 835.04,'L',	

	//
	70,5,      8,	 13.750,  10.794,	64.15, 2.160,    0,   64.15, 2.160,     0,'D',
	75,6,      9,	 17.594,  13.810,	93.88, 2.310,    0,   93.88, 2.310,     0,'D',
	80,6,      9,    18.794,  14.752,  114.66, 2.470,    0,  114.66, 2.470,     0,'D',
	90,7,     10,	 24.602,  19.312,  190.13, 2.780,    0,  190.13, 2.780,     0,'D',
	90,8,     10,	 28.888,  21.892,  220.06, 2.760,    0,  220.06, 2.760,     0,'D',
	100,8,    12,	 31.276,  24.552,  296.70, 3.080,    0,  296.70, 3.080,     0,'D',
	110,8,    12,	 34.476,  27.064,  398.54, 3.400,    0,  398.54, 3.400,     0,'D',
	125,10,   14,	 48.746,  38.266,  722.54, 3.850,    0,  722.54, 3.850,     0,'D',
	140,14,   14,	 75.140,  58.980, 2191.08, 5.400,    0, 1376.44, 4.280,     0,'D',
	160,14,   16,	 86.600,  67.974, 3328.90, 6.200,    0, 2096.27, 4.920,     0,'D',
	160,16,   16,	 98.140,  77.036, 3736.08, 6.170,    0, 2346.73, 4.890,     0,'D',
	200,16,   18,	124.020,  97.360, 7526.04, 7.790,    0, 4736.62, 6.180,     0,'D',
	200,20,   18,	153.000, 120.112, 9118.56, 7.720,    0, 5730.52, 6.120,     0,'D',
	200,24,   18,	181.320, 142.336,10583.58, 7.640,    0, 6680.72, 6.070,     0,'D',
	160,16,   18,	196.280, 154.072,86724.43,21.020,    0,86724.43,21.020,     0,'X',
	200,16,   18,	248.040, 194.720,103325.6,20.410,    0,103325.6,20.410,     0,'X',
	200,20,   18,	306.000, 240.224,125479.1,20.250,    0,125479.1,20.250,     0,'X',
	200,24,   18,	362.640, 284.672,146466.5,20.097,    0,146466.5,20.097,     0,'X',  	
};
//角钢库全局变量
ARRAY_LIST<JG_PARA_TYPE> xarrAngleSizeTbl;
CAngleLibrary globalAngleLibrary;
SYSLIB_ANGLESIZE_N jgguige_N;
SYSLIB_ANGLESIZE_TBL jgguige_table;
///////////////////////////////////////////////////////////////////////////////////////////////
SYSLIB_ANGLESIZE_N::operator int()
{
	return xarrAngleSizeTbl.Count;
}
int SYSLIB_ANGLESIZE_N::operator =(int N)
{
	xarrAngleSizeTbl.SetSize(N);
	return N;
}
SYSLIB_ANGLESIZE_N::SYSLIB_ANGLESIZE_N(int N)
{
	xarrAngleSizeTbl.SetSize(N);
}
SYSLIB_ANGLESIZE_TBL::operator JG_PARA_TYPE*()
{
	return xarrAngleSizeTbl.m_pData;
}
IAngleLibrary* AngleLibrary(){
	return &globalAngleLibrary;
}

CAngleLibrary::CAngleLibrary()
{
	if (xarrAngleSizeTbl.Count==0)
		xarrAngleSizeTbl.Attach(_xarrPresetAngleTbl,CURRENT_ANGL_SIZE_COUNT,MAX_ANGL_SIZE_COUNT);
}

CAngleLibrary::~CAngleLibrary()
{
}
JG_PARA_TYPE* CAngleLibrary::GetAngleSizeTbl() { 
	return xarrAngleSizeTbl.m_pData; 
}
int CAngleLibrary::AngleCount() {
	return xarrAngleSizeTbl.Count;
}
int CAngleLibrary::SetAngleCount(int count)
{
	xarrAngleSizeTbl.SetSize(count);
	return count;
}
JG_PARA_TYPE* CAngleLibrary::GetAt(int index)
{
	return xarrAngleSizeTbl.GetAt(index);
}
JG_PARA_TYPE* CAngleLibrary::SetAt(int index,const JG_PARA_TYPE& anglesize)
{
	if (index<xarrAngleSizeTbl.Count)
		xarrAngleSizeTbl[index]=anglesize;
	return xarrAngleSizeTbl.GetAt(index);
}
JG_PARA_TYPE* CAngleLibrary::AppendAngleSize(const JG_PARA_TYPE& anglesize)
{
	int count=xarrAngleSizeTbl.Count+1;
	xarrAngleSizeTbl.SetSize(count);
	xarrAngleSizeTbl[count-1]=anglesize;
	return xarrAngleSizeTbl.GetAt(count-1);
}
