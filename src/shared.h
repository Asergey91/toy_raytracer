// shared.h
#ifndef SHARED_H
#define SHARED_H

//crt
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <threads.h>

//shared defines
#define WIDTH 400
#define HEIGHT 348

//shared structs
typedef struct {
	float x;
	float y;
	float z;
}Vec3, Pos3;

typedef struct{
	Pos3 origin;
	Vec3 direction;
}Ray, Camera;

typedef struct{
	Pos3 verts[3];
	Vec3 normal;
}Tri;

typedef struct{
	Tri* tris; int tris_size;
	Camera camera;
	Pos3 light;
}Scene;

//shared from win_entry
extern uint64_t g_timer;
extern uint32_t* g_framebuffer_ptr;
extern void error_and_quit(char* message);

//shared from 2d
extern void fill_fps();
extern void fill_pixel(int x, int y, uint32_t color);
extern void fill_noise();

//shared from obj_parser
extern void parse_obj();

//shared from main
extern Scene g_scene;
extern void startup();
extern void update();
extern Vec3 vec3_normalize(Vec3 a);
extern Vec3 vec3_add(Vec3 a, Vec3 b);

#endif