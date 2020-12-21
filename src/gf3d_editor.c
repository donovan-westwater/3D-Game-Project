#include "gf3d_editor.h"
#include "gf3d_player.h"
#include "gf3d_vgraphics.h"
#include "simple_json.h"

EditorManger static editor = { 0 };
static int spaceDown = false;
static int backDown = false;
static int tabDown = false;
static int saveLoadDown = false;
void editorInit() {
	Entity* list = getEntList();
	editor.curID = 0;
	editor.selEnt = NULL;
	editor.levelFile = NULL;
	editor.selColor = vector4d(1, 0, 1, 1);
	editor.entColor = vector4d(0, 0, 0, 0);
	//editor.pos = vector3d(list[0].rSelf->position.x, list[0].rSelf->position.y, list[0].rSelf->position.z);
}

void tabSelect(int upOrDown) {
	Entity* list = getEntList();
	UniformBufferObject* u = gf3d_get_pointer_to_UBO();
	
	if (editor.curID == -1) {
		editor.selEnt = NULL;
		printf("There are no entities to select");
		return;
	}
		//editor.curID = 0;
	else if(editor.selEnt != NULL) editor.selEnt->rSelf->color = editor.entColor;
	if (upOrDown >= 0) {
		editor.curID++;
		while (list[editor.curID].inuse == 0) {
			if (editor.curID >= entSize) {
				editor.curID = 0;
				break;
			}
			editor.curID++;
		}
	}
	else {
		editor.curID--;
		while (list[editor.curID].inuse == 0) {
			if (editor.curID < 0) {
				editor.curID = u->totalObj;
				break;
			}
			editor.curID--;
		}
	}

	editor.entColor = list[editor.curID].rSelf->color;
	list[editor.curID].rSelf->color = editor.selColor;
	editor.selEnt = &list[editor.curID];
}

