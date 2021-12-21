#include "MainForm.h"

using namespace System;
using namespace System::Windows::Forms;
using namespace SimpleTokenParser;
using namespace SkinSharp;

[STAThreadAttribute]
void Main(array<String^>^ args) {
	Application::SetCompatibleTextRenderingDefault(false);
	try {
		SkinH_Net^ skin = gcnew SkinH_Net();
		skin->AttachEx("Skin.she", "");
	}
	catch (...) {
		Application::EnableVisualStyles();
	}

	MainForm form;
	Application::Run(% form);
}