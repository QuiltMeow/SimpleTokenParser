#pragma once

#include <iostream>
#include <sstream>
#include <msclr/marshal_cppstd.h>
#include "TokenParser.h"
#include "TokenType.h"

namespace SimpleTokenParser {
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;
	using namespace ScintillaNET;

	public ref class MainForm sealed : public System::Windows::Forms::Form {
	public:
		MainForm() {
			InitializeComponent();
			initCodeTool();
			cbCategory->SelectedIndex = 0;
		}
	protected:
		~MainForm() {
#ifdef _FREE_OBJECT
			if (parser != NULL) {
				delete parser;
			}
#endif
			if (components) {
				delete components;
			}
		}

		TokenParser* parser = NULL;

		static std::string toNativeString(String^ data) {
			return msclr::interop::marshal_as<std::string>(data);
		}
	private:
		ScintillaNET::Scintilla^ txtCode;
		System::Windows::Forms::Label^ labelTitle;
		System::Windows::Forms::ListView^ lvResult;
		System::Windows::Forms::Label^ labelCategory;
		System::Windows::Forms::ComboBox^ cbCategory;
		System::Windows::Forms::Button^ btnParse;
		System::Windows::Forms::Label^ labelStatistic;
		System::Windows::Forms::ColumnHeader^ chName;
		System::Windows::Forms::ColumnHeader^ cbPosition;
		System::ComponentModel::Container^ components;

#pragma region Windows Form Designer generated code
		void InitializeComponent()
		{
			System::ComponentModel::ComponentResourceManager^ resources = (gcnew System::ComponentModel::ComponentResourceManager(MainForm::typeid));
			this->txtCode = (gcnew ScintillaNET::Scintilla());
			this->labelTitle = (gcnew System::Windows::Forms::Label());
			this->lvResult = (gcnew System::Windows::Forms::ListView());
			this->cbPosition = (gcnew System::Windows::Forms::ColumnHeader());
			this->chName = (gcnew System::Windows::Forms::ColumnHeader());
			this->labelCategory = (gcnew System::Windows::Forms::Label());
			this->cbCategory = (gcnew System::Windows::Forms::ComboBox());
			this->btnParse = (gcnew System::Windows::Forms::Button());
			this->labelStatistic = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			//
			// txtCode
			//
			this->txtCode->Location = System::Drawing::Point(40, 50);
			this->txtCode->Name = L"txtCode";
			this->txtCode->Size = System::Drawing::Size(405, 500);
			this->txtCode->TabIndex = 1;
			this->txtCode->Text = resources->GetString(L"txtCode.Text");
			//
			// labelTitle
			//
			this->labelTitle->AutoSize = true;
			this->labelTitle->Font = (gcnew System::Drawing::Font(L"微軟正黑體", 14.25F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(136)));
			this->labelTitle->Location = System::Drawing::Point(12, 9);
			this->labelTitle->Name = L"labelTitle";
			this->labelTitle->Size = System::Drawing::Size(147, 24);
			this->labelTitle->TabIndex = 0;
			this->labelTitle->Text = L"Token 解析工具";
			//
			// lvResult
			//
			this->lvResult->Columns->AddRange(gcnew cli::array< System::Windows::Forms::ColumnHeader^  >(2) { this->cbPosition, this->chName });
			this->lvResult->GridLines = true;
			this->lvResult->HideSelection = false;
			this->lvResult->Location = System::Drawing::Point(482, 85);
			this->lvResult->Name = L"lvResult";
			this->lvResult->Size = System::Drawing::Size(305, 400);
			this->lvResult->TabIndex = 4;
			this->lvResult->UseCompatibleStateImageBehavior = false;
			this->lvResult->View = System::Windows::Forms::View::Details;
			this->lvResult->MouseDoubleClick += gcnew System::Windows::Forms::MouseEventHandler(this, &MainForm::lvResult_MouseDoubleClick);
			//
			// cbPosition
			//
			this->cbPosition->Text = L"位置";
			this->cbPosition->Width = 100;
			//
			// chName
			//
			this->chName->Text = L"名稱";
			this->chName->Width = 195;
			//
			// labelCategory
			//
			this->labelCategory->AutoSize = true;
			this->labelCategory->Location = System::Drawing::Point(480, 50);
			this->labelCategory->Name = L"labelCategory";
			this->labelCategory->Size = System::Drawing::Size(53, 12);
			this->labelCategory->TabIndex = 2;
			this->labelCategory->Text = L"項目檢視";
			//
			// cbCategory
			//
			this->cbCategory->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->cbCategory->FormattingEnabled = true;
			this->cbCategory->Items->AddRange(gcnew cli::array< System::Object^  >(9) {
				L"符號", L"數學運算子", L"2 Token 運算子", L"邏輯運算子", L"關鍵字",
					L"註解", L"變數", L"整數與浮點數", L"辨識錯誤"
			});
			this->cbCategory->Location = System::Drawing::Point(550, 47);
			this->cbCategory->Name = L"cbCategory";
			this->cbCategory->Size = System::Drawing::Size(150, 20);
			this->cbCategory->TabIndex = 3;
			this->cbCategory->SelectedIndexChanged += gcnew System::EventHandler(this, &MainForm::cbCategory_SelectedIndexChanged);
			//
			// btnParse
			//
			this->btnParse->Location = System::Drawing::Point(712, 527);
			this->btnParse->Name = L"btnParse";
			this->btnParse->Size = System::Drawing::Size(75, 23);
			this->btnParse->TabIndex = 6;
			this->btnParse->Text = L"解析";
			this->btnParse->UseVisualStyleBackColor = true;
			this->btnParse->Click += gcnew System::EventHandler(this, &MainForm::btnParse_Click);
			//
			// labelStatistic
			//
			this->labelStatistic->AutoSize = true;
			this->labelStatistic->Location = System::Drawing::Point(480, 500);
			this->labelStatistic->Name = L"labelStatistic";
			this->labelStatistic->Size = System::Drawing::Size(167, 12);
			this->labelStatistic->TabIndex = 5;
			this->labelStatistic->Text = L"空白數 : 0，行數 : 0，Tab 數 : 0";
			//
			// MainForm
			//
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 12);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(814, 566);
			this->Controls->Add(this->labelStatistic);
			this->Controls->Add(this->btnParse);
			this->Controls->Add(this->cbCategory);
			this->Controls->Add(this->labelCategory);
			this->Controls->Add(this->lvResult);
			this->Controls->Add(this->labelTitle);
			this->Controls->Add(this->txtCode);
			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::FixedSingle;
			this->Icon = (cli::safe_cast<System::Drawing::Icon^>(resources->GetObject(L"$this.Icon")));
			this->ImeMode = System::Windows::Forms::ImeMode::Off;
			this->MaximizeBox = false;
			this->Name = L"MainForm";
			this->Text = L"Token 解析工具";
			this->ResumeLayout(false);
			this->PerformLayout();
		}
#pragma endregion

