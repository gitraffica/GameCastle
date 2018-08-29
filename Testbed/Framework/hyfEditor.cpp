#include "hyfEditor.h"
#include "RaEditor.h"
#include "glui/glui.h"

void HYFEditor::setModeNormal(string* md) {
	RaEditor* test2 = (RaEditor*)curTest;

	if (test2->type == "attack") {
		if (test2->rengineBody) {
			delete test2->rengineBody;
			test2->rengineBody = 0;
		}
	}
}

void HYFEditor::createButtons() {

}

void HYFEditor::MouseUp(const b2Vec2& p) {
	RaEditor* edit = (RaEditor*)curTest;
	if (edit->mode == "rengine") {
		RaEditor::mouse2 = RaEditor::mouse1 = p;
		Building* build = new Building();
		((RaEditor*)curTest)->setModeNormal((RaEditor*)curTest, &edit->mode);
		b2Body* body = Raffica::createBox(RaEditor::mouse2.x, RaEditor::mouse2.y, 2, 2);
		DefendUserData* ud = new DefendUserData();
		ud->defenderFunc = Raffica::rengineDefender;
		ud->width = 4, ud->height = 4;

		vector<int>* textures = new vector<int>();
		textures->push_back(LoadTexture("pic/rengine.bmp"));

		ud->Init(4300, 3005, textures);
		Raffica::setUserData(body, ud);

		build->body = body;
		build->name = "rengine";
		build->userdata = ud;

		Raffica::buildingList.push_back(build);
		return;
	}
}

void HYFEditor::MouseDown(const b2Vec2& p) {
	RaEditor* edit = (RaEditor*)curTest;
	if (edit->mode == "rengine") {
	}
}

void HYFEditor::Step2(Settings* settings) {												//单帧执行，图层比正常图层前面
	RaEditor* edit = (RaEditor*)curTest;
	b2Vec2 &mouse2 = edit->mouse2, &mouse1 = edit->mouse1;
	if (edit->mode == "rengine") {
		glLoadIdentity();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4f(0.5f, 0.5f, 0.5f, 0.5f);
		glBegin(GL_POLYGON);
		glTexCoord2f(mouse2.x - 2, mouse2.y - 2); glVertex2f(mouse2.x - 2, mouse2.y - 2);
		glTexCoord2f(mouse2.x - 2, mouse2.y + 2); glVertex2f(mouse2.x - 2, mouse2.y + 2);
		glTexCoord2f(mouse2.x + 2, mouse2.y + 2); glVertex2f(mouse2.x + 2, mouse2.y + 2);
		glTexCoord2f(mouse2.x + 2, mouse2.y - 2); glVertex2f(mouse2.x + 2, mouse2.y - 2);
		glEnd();
		glDisable(GL_BLEND);
	}
}