#pragma once

#include "FormEventListener.h"
#include <vector>

namespace RTGI {
    namespace GUIFramework {

        using namespace System;
        using namespace System::ComponentModel;
        using namespace System::Collections;
        using namespace System::Windows::Forms;
        using namespace System::Data;
        using namespace System::Drawing;

        /// <summary>
        /// Summary for InformationPanel
        /// </summary>
        public ref class InformationPanel : public System::Windows::Forms::Form
        {
        public:
            InformationPanel(void)
            {
                InitializeComponent();

                mListeners = new std::vector<FormEventListener*> ;
                if( !mInfoPanel )
                {
                    mInfoPanel = this;
                }
            }

            void AddListener(FormEventListener* listener)
            {
                mListeners->push_back(listener);
            }

            void AddButton(String^ buttonName, int x, int y, int width, int height)
            {
                Button^ button = gcnew Button();

                button->Location = System::Drawing::Point(x, y);
                button->Name = buttonName;
                button->Size = System::Drawing::Size(width, height);
                button->TabIndex = 0;
                button->Text = buttonName;
                button->UseVisualStyleBackColor = true;
                button->Click += gcnew System::EventHandler(this, &InformationPanel::button_Click);

                groupBoxUserInputs->Controls->Add(button);
            }

            void AddRadioButton(String^ radioButtonName, int x, int y, int width, int height, bool checked)
            {
                RadioButton^ radioButton = gcnew RadioButton();

                radioButton->AutoSize = true;
                radioButton->Location = System::Drawing::Point(x, y);
                radioButton->Name = radioButtonName;
                radioButton->Size = System::Drawing::Size(width, height);
                radioButton->TabIndex = 1;
                radioButton->TabStop = true;
                radioButton->Text = radioButtonName;
                radioButton->UseVisualStyleBackColor = true;
                radioButton->Checked = checked;
                radioButton->Click += gcnew System::EventHandler(this, &InformationPanel::radioButton_Click);

                groupBoxUserInputs->Controls->Add(radioButton);
            }

            void AddTimingLabel(String^ labelName, int x, int y)
            {
                Label^ label = gcnew Label();

                label->AutoSize = true;
                label->Location = System::Drawing::Point(x, y);
                label->Name = labelName;
                label->Text = labelName;
                label->Size = System::Drawing::Size(32, 13);
                label->TabIndex = 0;

                groupBoxWorkloadTiming->Controls->Add(label);
            }

            void AddDebugLabel(String^ labelName, int x, int y)
            {
                Label^ label = gcnew Label();

                label->AutoSize = true;
                label->Location = System::Drawing::Point(x, y);
                label->Name = labelName;
                label->Text = labelName;
                label->Size = System::Drawing::Size(32, 13);
                label->TabIndex = 0;

                groupBoxDebugOutputs->Controls->Add(label);
            }

            void AddTextBox(String^ textBoxName, int x, int y, int width, int height)
            {
                Label^ label = gcnew Label();
                label->AutoSize = true;
                label->Location = System::Drawing::Point(x, y);
                label->Name = textBoxName;
                label->Text = textBoxName;
                label->Size = System::Drawing::Size(32, 13);
                label->TabIndex = 0;
                groupBoxUserInputs->Controls->Add(label);

                TextBox^ textBox = gcnew TextBox();
                int x2 = int(x + textBoxName->Length + 24);
                textBox->Location = System::Drawing::Point(x2, y);
                textBox->Name = textBoxName;
                textBox->Size = System::Drawing::Size(width, height);
                textBox->TabIndex = 1;

                groupBoxUserInputs->Controls->Add(textBox);
            }

            String^ GetTextBoxValue(String^ textBoxName)
            {
                array<Control^, 1>^ res = groupBoxUserInputs->Controls->Find(textBoxName, true);
                if( res->Length == 0 )
                {
                    return nullptr;
                }

                TextBox^ textBox = (TextBox^)res[1];
                if( textBox )
                {
                    return textBox->Text;
                }
                else
                {
                    return nullptr;
                }
            }

            void SetTimingLabelValue(String^ labelName, double value)
            {
                array<Control^, 1>^ res = groupBoxWorkloadTiming->Controls->Find(labelName, true);
                if( res->Length == 0 )
                {
                    return;
                }

                Label^ label = (Label^)res[0];
                if( label )
                {
                    label->Text = label->Name + ": " + value.ToString();
                }
            }

            void SetDebugLabelValue(String^ labelName, double value)
            {
                array<Control^, 1>^ res = groupBoxDebugOutputs->Controls->Find(labelName, true);
                if( res->Length == 0 )
                {
                    return;
                }

                Label^ label = (Label^)res[0];
                if( label )
                {
                    label->Text = label->Name + ": " + value.ToString();
                }
            }

            static InformationPanel^ GetInstance()
            {
                return mInfoPanel;
            }

        protected:
            /// <summary>
            /// Clean up any resources being used.
            /// </summary>
            ~InformationPanel()
            {
                if( components )
                {
                    delete components;
                }
                delete mListeners;
            }

        protected:
            static InformationPanel^ mInfoPanel = nullptr;
            std::vector<FormEventListener*>* mListeners;

        private: System::Windows::Forms::GroupBox^  groupBoxWorkloadTiming;
        private: System::Windows::Forms::GroupBox^  groupBoxUserInputs;
private: System::Windows::Forms::GroupBox^  groupBoxDebugOutputs;





        protected:

        protected:

        protected:

        private:
            /// <summary>
            /// Required designer variable.
            /// </summary>
            System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
            /// <summary>
            /// Required method for Designer support - do not modify
            /// the contents of this method with the code editor.
            /// </summary>
            void InitializeComponent(void)
            {
                this->groupBoxWorkloadTiming = (gcnew System::Windows::Forms::GroupBox());
                this->groupBoxUserInputs = (gcnew System::Windows::Forms::GroupBox());
                this->groupBoxDebugOutputs = (gcnew System::Windows::Forms::GroupBox());
                this->SuspendLayout();
                // 
                // groupBoxWorkloadTiming
                // 
                this->groupBoxWorkloadTiming->Location = System::Drawing::Point(12, 12);
                this->groupBoxWorkloadTiming->Name = L"groupBoxWorkloadTiming";
                this->groupBoxWorkloadTiming->Size = System::Drawing::Size(214, 339);
                this->groupBoxWorkloadTiming->TabIndex = 0;
                this->groupBoxWorkloadTiming->TabStop = false;
                this->groupBoxWorkloadTiming->Text = L"Workload Timing (ms):";
                // 
                // groupBoxUserInputs
                // 
                this->groupBoxUserInputs->Location = System::Drawing::Point(12, 357);
                this->groupBoxUserInputs->Name = L"groupBoxUserInputs";
                this->groupBoxUserInputs->Size = System::Drawing::Size(214, 392);
                this->groupBoxUserInputs->TabIndex = 1;
                this->groupBoxUserInputs->TabStop = false;
                this->groupBoxUserInputs->Text = L"User Inputs :";
                // 
                // groupBoxDebugOutputs
                // 
                this->groupBoxDebugOutputs->Location = System::Drawing::Point(232, 12);
                this->groupBoxDebugOutputs->Name = L"groupBoxDebugOutputs";
                this->groupBoxDebugOutputs->Size = System::Drawing::Size(214, 737);
                this->groupBoxDebugOutputs->TabIndex = 2;
                this->groupBoxDebugOutputs->TabStop = false;
                this->groupBoxDebugOutputs->Text = L"Debug Outputs :";
                // 
                // InformationPanel
                // 
                this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
                this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
                this->ClientSize = System::Drawing::Size(459, 761);
                this->Controls->Add(this->groupBoxDebugOutputs);
                this->Controls->Add(this->groupBoxUserInputs);
                this->Controls->Add(this->groupBoxWorkloadTiming);
                this->Name = L"InformationPanel";
                this->ResumeLayout(false);

            }
#pragma endregion
        private: System::Void button_Click(System::Object^  sender, System::EventArgs^  e) {
            for( int i = 0; i < (int)mListeners->size(); ++i )
            {
                (*mListeners)[i]->OnButtonClick(sender, e);
            }
        }
        private: System::Void radioButton_Click(System::Object^  sender, System::EventArgs^  e) {
            for( int i = 0; i < (int)mListeners->size(); ++i )
            {
                (*mListeners)[i]->OnRadioButtonClick(sender, e);
            }
        }
};
    }
}
