#include "gf3d_entity.h"
#include "gfc_matrix.h"

#ifndef __EDITOR_H__
#define __EDITOR_H__
typedef struct EditorManger_S {
	char* levelFile;
	Entity* selEnt;
	//Vector3D pos;
	Vector4D selColor;
	Vector4D entColor;
	int curID;
}EditorManger;
/*	Goals
		Should be able to create, move, and delete walls
		Can select walls, which turns them purple
			This can be done etheir via mouse click (optional) or by pressing TAB
			Can Save and load work/ level [Comes later]
*/
void editorInit();
//Steps 1 up or down through list of ents in the scene
//if int is < 0 then down if >= 0 then up
void tabSelect(int upOrDown);

void editorUpdate();

void deleteSelect();

void saveLevel();

void loadLevel();
#endif