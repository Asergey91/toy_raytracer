#include "shared.h"

typedef struct{
	bool hit;
	Tri tri;
	float tri_distance;
}Trace_result;

Scene g_scene={};
Ray g_rays[WIDTH*HEIGHT];
int g_num_threads=16;

Vec3 vec3_add(Vec3 a, Vec3 b)
{
	Vec3 result={a.x+b.x, a.y+b.y, a.z+b.z};
	return result;
}

Vec3 vec3_sub(Vec3 a, Vec3 b)
{
	Vec3 result={a.x-b.x, a.y-b.y, a.z-b.z};
	return result;
}

Vec3 vec3_cross(Vec3 a, Vec3 b)
{
	Vec3 result={};
	result.x=(a.y*b.z)-(a.z*b.y);
	result.y=(a.z*b.x)-(a.x*b.z);
	result.z=(a.x*b.y)-(a.y*b.x);
	return result;
}

Vec3 vec3_mult(Vec3 a, float scalar)
{	
	return (Vec3){a.x*scalar, a.y*scalar, a.z*scalar};
}

Vec3 vec3_div(Vec3 a, float scalar)
{	
	return (Vec3){a.x/scalar, a.y/scalar, a.z/scalar};
}

float vec3_length(Vec3 a)
{
	float sum_sq=(a.x*a.x)+(a.y*a.y)+(a.z*a.z);
	float length=sqrt(sum_sq);
	return length;
}

Vec3 vec3_normalize(Vec3 a)
{
	Vec3 result={};
	float length=vec3_length(a);
	result.x=a.x/length;
	result.y=a.y/length;
	result.z=a.z/length;
	return result;
}

float vec3_dot(Vec3 a, Vec3 b)
{
	return (a.x*b.x)+(a.y*b.y)+(a.z*b.z);
}

void setup_rays()
{
	Pos3 light=g_scene.light;
	Camera camera=g_scene.camera;
	
	//setup screen
	float scr_dis=1.4;
	float scr_scl=0.0008;
	float scr_width=((float)WIDTH)*scr_scl;
	float scr_height=((float)HEIGHT)*scr_scl;
	Vec3 scr_vec=vec3_mult(g_scene.camera.direction, scr_dis);
	Pos3 scr_origin=vec3_add(g_scene.camera.origin, scr_vec);
	Vec3 world_up={0,0,1};
	Vec3 scr_right=vec3_cross(camera.direction, world_up);
	scr_right=vec3_normalize(scr_right);
	Vec3 scr_up=vec3_cross(scr_right, camera.direction);
	scr_up=vec3_normalize(scr_up);
	Pos3 scr_left_side=vec3_sub(
		scr_origin,
		vec3_mult(scr_right, scr_width/2)
	);
	
	//bottom left of screen
	Pos3 line_start=vec3_sub(
		scr_left_side,
		vec3_mult(scr_up, scr_height/2)
	);
	
	//setup rays
	for(int i=0; i<HEIGHT; i++)
	{
		for(int j=0; j<WIDTH; j++){
			Vec3 inter=vec3_add(
				line_start,
				vec3_mult(scr_right, ((float)j*scr_scl))
			);
			
			Vec3 ray_dir=vec3_sub(inter, g_scene.camera.origin);
			ray_dir=vec3_normalize(ray_dir);
			
			g_rays[i*WIDTH+j].origin=g_scene.camera.origin;
			g_rays[i*WIDTH+j].direction=ray_dir;			
		}
		line_start=vec3_add(
			line_start,
			vec3_mult(scr_up, scr_scl)
		);
	}
}