void editorUpdate() {
	Entity* list = getEntList();
	const Uint8* keys;
	SDL_PumpEvents();   // update SDL's internal event structures
	keys = SDL_GetKeyboardState(NULL); // get the keyboard state for this frame
	if(editor.selEnt != NULL){
		if (keys[SDL_SCANCODE_UP]) editor.selEnt->rSelf->position.x += 0.1;
		if (keys[SDL_SCANCODE_DOWN]) editor.selEnt->rSelf->position.x -= 0.1;
		if (keys[SDL_SCANCODE_RIGHT]) editor.selEnt->rSelf->position.z += 0.1;
		if (keys[SDL_SCANCODE_LEFT]) editor.selEnt->rSelf->position.z -= 0.1;
		if (keys[SDL_SCANCODE_PAGEDOWN]) editor.selEnt->rSelf->position.y -= 0.1;
		if (keys[SDL_SCANCODE_PAGEUP]) editor.selEnt->rSelf->position.y += 0.1;

		if (keys[SDL_SCANCODE_W]) editor.selEnt->rSelf->rotation.z += 0.5;
		if (keys[SDL_SCANCODE_S]) editor.selEnt->rSelf->rotation.z -= 0.5;
		if (keys[SDL_SCANCODE_D]) editor.selEnt->rSelf->rotation.x += 0.5;
		if (keys[SDL_SCANCODE_A]) editor.selEnt->rSelf->rotation.x -= 0.5;
		if (keys[SDL_SCANCODE_Q]) editor.selEnt->rSelf->rotation.y += 0.5;
		if (keys[SDL_SCANCODE_E]) editor.selEnt->rSelf->rotation.y -= 0.5;

		if (keys[SDL_SCANCODE_I]) editor.selEnt->rSelf->scale.x += 0.1;
		if (keys[SDL_SCANCODE_K]) editor.selEnt->rSelf->scale.x -= 0.1;
		if (keys[SDL_SCANCODE_L]) editor.selEnt->rSelf->scale.z += 0.1;
		if (keys[SDL_SCANCODE_J]) editor.selEnt->rSelf->scale.z -= 0.1;
		if (keys[SDL_SCANCODE_U]) editor.selEnt->rSelf->scale.y += 0.1;
		if (keys[SDL_SCANCODE_O]) editor.selEnt->rSelf->scale.y -= 0.1;
	}

	if (keys[SDL_SCANCODE_SPACE] && !spaceDown) {
		spaceDown = true;
		Entity* n;
		if (editor.selEnt == NULL) {
			n = addEntity(vector4d(0, 2, 0, 1), vector4d(0, 0, 0, 1), vector4d(1, 1, 1, 1), vector4d(0, 0.5, 0.5, 1), vector3d(0, 0, 0), 1, 0,1);
		}
		else {
			n = addEntity(editor.selEnt->rSelf->position, editor.selEnt->rSelf->rotation, editor.selEnt->rSelf->scale, editor.selEnt->rSelf->color, vector3d(0, 0, 0), 1, 0,1);
			editor.selEnt->rSelf->color = editor.entColor;
		}
		editor.curID = n->rSelf->id;
		editor.entColor = n->rSelf->color;
		editor.selEnt = n;
		editor.selEnt->rSelf->color = editor.selColor;
	}
	else if (!keys[SDL_SCANCODE_SPACE] && spaceDown) {
		spaceDown = false;
	}

	if (keys[SDL_SCANCODE_BACKSPACE] && !backDown && editor.selEnt != NULL) {
		backDown = true;
		deleteEntity(editor.selEnt);
		tabSelect(-1);
	}
	else if (!keys[SDL_SCANCODE_BACKSPACE] && backDown) {
		backDown = false;
	}

	if (keys[SDL_SCANCODE_TAB] && !tabDown) {
		tabDown = true;
		tabSelect(1);
	}
	else if (keys[SDL_SCANCODE_LEFTBRACKET] && !tabDown) {

		tabDown = true;
		editor.selEnt->eType--;
		editor.selEnt->rSelf->type--;
		if (editor.selEnt->rSelf->type == 3 || editor.selEnt->rSelf->type == 2) editor.selEnt->rSelf->type = 1;
		if (editor.selEnt->eType < 1) editor.selEnt->eType = 1;
		if (editor.selEnt->rSelf->type < 1) editor.selEnt->rSelf->type = 1;
	}
	else if (keys[SDL_SCANCODE_RIGHTBRACKET] && !tabDown) {

		editor.selEnt->eType++;
		editor.selEnt->rSelf->type++;
		if (editor.selEnt->rSelf->type > 8) editor.selEnt->rSelf->type = 8;
		if (editor.selEnt->rSelf->type == 3 || editor.selEnt->rSelf->type == 2) editor.selEnt->rSelf->type = 4;
		if (editor.selEnt->eType > Herd) editor.selEnt->eType = Herd;
		tabDown = true;
	}
	else if (!keys[SDL_SCANCODE_TAB] && !keys[SDL_SCANCODE_LEFTBRACKET] && !keys[SDL_SCANCODE_RIGHTBRACKET] && tabDown) tabDown = false;

	if (keys[SDL_SCANCODE_COMMA] && !saveLoadDown) {
		clearEntList();
		playerManInit();
		loadLevel();
		saveLoadDown = true;
	}
	else if (keys[SDL_SCANCODE_PERIOD] && !saveLoadDown)
	{
		editor.selEnt->rSelf->color = editor.entColor;
		saveLevel();
		editor.selEnt->rSelf->color = editor.selColor;
		saveLoadDown = true;
	}
	else if (!keys[SDL_SCANCODE_MINUS]&& !keys[SDLK_PLUS] && saveLoadDown) saveLoadDown = false;

	

}

void saveTest() {
	SJson* m = sj_object_new();
	SJson* n = sj_object_new();

	//SJson* a = sj_array_new();
	SJson* vector = sj_array_new();
	SJson* j = sj_new_float(0.05);
	sj_array_append(vector, j);
	j = sj_new_float(0.04);
	sj_array_append(vector, j);
	j = sj_new_float(0.03);
	sj_array_append(vector, j);

	sj_object_insert(n, "Pos", vector);

	sj_object_insert(m, "Ent_Id", sj_new_int(0));
	sj_object_insert(m, "Entity", n);

	sj_save(m, "levels/test.txt");
}


