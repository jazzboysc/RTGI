#pragma once


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

                if( !mInfoPanel )
                {
                    mInfoPanel = this;
                }
            }

            void AddLabel(String^ labelName, int x, int y)
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

            void SetLabelValue(String^ labelName, double value)
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
            }

        protected:
            static InformationPanel^ mInfoPanel = nullptr;
        private: System::Windows::Forms::GroupBox^  groupBoxWorkloadTiming;
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
                this->SuspendLayout();
                // 
                // groupBoxWorkloadTiming
                // 
                this->groupBoxWorkloadTiming->Location = System::Drawing::Point(12, 12);
                this->groupBoxWorkloadTiming->Name = L"groupBoxWorkloadTiming";
                this->groupBoxWorkloadTiming->Size = System::Drawing::Size(214, 272);
                this->groupBoxWorkloadTiming->TabIndex = 0;
                this->groupBoxWorkloadTiming->TabStop = false;
                this->groupBoxWorkloadTiming->Text = L"Workload Timing (ms):";
                // 
                // InformationPanel
                // 
                this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
                this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
                this->ClientSize = System::Drawing::Size(238, 295);
                this->Controls->Add(this->groupBoxWorkloadTiming);
                this->Name = L"InformationPanel";
                this->ResumeLayout(false);

            }
#pragma endregion
        };
    }
}
