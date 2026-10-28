#include "shared.h"

static uint32_t fast_rand() 
{
	static uint32_t rand_state = 0xDEADBEEF;
	rand_state ^= rand_state << 13;
	rand_state ^= rand_state >> 17;
	rand_state ^= rand_state << 5;
	return rand_state;
}

void fill_pixel (int x, int y, uint32_t color)
{
	int test_x = (x>=0 && x<=(WIDTH-1));
	
	int test_y = (y>=0 && y<=(HEIGHT-1));
	
	if(test_x && test_y)
	{
		int index=y * WIDTH + x;
		g_framebuffer_ptr[index]=color;
	}
}

static void fill_square_outline(int x, int y, int size, uint32_t color)
{
	for (int i = 0; i < size; i++)
	//нижняя и верхняя грань квадрата
	{
		fill_pixel(x+i, y,color);//низ
		fill_pixel(x+i, y+size, color);//верх
	}
	for (int i = 0; i < size; i++)
	//правая грань квадрата и левая	
	{
		fill_pixel(x, y+i, color);//правая
		fill_pixel(x + size, y+i, color);//левая
	}
}
static void fill_screen(uint32_t color)
{	
	for (int i = 0; i < WIDTH*HEIGHT; i++)
	{
		g_framebuffer_ptr[i]=color;
	}
}

static void fill_square(int x, int y, int size, uint32_t color)
{
	for (int i = 0; i<size; i++){
		for (int j = 0; j<size; j++){
			fill_pixel(x+i, y+j, color);
		}
	}
}

static void fill_circle(int x, int y, int r, uint32_t color)
{
	for (int i = 0; i < r; i++)
	{
		for (int j = 0; j < r; j++)
		{	
			int a = (x-(x+j));
			int b = (y-(y+i));
			if ((r*r) >= (a*a + b*b))
			{
				fill_pixel(x+j, y+i, color);
				fill_pixel(x+j, y-i, color);
				fill_pixel(x-j, y-i, color);
				fill_pixel(x-j, y+i, color);
			}
		}
	}
}


void fill_noise()
{	
	for (int i = 0; i < (WIDTH*HEIGHT); i++)
	{
			g_framebuffer_ptr[i] = fast_rand();	
	}
}

static void fill_rectangle(int x, int y, int width, int height, uint32_t color)
{
	for (int i = 0; i < height; i++) 
	{
		for (int j = 0; j < width; j++) 
		{
			fill_pixel(x + j, y + i, color);
		}
	}
}

static void fill_moon(int x, int y, int r, uint32_t color){
	fill_circle(x, y, r, color);
	fill_circle(x-50, y, r, 0xFF000000);
}
static void fill_circle_outline(int x, int y, int r, uint32_t color, int thickness){
	
	fill_circle(x, y, r, color);
	fill_circle(x, y, r-thickness, 0xFF000000);
}

static void fasca(int x, int y, int width, int height, int r, uint32_t color)
{
	fill_circle(x + r, y , r, color);//нижний левый
	fill_circle(x + width - r, y, r, color);//нижний правый
	fill_circle(x + width - r, y + height - 2 * r, r, color);//верхний правый
	fill_circle(x + r, y + height - 2 * r, r, color);//верхний левый
	
	fill_rectangle(x, y, width, height-2*r, color);
	fill_rectangle(x + r, y-r, width-2*r, height, color);
}