void saveLevel() {

	SJson *m = sj_object_new();
	SJson* eJList = sj_array_new();
	Entity* entList = getEntList();
	SJson* e;
	for (int i = 0; i < entSize; i++) {
		if (entList[i].inuse == 0 || entList[i].eType == Collectible) {
			continue;
		}
		e = sj_object_new();
		sj_object_insert(e, "Id", sj_new_int(entList[i].id));
		
		SJson *v4 = sj_array_new();
		sj_array_append(v4, sj_new_float(entList[i].rSelf->position.x));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->position.y));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->position.z));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->position.w));
		sj_object_insert(e, "position", v4);

		v4 = sj_array_new();
		sj_array_append(v4, sj_new_float(entList[i].rSelf->rotation.x));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->rotation.y));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->rotation.z));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->rotation.w));
		sj_object_insert(e, "rotation", v4);

		v4 = sj_array_new();
		sj_array_append(v4, sj_new_float(entList[i].rSelf->scale.x));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->scale.y));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->scale.z));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->scale.w));
		sj_object_insert(e, "scale", v4);

		v4 = sj_array_new();
		sj_array_append(v4, sj_new_float(entList[i].rSelf->color.x));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->color.y));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->color.z));
		sj_array_append(v4, sj_new_float(entList[i].rSelf->color.w));
		sj_object_insert(e, "color", v4);

		sj_object_insert(e, "type", sj_new_int(entList[i].rSelf->type));
		sj_object_insert(e, "eType", sj_new_int(entList[i].eType));
		sj_array_append(eJList, e);

	}
	sj_object_insert(m, "Entity_List", eJList);
	
	sj_save(m, "levels/demo.txt");


}

void loadLevel() {
	SJson *load = sj_load("levels/demo.txt");
	SJson *list = sj_object_get_value(load, "Entity_List");
	SJson* e;
	SJson* val;
	Entity *newEnt;
	int total = sj_array_get_count(list);
	for (int i = 0; i < total; i++) {
		e = sj_array_get_nth(list, i);
		//Id
		int id;
		val = sj_object_get_value(e, "Id");
		sj_get_integer_value(val, &id);

		//Position
		Vector4D pos = vector4d(0, 0, 0, 0);
		val = sj_object_get_value(e, "position");
		sj_get_float_value(sj_array_get_nth(val, 0), &pos.x);
		sj_get_float_value(sj_array_get_nth(val, 1), &pos.y);
		sj_get_float_value(sj_array_get_nth(val, 2), &pos.z);
		sj_get_float_value(sj_array_get_nth(val, 3), &pos.w);

		//Rotation
		Vector4D rot = vector4d(0, 0, 0, 0);
		val = sj_object_get_value(e, "rotation");
		sj_get_float_value(sj_array_get_nth(val, 0), &rot.x);
		sj_get_float_value(sj_array_get_nth(val, 1), &rot.y);
		sj_get_float_value(sj_array_get_nth(val, 2), &rot.z);
		sj_get_float_value(sj_array_get_nth(val, 3), &rot.w);

		//Scale
		Vector4D scale = vector4d(0, 0, 0, 0);
		val = sj_object_get_value(e, "scale");
		sj_get_float_value(sj_array_get_nth(val, 0), &scale.x);
		sj_get_float_value(sj_array_get_nth(val, 1), &scale.y);
		sj_get_float_value(sj_array_get_nth(val, 2), &scale.z);
		sj_get_float_value(sj_array_get_nth(val, 3), &scale.w);

		//Color
		Vector4D color = vector4d(0, 0, 0, 0);
		val = sj_object_get_value(e, "color");
		sj_get_float_value(sj_array_get_nth(val, 0), &color.x);
		sj_get_float_value(sj_array_get_nth(val, 1), &color.y);
		sj_get_float_value(sj_array_get_nth(val, 2), &color.z);
		sj_get_float_value(sj_array_get_nth(val, 3), &color.w);

		//Type
		int type;
		val = sj_object_get_value(e, "type");
		sj_get_integer_value(val, &type);

		int eType;
		val = sj_object_get_value(e, "eType");
		sj_get_integer_value(val, &eType);

		//Create ent
		if (type == 0) newEnt = addCollctible(vector3d(pos.x, pos.y, pos.z));
		else if(type == 1) newEnt = addEntity(pos, rot, scale, color, vector3d(0, 0, 0), type, 0,1);
		else {
			//newEnt = addEntity(pos, rot, scale, color, vector3d(0, 0, 0), type, 0, eType);
			switch (eType) {
			case Chaser:
				addChaser(vector3d(pos.x,pos.y,pos.z));
				break;
			case Patroller:
				addPatrol(vector3d(pos.x, pos.y, pos.z),vector3d(0,0,-1));
				break;
			case Inspector:
				addHoleInspector(vector3d(pos.x, pos.y, pos.z));
				break;
			case Blind:
				addBlind(vector3d(pos.x, pos.y, pos.z));
				break;
			case Herd:
				addHerd(vector3d(pos.x, pos.y, pos.z));
				break;
			}
		}
	}
}