Trace_result trace_ray(Ray ray, Tri tri)
{
	Trace_result result={};
    
	//check if ray is parallel or facing away
	float dot = vec3_dot(ray.direction, tri.normal);
	if(dot>=0.0){
		return result;
	}
	
	//P = ray.origin + t * ray.direction //point on a ray
	//(P - v0) · normal = 0 //any point on a plane
	
	//((ray.origin + t * ray.direction) - v0) · normal = 0 //substituted
	//t = ((v0 - ray.origin) · normal) / (ray.direction · normal) //solve for t
	//t = ((v0 - ray.origin) · normal) / dot //simplify
	float t=vec3_dot(
		vec3_sub(tri.verts[0], ray.origin), 
		tri.normal
	)/dot;
	
	//intersection is behind
	if(t<0.0){
		return result;
	}
	
	//get intersection point
	Pos3 P = vec3_add(ray.origin, vec3_mult(ray.direction, t));
	
	// Define all 3 edges and vectors from edge starts to point P
	Vec3 edge1 = vec3_sub(tri.verts[1], tri.verts[0]);
	Vec3 edge2 = vec3_sub(tri.verts[2], tri.verts[1]);
	Vec3 edge3 = vec3_sub(tri.verts[0], tri.verts[2]);

	Vec3 vp1 = vec3_sub(P, tri.verts[0]);
	Vec3 vp2 = vec3_sub(P, tri.verts[1]);
	Vec3 vp3 = vec3_sub(P, tri.verts[2]);
	
	// Cross product of (edge, vector to P) gives a perpendicular vector.
	// Dotting that with the normal tells us if it points in the same direction as the normal.
	float c1 = vec3_dot(vec3_cross(edge1, vp1), tri.normal);
	float c2 = vec3_dot(vec3_cross(edge2, vp2), tri.normal);
	float c3 = vec3_dot(vec3_cross(edge3, vp3), tri.normal);
	
	// If all three are >= 0, P is on the inside of all edges
	if (c1 >= 0.0 && c2 >= 0.0 && c3 >= 0.0) 
	{
		result.hit = true;
		result.tri = tri;
		result.tri_distance = t;
	}
	return result;
}

void trace_and_fill(int i){
	uint32_t base_color=0x8888FFFF;
	Ray ray=g_rays[i];
	Trace_result closest_result={};
	for (int j=0; j<g_scene.tris_size; j++)
	{
		Tri tri=g_scene.tris[j];
		Trace_result current_result=trace_ray(ray, tri);
		if(current_result.hit){
			if(
				closest_result.hit && 
				closest_result.tri_distance<current_result.tri_distance
			)
			{
				//closest result is better
			}
			else
			{
				closest_result=current_result;
			}
		}
	}
	if(closest_result.hit)
	{
		uint32_t shaded_color=base_color;
		float dot=vec3_dot(
			closest_result.tri.normal, 
			vec3_normalize(g_scene.light)
		);
		dot=(dot+1)/2; //normalize 0..1; 0 being dark 1 being light
		uint8_t r,g,b,a;
		r=(base_color >> 0);r*=dot;
		g=(base_color >> 8);g*=dot;
		b=(base_color >> 16);b*=dot;
		a=(base_color >> 24);a*=dot;
		shaded_color=(r << 24) | (g << 16) | (b << 8) | (a << 0);		
		g_framebuffer_ptr[i]=shaded_color;
	}
}

int worker(void* arg)
{
    int id = *(int*)arg;
	int g_rays_per_thread=WIDTH*HEIGHT/g_num_threads;
    for (int i = 0; i < g_rays_per_thread; i++) {
        int frame_buffer_index = i * g_num_threads + id;
        trace_and_fill(frame_buffer_index);
    }
	return 0;
}

void rotate(float degrees)
{
    float rad = degrees * 0.017453292519943295f;
    float c = cosf(rad);
    float s = sinf(rad);

    for (int i = 0; i < g_scene.tris_size; i++) {
		for(int j =0; j<3; j++){
			float y = g_scene.tris[i].verts[j].y;
			float z = g_scene.tris[i].verts[j].z;
			
			g_scene.tris[i].verts[j].y =  y * c + z * s;
			g_scene.tris[i].verts[j].z = -y * s + z * c;			
		}
		float n_x = g_scene.tris[i].normal.y;
		float n_z = g_scene.tris[i].normal.z;
		
		g_scene.tris[i].normal.y =  n_x * c + n_z * s;
		g_scene.tris[i].normal.z = -n_x * s + n_z * c;
    }
}

void startup()
{
	parse_obj();
	
	//REMEMBER TO EXPORT FROM BLENDER Z UP Y FORWARD
	//setup camera+light+rays
	Vec3 light={};
	
	light.x=4.07620;
	light.y=1.00550;
	light.z=5.90390;
	
	Camera camera={};
	
	camera.origin.x=7.35889;
	camera.origin.y=-6.92579;
	camera.origin.z=4.95831;
	
	camera.direction.x=-0.65375;
	camera.direction.y=0.61528;
	camera.direction.z=-0.44049;
	
	g_scene.light=light;
	g_scene.camera=camera;
	
	setup_rays();
}

//runs every frame
void update()
{
	fill_noise();
	
	thrd_t threads[g_num_threads];
    int ids[g_num_threads];

    for (int i = 0; i < g_num_threads; i++) 
	{
        ids[i] = i;
        thrd_create(&threads[i], worker, &ids[i]);
    }
	
	//wait for every thread to finish
    for (int i = 0; i < g_num_threads; i++) 
	{
        thrd_join(threads[i], NULL);
    }
	
	rotate(5.0f);
	
	fill_fps();
}