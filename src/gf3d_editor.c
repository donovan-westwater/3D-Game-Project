#include "gf3d_editor.h"
#include "gf3d_vgraphics.h"

EditorManger static editor = { 0 };
static int spaceDown = false;
static int backDown = false;
static int tabDown = false;
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
			n = addEntity(vector4d(0, 2, 0, 1), vector4d(0, 0, 0, 1), vector4d(1, 1, 1, 1), vector4d(0, 0.5, 0.5, 1), vector3d(0, 0, 0), 1, 0);
		}
		else {
			n = addEntity(editor.selEnt->rSelf->position, editor.selEnt->rSelf->rotation, editor.selEnt->rSelf->scale, editor.selEnt->rSelf->position, vector3d(0, 0, 0), 1, 0);
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
	else if (!keys[SDL_SCANCODE_TAB] && tabDown) tabDown = false;

}

