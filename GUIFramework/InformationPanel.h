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

            void UpdateIndirectLighting(double value)
            {
                textBoxIndirectLighting->Text = value.ToString();
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
        private: System::Windows::Forms::TextBox^  textBoxIndirectLighting;
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
                this->textBoxIndirectLighting = (gcnew System::Windows::Forms::TextBox());
                this->SuspendLayout();
                // 
                // textBoxIndirectLighting
                // 
                this->textBoxIndirectLighting->Location = System::Drawing::Point(56, 64);
                this->textBoxIndirectLighting->Name = L"textBoxIndirectLighting";
                this->textBoxIndirectLighting->Size = System::Drawing::Size(100, 20);
                this->textBoxIndirectLighting->TabIndex = 0;
                // 
                // InformationPanel
                // 
                this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
                this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
                this->ClientSize = System::Drawing::Size(212, 499);
                this->Controls->Add(this->textBoxIndirectLighting);
                this->Name = L"InformationPanel";
                this->ResumeLayout(false);
                this->PerformLayout();

            }
#pragma endregion
        };
    }
}