	private:
		void initCodeTool() {
			txtCode->StyleResetDefault();
			txtCode->Styles[Style::Default]->Font = "Consolas";
			txtCode->Styles[Style::Default]->Size = 10;
			txtCode->StyleClearAll();

			txtCode->Styles[Style::Cpp::Default]->ForeColor = Color::Silver;
			txtCode->Styles[Style::Cpp::Comment]->ForeColor = Color::FromArgb(0, 128, 0); // 綠
			txtCode->Styles[Style::Cpp::CommentLine]->ForeColor = Color::FromArgb(0, 128, 0);
			txtCode->Styles[Style::Cpp::CommentLineDoc]->ForeColor = Color::FromArgb(128, 128, 128); // 灰
			txtCode->Styles[Style::Cpp::Number]->ForeColor = Color::Olive;
			txtCode->Styles[Style::Cpp::Word]->ForeColor = Color::Blue;
			txtCode->Styles[Style::Cpp::Word2]->ForeColor = Color::Blue;
			txtCode->Styles[Style::Cpp::String]->ForeColor = Color::FromArgb(163, 21, 21); // 紅
			txtCode->Styles[Style::Cpp::Character]->ForeColor = Color::FromArgb(163, 21, 21);
			txtCode->Styles[Style::Cpp::Verbatim]->ForeColor = Color::FromArgb(163, 21, 21);
			txtCode->Styles[Style::Cpp::StringEol]->BackColor = Color::Pink;
			txtCode->Styles[Style::Cpp::Operator]->ForeColor = Color::Purple;
			txtCode->Styles[Style::Cpp::Preprocessor]->ForeColor = Color::Maroon;
			txtCode->Lexer = Lexer::Cpp;

			txtCode->SetKeywords(0, "abstract as base break case catch checked continue default delegate do else event explicit extern false finally fixed for foreach goto if implicit in interface internal is lock namespace new null object operator out override params private protected public readonly ref return sealed sizeof stackalloc switch this throw true try typeof unchecked unsafe using virtual while");
			txtCode->SetKeywords(1, "bool byte char class const decimal double enum float int long sbyte short static string struct uint ulong ushort void");
		}

