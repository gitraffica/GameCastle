#include "HYFAttackEditor.h"
#include "RaAttackEditor.h"



void HYFAttackEditor::rengineUp(const b2Vec2& p, string* mode, const RaButton* button) {
	if (*mode != "rengine") {
		SETNORMAL;
		*mode = "rengine";
		cout << *mode << endl;
		RaEditor::mouse2 = RaEditor::mouse1 = -p;

	}
	else {
		SETNORMAL;
	}
}

void HYFAttackEditor::attackerUp(const b2Vec2& p, string* mode, const RaButton* button) {
	vector<string> b1;
	b1.push_back("pic/rjrengine.bmp");
	b1.push_back("pic/rjrengine2.bmp");

	RaEditor::rengineBody = Raffica::createButton(-10, 14, 2, 1, b1, "rengine");

	RaEditor::rengineBody->setPressUp(rengineUp);
}

void HYFAttackEditor::createButtons2() {
	

}