static void fill_single_number(int number, int x, int y, uint32_t color, int size)
{
	int width=3;
	int height=5;
	uint32_t zero[]=
	{
		color     , color     , color, 
		
		color     , 0x00000000, color,
		
		color     , 0x00000000, color,
		
		color     , 0x00000000, color,
		
		color     , color     , color
	};
	uint32_t one[]=
	{
		0x00000000, color     , color,
		
		0x00000000, 0x00000000, color,
		
		0x00000000, 0x00000000, color,
		
		0x00000000, 0x00000000, color,
		
		0x00000000, 0x00000000, color
	};
	uint32_t two[]=
	{
		color     , color     , color, 
		
		0x00000000, 0x00000000, color,
		
		color     , color     , color,
		
		color     , 0x00000000, 0x00000000,
		
		color     , color     , color
	};
	uint32_t three[]=
	{
		color     , color     , color, 
		
		0x00000000, 0x00000000, color,
		
		color     , color     , color,
		
		0x00000000, 0x00000000, color,
		
		color     , color     , color
	};
	uint32_t four[]=
	{
		color     , 0x00000000, color, 
		
		color     , 0x00000000, color,
		
		color     , color     , color,
		
		0x00000000, 0x00000000, color,
		
		0x00000000, 0x00000000, color
	};
	uint32_t five[]=
	{
		color     , color     , color, 
		
		color     , 0x00000000, 0x00000000,
		
		color     , color     , color,
		
		0x00000000, 0x00000000, color,
		
		color     , color     , color
	};
	uint32_t six[]=
	{
		color     , color     , color, 
		
		color     , 0x00000000, 0x00000000,
		
		color     , color     , color,
		
		color     , 0x00000000, color,
		
		color     , color     , color
	};
	uint32_t seven[]=
	{
		color     , color     , color, 
		
		0x00000000, 0x00000000, color,
		
		0x00000000, color     , color,
		
		0x00000000, 0x00000000, color,
		
		0x00000000, 0x00000000, color
	};
	uint32_t eight[]=
	{
		color     , color     , color, 
		
		color     , 0x00000000, color,
		
		color     , color     , color,
		
		color     , 0x00000000, color,
		
		color     , color     , color
	};
	uint32_t nine[]=
	{
		color     , color     , color, 
		
		color     , 0x00000000, color,
		
		color     , color     , color,
		
		0x00000000, 0x00000000, color,
		
		0x00000000, 0x00000000, color
	};

	uint32_t* numbers []= {zero, one, two, three, four, five, six, seven, eight, nine};

	
	uint32_t* bitmap_number = numbers[number];
	
	for (int i = 0; i < height * size; i++)
	{
		for (int j = 0; j < width *size; j++)
		{
			int index = (i/size)*width+(j/size);
			fill_pixel(x+j, y-i, bitmap_number[index]);
		}
	}
}
static int my_pow(int base, int exp){
	if(exp==0) return 1;
	exp -= 1;
	int temp = base;
	for(;exp>0;exp--){
		base = temp*base;
	}
	return base;
}
static void fill_number(int number, int x, int y, uint32_t color, int size)
{
	if (number == 0)
	{
		fill_single_number(number, x, y, color, size);
		return;
	}
	
	if (number < 0)
	{
		number = abs(number);
	}
	
	int counter = 0;//количество цифр
	int temp = number;
	
	while (temp != 0)//
	{
		temp /= 10;
		counter += 1;
	}
	
	int* array = malloc(counter*sizeof(int));
	
	for (int i=0; i<counter; i++)
	{
		array[i] = (number / my_pow(10 ,(counter - (i + 1)))) % 10;//  123/10^(3-0)	
		fill_single_number(array[i], x, y, color, size);
		x += 5*size;
	}
	free(array);
}

void fill_fps()
{
	static uint64_t prev_time=0;
	static int frame=0;
	static int counter=0;
	static int current_fps = 0;
	uint64_t delta = g_timer-prev_time; //2-50
	counter += delta;
	frame++;
	prev_time=g_timer;


	if (counter >= 1000)
	{
		current_fps = frame;
		frame = 0;
		counter =0;
	}
	
	int offset = 20;
	int size = 3;
	fill_rectangle(offset-size, HEIGHT-offset-(size*5)-size, size*3*10, (size*5)+(size*2), 0x00000000);
	fill_number(current_fps, offset, HEIGHT-offset, 0xb1bab0ba, size);
}
