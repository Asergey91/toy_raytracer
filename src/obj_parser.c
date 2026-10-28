#include "shared.h"

//obj stuff
typedef struct{
	int v_index[3];
	int vn_index[3];
}Obj_face;

typedef struct {
	Vec3* v; int v_s;
	Vec3* vn; int vn_s;
	Obj_face* face; int face_s;
}Obj_model;

static Obj_model g_parsed_obj ={};

#define ARR_PUSH(arr, len, item)\
{\
    (len)++;\
    (arr) = realloc((arr), sizeof(*(arr)) * (len));\
	if(!(arr))\
	{\
		error_and_quit("realloc failed");\
	}\
    (arr)[(len) - 1] = (item);\
}

static char* read_obj(char* path)
{
	FILE* fp = fopen(path, "r");
	if(!fp)
	{
		error_and_quit("cant locate obj file");
	}
	char c;
	uint64_t file_size = 0;
	while((c=getc(fp))!= EOF)
	{
		file_size ++;
	}
	rewind(fp);
	char* buffer = malloc(file_size+1);
	fread(buffer, 1, file_size, fp);
	
	buffer[file_size]='\0';
	
	fclose(fp);
	
	return buffer;
}

static int validate_obj(char *s) {

    int has_v = 0, has_vn = 0, has_f = 0;

    while (*s) {
		if (s[0] == 'v' && s[1] == ' '){
			has_v = 1;
		}
        
		if (s[0] == 'v' && s[1] == 'n'){ 
			has_vn = 1;
		}
        
        if (s[0] == 'f' && s[1] == ' ') {
            has_f = 1;
            int spaces = 0, dummy[6];
            
            //check 3 spaces(triangle)
			for (const char* p = s; *p && *p != '\n'; p++) {
                if (*p == ' ') spaces++;
			}
			
            if (spaces != 3) {
				return 0;
			} 
            
            // sscanf forces the exact v//vn pattern
            if (sscanf(s, "f %d//%d %d//%d %d//%d", &dummy[0], &dummy[1], &dummy[2], &dummy[3], &dummy[4], &dummy[5]) != 6){ 
                return 0;
			}
        }

        // Skip to next line
        while (*s && *s != '\n'){
			s++;
		}
		if (*s == '\n') {
			s++;
		}
    }

    return has_v && has_vn && has_f;
}

void parse_obj()
{
	char* obj_string=read_obj("../assets/def.obj");
	
	if(!validate_obj(obj_string))
	{
		error_and_quit("obj needs to be triangulated and without uv coordinates");
	}
	
	char* lf="\n";
	char* line=strtok(obj_string, lf);
	int o_count=0;
	
	while (line && o_count < 2) {
		//check model count
		if(line[0]=='o')
		{
			o_count++;
		}
		else if(line[0]=='v' && line[1]==' ')
		{
			//push a vertex
			float x,y,z;
			sscanf(line, "v %f %f %f", &x, &y, &z);
			Vec3 v3={x,y,z};
			ARR_PUSH(g_parsed_obj.v, g_parsed_obj.v_s, v3);
		}
		else if(line[0]=='v' && line[1]=='n')
		{
			//push a vertex normal
			float x, y, z;
			sscanf(line, "vn %f %f %f", &x, &y, &z);
			Vec3 v3 = { x,y,z };
			ARR_PUSH(g_parsed_obj.vn, g_parsed_obj.vn_s, v3);
		}
		else if(line[0]=='f' && line[1]==' ')
		{
			//push a face
			Obj_face of={};
			sscanf
			(
				line, 
				"f %d//%d %d//%d %d//%d", 
				&of.v_index[0], &of.vn_index[0], 
				&of.v_index[1], &of.vn_index[1], 
				&of.v_index[2], &of.vn_index[2]
			);
			//fix indecies to be 0 based
			for(int i=0; i<3; i++)
			{
				of.v_index[i]--;
				of.vn_index[i]--;
			}
			ARR_PUSH(g_parsed_obj.face, g_parsed_obj.face_s, of);
		}
		//go to new line
		line=strtok(NULL, lf);
	}
	
	//write to our scene format
	for(int i=0; i<g_parsed_obj.face_s; i++)
	{
		Obj_face face = g_parsed_obj.face[i];
		Vec3 normals[3];
		Tri tri={};
		for(int j=0; j<3; j++)
		{
			tri.verts[j]=g_parsed_obj.v[face.v_index[j]];
			normals[j]=g_parsed_obj.vn[face.vn_index[j]];
		}
		Vec3 tri_normal={};
		tri_normal=vec3_add(
			vec3_add(normals[0], normals[1]),
			normals[2]
		);
		tri_normal=vec3_normalize(tri_normal);
		tri.normal=tri_normal;
		ARR_PUSH(g_scene.tris, g_scene.tris_size, tri);
	}
}