		System::Void btnParse_Click(System::Object^ sender, System::EventArgs^ e) {
			std::string nativeString = toNativeString(txtCode->Text);
			std::stringstream ss(nativeString);

			if (parser != NULL) {
				delete parser;
			}
			parser = new TokenParser();
			parser->parse(ss);
			updateParseStatus();
		}

		void updateResultList(std::vector<RecognizeToken>& data) {
			lvResult->Items->Clear();
			for (int i = 0; i < data.size(); ++i) {
				RecognizeToken token = data[i];
				ListViewItem^ item = gcnew ListViewItem(gcnew array<String^>{ token.position.ToString(), gcnew String(token.token.c_str()) });
				lvResult->Items->Add(item);
			}
		}

		void updateStatistic(int blank, int line, int tab) {
			labelStatistic->Text = "空白數 : " + blank + "，行數 : " + line + "，Tab 數 : " + tab;
		}

		void updateParseStatus() {
			if (parser == NULL) {
				lvResult->Items->Clear();
				updateStatistic(0, 0, 0);
				return;
			}

			int index = cbCategory->SelectedIndex;
			std::vector<RecognizeToken>* token = NULL;
			switch (index) {
			case SYMBOL: {
				token = &parser->getRecognizeSymbol();
				break;
			}
			case MATH_OPERATOR: {
				token = &parser->getRecognizeMathOperator();
				break;
			}
			case TWO_TOKEN_OPERATOR: {
				token = &parser->getRecognizeTwoTokenOperator();
				break;
			}
			case LOGIC_OPERATOR: {
				token = &parser->getRecognizeLogicalOperator();
				break;
			}
			case KEY_WORD: {
				token = &parser->getRecognizeKeyword();
				break;
			}
			case COMMENT: {
				token = &parser->getRecognizeComment();
				break;
			}
			case VARIABLE: {
				token = &parser->getRecognizeId();
				break;
			}
			case NUMBER_TOKEN: {
				token = &parser->getRecognizeNumber();
				break;
			}
			case ERROR_TOKEN: {
				token = &parser->getError();
				break;
			}
			}
			updateResultList(*token);
			updateStatistic(parser->getBlankCount(), parser->getLineCount(), parser->geTtabCount());
		}

		System::Void cbCategory_SelectedIndexChanged(System::Object^ sender, System::EventArgs^ e) {
			updateParseStatus();
		}

		System::Void lvResult_MouseDoubleClick(System::Object^ sender, System::Windows::Forms::MouseEventArgs^ e) {
			if (lvResult->SelectedItems->Count == 1) {
				ListViewItem^ item = lvResult->SelectedItems[0];
				int position = Int32::Parse(item->SubItems[0]->Text);
				int length = item->SubItems[1]->Text->Length;

				txtCode->Focus();
				txtCode->SelectionStart = position;
				txtCode->SelectionEnd = position + length;
				txtCode->ScrollCaret();
			}
		}
	};
}